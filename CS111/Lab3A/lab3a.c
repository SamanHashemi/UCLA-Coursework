//NAME: Saman Hashemipour
//EMAIL: hash.saman@gmail.com
//ID: 904903562

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
int fs_filedir = -1;

uint32_t blk_size;

void errorExit(char* msg){
    fprintf(stderr, msg);
    exit(1);
}

void close_dir() {
    close(fs_filedir);
}

void printIndirect(unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4, unsigned int d5){
    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", d1, d2, d3, d4, d5);
}

void printDirent(unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4, unsigned int d5, char* s1){
    fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,\'%s\'\n", d1, d2, d3, d4, d5, s1);
}

void indirRefSummary(unsigned int in_num, unsigned int offset, unsigned int indr_level, uint32_t block_num) {
    uint32_t ref_num;
    unsigned int i = 0;
    unsigned int curr_offset;
    while(i < blk_size/sizeof(uint32_t)){
        int temp = pread(fs_filedir, &ref_num, sizeof(uint32_t), block_num*blk_size + i*sizeof(uint32_t));
        if (temp == -1) {
            errorExit("Error: pread()");
        }
        if (ref_num != 0) {
            curr_offset = offset+(unsigned int)(i*pow(256, indr_level-1));
            printIndirect(in_num, indr_level, curr_offset, block_num, ref_num);
            if (indr_level > 1){
                indirRefSummary(in_num, curr_offset, indr_level-1, ref_num);
            }
        }
        i++;
    }
}

unsigned int dirEntrySummary(unsigned int in_num, unsigned int byte_off, uint32_t block_num) {
    unsigned int len;
    unsigned int offset = byte_off;
    unsigned int byte_num = block_num * blk_size;
    unsigned int total_blocks = (block_num+1)*blk_size;
    struct ext2_dir_entry dir_entry;
    while (byte_num < total_blocks) {
        int temp = pread(fs_filedir, &dir_entry, sizeof(struct ext2_dir_entry), byte_num);
        if (temp == -1) {
            errorExit("Error: pread()");
        }
        len = dir_entry.rec_len;
        if (dir_entry.inode != 0) {
            printDirent(in_num, offset, dir_entry.inode, len, dir_entry.name_len, dir_entry.name);
        }
        offset += len;
        byte_num += len;
    }
    return offset;
}

unsigned int indirDirEntrySummary(unsigned int in_num, unsigned int offset, unsigned int indr_level, uint32_t block_num) {
    uint32_t ref_num;
    int temp;
    unsigned int i = 0;
    while(i < blk_size/sizeof(uint32_t)){
        temp = pread(fs_filedir, &ref_num, sizeof(uint32_t), block_num*blk_size + i*sizeof(uint32_t));
        if (temp == -1) {
            errorExit("Error: pread()");
        }
        if(indr_level <= 0) {
            if (ref_num != 0) {
                offset = indirDirEntrySummary(in_num, offset, indr_level - 1, ref_num);
            }
        }
        else if (indr_level > 0) {
            if (ref_num != 0) {
                offset = dirEntrySummary(in_num, offset, ref_num);
            }
        }
        i++;
    }
    return offset;
}

