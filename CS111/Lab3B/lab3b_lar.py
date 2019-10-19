import sys
import csv
import string

exit_status = 0;

class Superblock:
    def __init__(self, row):
        self.total_blocks = int(row[1])
        self.total_inodes = int(row[2])
        self.block_size = int(row[3])
        self.inode_size = int(row[4])
        self.blocks_per_group = int(row[5])
        self.inodes_per_group = int(row[6])
        self.first_non_reserved_inode = int(row[7])

class Inode:
    def __init__(self, row):
        self.inode_num = int(row[1])
        self.file_type = row[2]
        self.mode = row[3]
        self.owner = int(row[4])
        self.group = int(row[5])
        self.link_count = int(row[6])
        self.ctime = row[7]
        self.mtime = row[8]
        self.atime = row[9]
        self.file_size = int(row[10])
        self.num_blocks = int(row[11])
        self.blocks = [int(row[i]) for i in range(12,24)]
        self.indirects = [int(row[i]) for i in range(24,27)]

class Dirent:
    def __init__(self, row):
        self.parent_inode_num = int(row[1])
        self.byte_offset = int(row[2])
        self.inode = int(row[3])
        self.rec_len = int(row[4])
        self.name_len = int(row[5])
        self.name = row[6]

class Indirect:
    def __init__(self, row):
        self.owning_inode_num = int(row[1])
        self.indirection_level = int(row[2])
        self.block_offset = int(row[3])
        self.block_num = int(row[4])
        self.reference_num = int(row[5])

def block_consistency_audit(superblock, group, free_blocks, inodes, indirects):
    blocks = {}
    
    for inode in inodes:
        offset = 0
        for block in inode.blocks:
            if block != 0:
                if block >= superblock.total_blocks or block < 0:
                    exit_status = 2
                    print("INVALID BLOCK {} IN INODE {} AT OFFSET {}".format(block, inode.inode_num, offset))
                elif block < 8:
                    exit_status = 2
                    print("RESERVED BLOCK {} IN INODE {} AT OFFSET {}".format(block, inode.inode_num, offset))
                elif block in blocks:
                    blocks[block].append(("BLOCK", inode.inode_num, offset))
                else:
                    blocks[block] = [("BLOCK", inode.inode_num, offset)]
            offset += 1
        
        block_type = None
        for i in range(3):
            if i == 0:
                block_type = "INDIRECT BLOCK"
                offset = 12
            elif i == 1:
                block_type = "DOUBLE INDIRECT BLOCK"
                offset = 268
            elif i == 2:
                block_type = "TRIPLE INDIRECT BLOCK"
                offset = 65804
            block = inode.indirects[i]
            if block != 0:
                if block >= superblock.total_blocks or block < 0:
                    exit_status = 2
                    print("INVALID {} {} IN INODE {} AT OFFSET {}".format(block_type, block, inode.inode_num, offset))
                elif block < 8:
                    exit_status = 2
                    print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(block_type, block, inode.inode_num, offset))
                elif block in blocks:
                    blocks[block].append((block_type, inode.inode_num, offset))
                else:
                    blocks[block] = [(block_type, inode.inode_num, offset)]

for indirect in indirects:
    block_type = None
        if indirect.indirection_level == 1:
            block_type = "BLOCK"
    elif indirect.indirection_level == 2:
        block_type = "INDIRECT BLOCK"
        elif indirect.indirection_level == 3:
            block_type = "DOUBLE INDIRECT BLOCK"
block = indirect.reference_num
    if block != 0:
        if block >= superblock.total_blocks or block < 0:
            exit_status = 2
                print("INVALID {} {} IN INODE {} AT OFFSET {}".format(block_type, block, indirect.owning_inode_num, indirect.block_offset))
            elif block < 8:
                exit_status = 2
                print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(block_type, block, indirect.owning_inode_num, indirect.block_offset))
        elif block in blocks:
            blocks[block].append((block_type, indirect.owning_inode_num, offset))
            else:
                blocks[block] = [(block_type, indirect.owning_inode_num, offset)]

