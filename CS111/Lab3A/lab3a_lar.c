#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "ext2_fs.h"

const char *fs;
int fs_fd = -1;

uint32_t block_size;

void error_handler(char* func, int err) {
    fprintf(stderr, "Error in %s: (%d)%s\n", func, err, strerror(err));
    exit(1);
}

void clean_up() {
    close(fs_fd);
}

void indirect_reference_summary(unsigned int inode_num, unsigned int block_offset, unsigned int indirection_level, uint32_t block_number) {
    uint32_t reference_number;
    for (unsigned int i = 0; i < block_size/sizeof(uint32_t); i++) {
        if (pread(fs_fd, &reference_number, sizeof(uint32_t), block_number*block_size + i*sizeof(uint32_t)) == -1) {
            error_handler("pread()", errno);
        }
        if (reference_number != 0) {
            unsigned int curr_offset = block_offset+(unsigned int)(i*pow(256, indirection_level-1));
            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_num, indirection_level, curr_offset, block_number, reference_number);
            if (indirection_level > 1) {
                indirect_reference_summary(inode_num, curr_offset, indirection_level-1, reference_number);
            }
        }
    }
}

unsigned int directory_entry_summary(unsigned int inode_num, unsigned int byte_offset, uint32_t block_number) {
    unsigned int byte_number = block_number * block_size;
    struct ext2_dir_entry dir_entry;
    while (byte_number < (block_number+1)*block_size) {
        if (pread(fs_fd, &dir_entry, sizeof(struct ext2_dir_entry), byte_number) == -1) {
            error_handler("pread()", errno);
        }
        if (dir_entry.inode != 0) {
            fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,\'%s\'\n", inode_num, byte_offset, dir_entry.inode, dir_entry.rec_len, dir_entry.name_len, dir_entry.name);
        }
        byte_number += dir_entry.rec_len;
        byte_offset += dir_entry.rec_len;
    }
    return byte_offset;
}

unsigned int indirect_directory_entry_summary(unsigned int inode_num, unsigned int byte_offset, unsigned int indirection_level, uint32_t block_number) {
    uint32_t reference_number;
    for (unsigned int i = 0; i < block_size/sizeof(uint32_t); i++) {
        if (pread(fs_fd, &reference_number, sizeof(uint32_t), block_number*block_size + i*sizeof(uint32_t)) == -1) {
            error_handler("pread()", errno);
        }
        if (indirection_level > 0) {
            if (reference_number != 0) {
                unsigned int next_offset = directory_entry_summary(inode_num, byte_offset, reference_number);
                byte_offset = next_offset;
            }
        } else {
            if (reference_number != 0) {
                unsigned int next_offset = indirect_directory_entry_summary(inode_num, byte_offset, indirection_level - 1, reference_number);
                byte_offset = next_offset;
            }
        }
    }
    return byte_offset;
}

void directory_entries_handler(unsigned int inode_num, struct ext2_inode inode) {
    unsigned int byte_offset = 0;
    for (int i = 0; i < 12; i++) {
        if (inode.i_block[i] != 0) {
            unsigned int next_offset = directory_entry_summary(inode_num, byte_offset, inode.i_block[i]);
            byte_offset = next_offset;
        }
    }
    for (unsigned int i = 12; i < 15; i++) {
        if (inode.i_block[i] != 0) {
            unsigned int next_offset = indirect_directory_entry_summary(inode_num, byte_offset, i-11, inode.i_block[i]);
            byte_offset = next_offset;
        }
    }
}