void superblockSummary() {
    struct ext2_super_block superblock;
    struct ext2_group_desc group;
    int temp = pread(fs_filedir, &superblock, sizeof(struct ext2_super_block), 1024);
    if (temp == -1) {
        errorExit("Error: pread()");
    }
    blk_size = EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
    
    fprintf(stdout, "SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", (uint32_t)superblock.s_blocks_count, (uint32_t)superblock.s_inodes_count, blk_size, (uint32_t)superblock.s_inode_size, (uint32_t)superblock.s_blocks_per_group, (uint32_t)superblock.s_inodes_per_group, (uint32_t)superblock.s_first_ino);
    
    temp = pread(fs_filedir, &group, sizeof(struct ext2_group_desc), 1024+sizeof(struct ext2_super_block));
    if (temp == -1) {
        errorExit("Error: pread()");
    }
    
    fprintf(stdout, "GROUP,0,%u,%u,%u,%u,%u,%u,%u\n", (uint32_t)superblock.s_blocks_count, (uint32_t)superblock.s_inodes_count, group.bg_free_blocks_count, group.bg_free_inodes_count, group.bg_block_bitmap, group.bg_inode_bitmap, group.bg_inode_table);
    
    // Scan for free blocks
    uint8_t c_byte = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    while(i < (uint32_t)superblock.s_blocks_count){
        temp = pread(fs_filedir, &c_byte, 1, (uint32_t)group.bg_block_bitmap*blk_size+i);
        if (temp == -1) {
            errorExit("Error: pread()");
        }
        j = 0;
        while(j < 8){
            if ((c_byte & (1<<j)) == 0) {
                fprintf(stdout, "BFREE,%u\n", i*8+j+1);
            }
            j++;
        }
        i++;
    }
    
    // Scan for free inodes
    c_byte = 0;
    i = 0;
    while(i < (uint32_t)superblock.s_inodes_count){
        temp = pread(fs_filedir, &c_byte, 1, group.bg_inode_bitmap*blk_size+i);
        if (temp == -1) {
            errorExit("Error: pread()");
        }
        j = 0;
        while(j < 8){
            if ((c_byte & (1<<j)) == 0) {
                fprintf(stdout, "IFREE,%u\n", i*8+j+1);
            }
            j++;
        }
        i++;
    }
    
    // Inode Summary
    struct ext2_inode inode;
    i = 0;
    while(i < (uint32_t)superblock.s_inodes_count){
        if (pread(fs_filedir, &inode, (uint32_t)superblock.s_inode_size, ((uint32_t)group.bg_inode_table)*blk_size + ((uint32_t)superblock.s_inode_size)*i) == -1) {
            errorExit("Error: pread()");
        }
        if (inode.i_mode == 0 || inode.i_links_count == 0){
            i++;
            continue;
        }
        char f_type = '?';
        if (inode.i_mode & 0x4000)
            f_type = 'd';
        else if (inode.i_mode & 0x8000)
            f_type = 'f';
        else if (inode.i_mode & 0xA000)
            f_type = 's';
        
        
        int i1 = i+1;
        switch(f_type){
            case 'f':
                indirRefSummary(i1, 12+256+65536, 3, inode.i_block[14]);
                indirRefSummary(i1, 12+256, 2, inode.i_block[13]);
                indirRefSummary(i1, 12, 1, inode.i_block[12]);
                break;
            case 'd': ;
                unsigned int byte_offset = 0;
                unsigned int m = 0;
                unsigned int n = 12;
                while(m < 12){
                    if (inode.i_block[m] != 0) {
                        byte_offset = dirEntrySummary(i1, byte_offset, inode.i_block[m]);
                    }
                    m++;
                }
                while(n < 15){
                    if (inode.i_block[n] != 0) {
                        byte_offset = indirDirEntrySummary(i1, byte_offset, n-11, inode.i_block[n]);
                    }
                    n++;
                }
                indirRefSummary(i1, 12+256+65536, 3, inode.i_block[14]);
                indirRefSummary(i1, 268, 2, inode.i_block[13]);
                indirRefSummary(i1, 12, 1, inode.i_block[12]);
                break;
        }
        
        time_t rn;
        
        int size = 18;
        char accessed[size];
        rn = inode.i_atime;
        struct tm* acc_time = gmtime(&rn);
        strftime(accessed, size, "%D %H:%M:%S", acc_time);
        
        char modified[size];
        rn = inode.i_mtime;
        struct tm* mod_time = gmtime(&rn);
        strftime(modified, size, "%D %H:%M:%S", mod_time);
        
        char created[size];
        rn = inode.i_ctime;
        struct tm* creat_time = gmtime(&rn);
        strftime(created, size, "%D %H:%M:%S", creat_time);
        
        fprintf(stdout, "INODE,%u,%c,%o,%u,%u,%u,%s,%s,%s,%d,%d", i+1, f_type, inode.i_mode & 0xFFF, inode.i_uid, inode.i_gid, inode.i_links_count, created, modified, accessed, inode.i_size, inode.i_blocks);
        j = 0;
        while(j < 15){
            fprintf(stdout, ",%d", inode.i_block[j]);
            j++;
        }
        fprintf(stdout, "\n");
        i++;
    }
}



int main(int argc, char **argv) {
    if (argc != 2) {
        errorExit("Error: Number of arguments was incorrect.");
    }
    
    fs = argv[1];
    fs_filedir = open(fs, O_RDONLY);
    if (fs_filedir == -1) {
        errorExit("Error: open()");
    }
    
    if (atexit(close_dir) == -1) {
        errorExit("Error: atexit()");
    }
    
    superblockSummary();
    
    exit(0);
}
