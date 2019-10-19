#!/usr/local/cs/bin/python3

import sys
import csv

exitcode = 0


class Superblock:
    def __init__(self, row):
        values = []
        for i in range(1, 8):
            values.append(int(row[i]))
        self.total_blocks = int(values[0])
        self.total_inodes = int(values[1])
        self.block_size = int(values[2])
        self.inode_size = int(values[3])
        self.first_non_reserved_inode = int(values[6])


class Inode:
    def __init__(self, row):
        values = []
        for i in range(1, 27):
            values.append((row[i]))
        self.inode_num = int(values[0])
        self.file_type = values[1]
        self.mode = values[2]
        self.owner = int(values[3])
        self.link_count = int(values[5])
        self.file_size = int(values[9])
        self.num_blocks = int(values[10])
        self.blocks = []
        for i in range(12, 24):
            self.blocks.append(int(values[i-1]))
        self.indirect_list = []
        for i in range(24, 27):
            self.indirect_list.append(int(values[i-1]))


class Dirent:
    def __init__(self, row):
        values = []
        for i in range(1, 7):
            values.append((row[i]))
        self.par_inode_num = int(values[0])
        self.offset = int(values[1])
        self.inode = int(values[2])
        self.ent_len = int(values[3])
        self.name_len = int(values[4])
        self.name = values[5]


class Indirect:
    def __init__(self, row):
        values = []
        for i in range(1, 6):
            values.append((row[i]))
        self.owner_inode_num = int(values[0])
        self.indir_lvl = int(values[1])
        self.block_offset = int(values[2])
        self.block_num = int(values[3])
        self.ref_num = int(values[4])