for block in range(8,superblock.total_blocks):
    if block not in free_blocks and block not in blocks:
        exit_status = 2
            print("UNREFERENCED BLOCK {}".format(block))
        elif block in free_blocks and block in blocks:
            exit_status = 2
            print("ALLOCATED BLOCK {} ON FREELIST".format(block))
    if block in blocks and len(blocks[block]) > 1:
        for block_type, inode, offset in blocks[block]:
            exit_status = 2
                print("DUPLICATE {} {} IN INODE {} AT OFFSET {}".format(block_type, block, inode, offset))

def inode_allocation_audit(superblock, free_inodes, inodes):
    allocated_inodes = {inode.inode_num for inode in inodes}
    
    for inode in allocated_inodes:
        if inode in free_inodes:
            exit_status = 2
            print("ALLOCATED INODE {} ON FREELIST".format(inode))

for i in range(superblock.first_non_reserved_inode, superblock.total_inodes):
    if i not in allocated_inodes and i not in free_inodes:
        exit_status = 2
            print("UNALLOCATED INODE {} NOT ON FREELIST".format(i))

return allocated_inodes

def directory_consistency_audit(superblock, inodes, dirents, allocated_inodes):
    link_count = [0] * (superblock.total_inodes + 1)
    parent = [0] * (superblock.total_inodes + 1)
    parent[2] = 2 #parent of root directory is root itself
    
    for dirent in dirents:
        if dirent.inode > superblock.total_inodes or dirent.inode < 1:
            exit_status = 2
            print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(dirent.parent_inode_num, dirent.name, dirent.inode))
        elif dirent.inode not in allocated_inodes:
            exit_status = 2
            print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(dirent.parent_inode_num, dirent.name, dirent.inode))
        else:
            link_count[dirent.inode] += 1
            if dirent.name != "'.'" and dirent.name != "'..'":
                parent[dirent.inode] = dirent.parent_inode_num

for inode in inodes:
    if inode.link_count != link_count[inode.inode_num]:
        exit_status = 2
            print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(inode.inode_num, link_count[inode.inode_num], inode.link_count))

for dirent in dirents:
    if dirent.name == "'.'" and dirent.parent_inode_num != dirent.inode:
        exit_status = 2
            print("DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(dirent.parent_inode_num, dirent.inode, dirent.parent_inode_num))
        if dirent.name == "'..'" and parent[dirent.parent_inode_num] != dirent.inode:
            exit_status = 2
            print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(dirent.parent_inode_num, dirent.inode, dirent.parent_inode_num))

def main():
    if len(sys.argv) != 2:
        print("Error: Invalid arguments. Correct usage: ./lab3b CSV_FILENAME", file=sys.stderr)
        sys.exit(1)
    
    superblock = None
    group = None
    free_blocks = []
    free_inodes = []
    inodes = []
    dirents = []
    indirects = []

try:
    with open(sys.argv[1], 'r') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row[0] == 'SUPERBLOCK':
                superblock = Superblock(row)
                elif row[0] == 'BFREE':
                    free_blocks.append(int(row[1]))
            elif row[0] == 'IFREE':
                free_inodes.append(int(row[1]))
                elif row[0] == 'INODE':
                    inodes.append(Inode(row))
            elif row[0] == 'DIRENT':
                dirents.append(Dirent(row))
                elif row[0] == 'INDIRECT':
                    indirects.append(Indirect(row))
except (IOError, OSError):
    print("Error: Could not read file, no such file or directory", file=sys.stderr)
    sys.exit(1)
    
    block_consistency_audit(superblock, group, free_blocks, inodes, indirects)
    allocated_inodes = inode_allocation_audit(superblock, free_inodes, inodes)
    directory_consistency_audit(superblock, inodes, dirents, allocated_inodes)
    
    sys.exit(exit_status)

if __name__ == "__main__":
    main()