void inodes_summary(uint32_t inodes_table, uint32_t total_inodes, uint32_t inode_size) {
    struct ext2_inode inode;
    for (unsigned int i = 0; i < total_inodes; i++) {
        if (pread(fs_fd, &inode, inode_size, inodes_table*block_size + inode_size*i) == -1) {
            error_handler("pread()", errno);
        }
        if (inode.i_mode == 0 || inode.i_links_count == 0)
            continue;
        
        char file_type = '?';
        if (inode.i_mode & 0x4000) {
            file_type = 'd';
            directory_entries_handler(i+1, inode);
            indirect_reference_summary(i+1, 12, 1, inode.i_block[12]);
            indirect_reference_summary(i+1, 268, 2, inode.i_block[13]);
            indirect_reference_summary(i+1, 65804, 3, inode.i_block[14]);
        }
        else if (inode.i_mode & 0x8000) {
            file_type = 'f';
            indirect_reference_summary(i+1, 12, 1, inode.i_block[12]);
            indirect_reference_summary(i+1, 268, 2, inode.i_block[13]);
            indirect_reference_summary(i+1, 65804, 3, inode.i_block[14]);
        } else if (inode.i_mode & 0xA000)
            file_type = 's';
        
        uint16_t owner = inode.i_uid;
        uint16_t group = inode.i_gid;
        uint16_t link_count = inode.i_links_count;
        
        time_t time;
        
        char created[18];
        time = inode.i_ctime;
        struct tm* ctime = gmtime(&time);
        strftime(created, 18, "%D %H:%M:%S", ctime);
        
        char modified[18];
        time = inode.i_mtime;
        struct tm* mtime = gmtime(&time);
        strftime(modified, 18, "%D %H:%M:%S", mtime);
        
        char accessed[18];
        time = inode.i_atime;
        struct tm* atime = gmtime(&time);
        strftime(accessed, 18, "%D %H:%M:%S", atime);
        
        uint32_t size = inode.i_size;
        uint32_t blocks = inode.i_blocks;
        
        fprintf(stdout, "INODE,%u,%c,%o,%u,%u,%u,%s,%s,%s,%d,%d", i+1, file_type, inode.i_mode & 0xFFF, owner, group, link_count, created, modified, accessed, size, blocks);
        for (int j = 0; j < 15; j++) {
            fprintf(stdout, ",%d", inode.i_block[j]);
        }
        fprintf(stdout, "\n");
    }
}

void scan_free_inodes(uint32_t inode_bitmap, uint32_t total_inodes) {
    uint8_t byte = 0;
    for (unsigned int i = 0; i < total_inodes; i++) {
        if (pread(fs_fd, &byte, 1, inode_bitmap*block_size+i) == -1) {
            error_handler("pread()", errno);
        }
        for (unsigned int j = 0; j < 8; j++) {
            if ((byte & (1<<j)) == 0) {
                fprintf(stdout, "IFREE,%u\n", i*8+j+1);
            }
        }
    }
}

void scan_free_blocks(uint32_t block_bitmap, uint32_t total_blocks) {
    uint8_t byte = 0;
    for (unsigned int i = 0; i < total_blocks; i++) {
        if (pread(fs_fd, &byte, 1, block_bitmap*block_size+i) == -1) {
            error_handler("pread()", errno);
        }
        for (unsigned int j = 0; j < 8; j++) {
            if ((byte & (1<<j)) == 0) {
                fprintf(stdout, "BFREE,%u\n", i*8+j+1);
            }
        }
    }
}

void group_summary(uint32_t total_blocks, uint32_t total_inodes, uint32_t inode_size) {
    struct ext2_group_desc group;
    
    if (pread(fs_fd, &group, sizeof(struct ext2_group_desc), 1024+sizeof(struct ext2_super_block)) == -1) {
        error_handler("pread()", errno);
    }
    
    uint16_t num_free_blocks = group.bg_free_blocks_count;
    uint16_t num_free_inodes = group.bg_free_inodes_count;
    uint32_t block_bitmap = group.bg_block_bitmap;
    uint32_t inode_bitmap = group.bg_inode_bitmap;
    uint32_t first_block_inodes = group.bg_inode_table;
    
    fprintf(stdout, "GROUP,0,%u,%u,%u,%u,%u,%u,%u\n", total_blocks, total_inodes, num_free_blocks, num_free_inodes, block_bitmap, inode_bitmap, first_block_inodes);
    
    scan_free_blocks(block_bitmap, total_blocks);
    scan_free_inodes(inode_bitmap, total_inodes);
    inodes_summary(first_block_inodes, total_inodes, inode_size);
}

void superblock_summary() {
    struct ext2_super_block superblock;
    
    if (pread(fs_fd, &superblock, sizeof(struct ext2_super_block), 1024) == -1) {
        error_handler("pread()", errno);
    }
    
    uint32_t total_blocks = superblock.s_blocks_count;
    uint32_t total_inodes = superblock.s_inodes_count;
    block_size = EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
    uint32_t inode_size = superblock.s_inode_size;
    uint32_t blocks_per_group =    superblock.s_blocks_per_group;
    uint32_t inodes_per_group =    superblock.s_inodes_per_group;
    uint32_t first_non_reserved_inode = superblock.s_first_ino;
    
    fprintf(stdout, "SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", total_blocks, total_inodes, block_size, inode_size, blocks_per_group, inodes_per_group, first_non_reserved_inode);
    
    group_summary(total_blocks, total_inodes, inode_size);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: Wrong number of arguments. Usage: ./lab3a FILE_SYSTEM_IMG\n");
        exit(1);
    }
    
    fs = argv[1];
    if ((fs_fd = open(fs, O_RDONLY)) == -1) {
        error_handler("open()", errno);
    }
    
    if (atexit(clean_up) == -1) {
        error_handler("atexit()", errno);
    }
    
    superblock_summary();
    
    exit(0);
}