def main():
    if len(sys.argv) != 2:
        print("Error: Invalid arguments. Correct usage: ./lab3b CSV_FILENAME", file=sys.stderr)
        sys.exit(1)

    supblock = None
    blocks_free = []
    inodes_free = []
    inodes = []
    dirents = []
    indirects = []
    blocks = {}

    try:
        file = open(sys.argv[1], "r")
    except (IOError, OSError):
        print("Error: Could not read file, no such file or directory", file=sys.stderr)
        sys.exit(1)

    super_block = "SUPERBLOCK"
    inode_str = "INODE"
    bfree_str = "BFREE"
    ifree_str = "IFREE"
    indirect_str = "INDIRECT"
    dirent_str = "DIRENT"

    with file as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row[0] == super_block:
                supblock = Superblock(row)
            elif row[0] == inode_str:
                inodes.append(Inode(row))
            elif row[0] ==  bfree_str:
                blocks_free.append(int(row[1]))
            elif row[0] == ifree_str:
                inodes_free.append(int(row[1]))
            elif row[0] == indirect_str:
                indirects.append(Indirect(row))
            elif row[0] == dirent_str:
                dirents.append(Dirent(row))

    for i in inodes:
        offset = 0
        for b in i.blocks:
            if b != 0:
                if b < 0 or b >= supblock.total_blocks:
                    print("INVALID BLOCK {} IN INODE {} AT OFFSET {}".format(b, i.inode_num, offset))
                    exitcode = 2
                elif b in blocks:
                    blocks[b].append(("BLOCK", i.inode_num, offset))
                elif b < 8:
                    print("RESERVED BLOCK {} IN INODE {} AT OFFSET {}".format(b, i.inode_num, offset))
                    exitcode = 2
                else:
                    blocks[b] = [("BLOCK", i.inode_num, offset)]

        offset = len(i.blocks)
        block_type = None
        for j in range(0, 3):
            if j == 2:
                block_type = "TRIPLE INDIRECT BLOCK"
                offset = 65804
            if j == 1:
                block_type = "DOUBLE INDIRECT BLOCK"
                offset = 268
            if j == 0:
                block_type = "INDIRECT BLOCK"
                offset = 12

            block = i.indirect_list[j]
            if block != 0:
                if block < 0 or block >= supblock.total_blocks:
                    print("INVALID {} {} IN INODE {} AT OFFSET {}".format(block_type, block, i.inode_num, offset))
                    exitcode = 2
                elif block in blocks:
                    blocks[block].append((block_type, i.inode_num, offset))
                elif block < 8:
                    print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(block_type, block, i.inode_num, offset))
                    exitcode = 2
                else:
                    blocks[block] = [(block_type, i.inode_num, offset)]

    for indir in indirects:
        block_type = None
        block = indir.ref_num
        if indir.indir_lvl != 1 or indir.indir_lvl != 2 or indir.indir_lvl != 3:
            pass
        if indir.indir_lvl == 3:
            block_type = "DOUBLE INDIRECT BLOCK"
        if indir.indir_lvl == 2:
            block_type = "INDIRECT BLOCK"
        if indir.indir_lvl == 1:
            block_type = "BLOCK"

        if block != 0:
            if block >= supblock.total_blocks or block < 0:
                print("INVALID {} {} IN INODE {} AT OFFSET {}".format(block_type, block, indir.owner_inode_num, indir.block_offset))
                exitcode = 2
            elif block in blocks:
                blocks[block].append((block_type, indir.owner_inode_num, offset))
            elif block < 8:
                print("RESERVED {} {} IN INODE {} AT OFFSET {}".format(block_type, block, indir.owner_inode_num, indir.block_offset))
                exitcode = 2
            else:
                blocks[block] = [(block_type, indir.owner_inode_num, offset)]

    for b in range(8, supblock.total_blocks):
        if b in blocks and b in blocks_free:
            print("ALLOCATED BLOCK {} ON FREELIST".format(b))
            exitcode = 2
        elif b not in blocks and b not in blocks_free:
            print("UNREFERENCED BLOCK {}".format(b))
            exitcode = 2

        if  b in blocks and len(blocks[b]) > 1:
            for block_type, inode, offset in blocks[b]:
                print("DUPLICATE {} {} IN INODE {} AT OFFSET {}".format(block_type, b, inode, offset))
                exitcode = 2

    allocated_inodes = {inode.inode_num for inode in inodes}

    for i in range(supblock.first_non_reserved_inode, supblock.total_inodes):
        if i not in allocated_inodes and i not in inodes_free:
            print("UNALLOCATED INODE {} NOT ON FREELIST".format(i))
            exitcode = 2

    for i in allocated_inodes:
        if i in inodes_free:
            print("ALLOCATED INODE {} ON FREELIST".format(i))
            exitcode = 2

    parent = [0] * (supblock.total_inodes + 1)
    parent[2] = 2
    link_count = [0] * (supblock.total_inodes + 1)


    for dirent in dirents:
        if dirent.inode > supblock.total_inodes or dirent.inode < 1:
            print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(dirent.par_inode_num, dirent.name, dirent.inode))
            exitcode = 2
        elif dirent.inode not in allocated_inodes:
            print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(dirent.par_inode_num, dirent.name, dirent.inode))
            exitcode = 2
        else:
            if dirent.name != "'.'" and dirent.name != "'..'":
                parent[dirent.inode] = dirent.par_inode_num
            link_count[dirent.inode] += 1

    for dir in dirents:
        if dir.name == "'.'" and dir.par_inode_num != dir.inode:
            print("DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(dir.par_inode_num, dir.inode,
                                                                                     dir.par_inode_num))
            exitcode = 2
        if dir.name == "'..'" and parent[dir.par_inode_num] != dir.inode:
            print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(dir.par_inode_num, dir.inode,
                                                                                      dir.par_inode_num))
            exitcode = 2

    for i in inodes:
        if i.link_count != link_count[i.inode_num]:
            print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(i.inode_num, link_count[i.inode_num],
                                                                     i.link_count))
            exitcode = 2




    sys.exit(exitcode)

if __name__ == "__main__":
    main()
