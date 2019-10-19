import sys, string, locale, csv

class superblock {
	def __init__(self, csv):
		self.total_blocks = csv[1]
		self.total_inodes = csv[2]
		self.block_size = csv[3]
		self.inode_size = csv[4]
		self.first_nonreserved_inode = csv[7]
}
'''
class block {
	def __init__(self, csv):
		self.inode_num = csv_entry[1]
		self.offset =
		self.level =
}
'''
class inode {
	def __init__(self, csv):
		self.inode_num = csv[1]
		self.type = csv[2]
		self.mode = csv[3]
		self.owner = csv[4]
		self.link_count = csv[6]
		self.file_size = int(csv[10])
		self.num_blocks = int(csv[11])
		self.blocks = []
		for i in range(12,24):
			self.blocks.append(int(csv[i]))
		self.single_ind = csv[24]
		self.double_ind = csv[25]
		self.triple_ind = csv[26]
}

class dirent {
	def __init__(self, csv):
		self.parent_inode = csv[1]
		self.offset = csv[2]
		self.inode_of_refd = csv[3]
		self.entry_len = csv[4]
		self.name_len = csv[5]
		self.name = csv[6]
}

class indirect {
	def __init__(self, csv):
		self.owner_inode_num = csv[1]
		self.level = csv[2]
		self.offset_of_refd = csv[3]
		self.scanned_block_num = csv[4]
		self.refd_block_num = csv[5]
}

if __name__ == '__main__':
	#for exit
	code = 0


	#Reference: dict['School'] = "DPS School" => Add new entry

	#stuff to hold the entries from csv
	sup_block = None
	b_free = [] #scan bfree entries
	blocks = {}#scan inode entries
	i_free = [] #scan ifree entries
	inodes = [] #scan dirent and inode entries => dirent is actual link count, inode entries have link_count variable, check if different
	dirents = []
	indirects = []


	if sys.argc != 2:
		sys.stderr.write("*** Error: incorrect number of arguments!\n")

	try:
		file = open(sys.argv[1], "r")
	except:
		sys.stderr.write("*** Error: unable to open file!\n")

	with file as csvfile:
		reader = csv.reader(csvfile, delimiter=',', quotechar="\'")
		for line in reader:
			if line[0] == "SUPERBLOCK":
				sup_block = superblock(line)
			elif line[0] == "BFREE":
				b_free.append(line[1])
			elif line[0] == "IFREE":
				i_free.append(line[1])
			elif line[0] == "DIRENT":
				dirents.append(dirent(line))
			elif line[0] == "INDIRECT":
				indirects.append(indirect(line))
			elif line == "INODE":
				inodes.append(inode(line))

	#block consistensy audits
	for i in inodes:
		offset = 0
		for b in i.blocks[0:12]:
			if b < 0 or b >= sup_block.total_blocks:
				print("INVALID BLOCK {} IN INODE {} AT OFFSET {}\n".format(b, i.inode_num, offset))
				code = 2
			elif b in b_free:
				print("ALLOCATED BLOCK {} ON FREELIST\n".format(b))
				code = 2
			elif b < 8: #the block numbers below 8 are the reserved ones
				print("RESERVED BLOCK {} IN INODE {} AT OFFSET {}\n".format(b, i.inode_num, offset))
				code = 2
			elif b in blocks:
				blocks[b].append((i.inode_num, offset, 0))
			else:
				blocks[b] = [(i.inode_num, offset, 0)] #tuple format: (inode number, offset, level of indirection) => we want this to be a list so we can check the number of references
			offset++

		offset = 12
		ind = i.single_ind
		if ind != 0:
			if ind < 0 or ind >= sup_block.total_blocks:
				print("INVALID INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(ind, i.inode_num, offset))
				code = 2
			elif ind in b_free:
				print("ALLOCATED BLOCK {} ON FREELIST\n".format(ind))
				code = 2
			elif ind < 8:
				print("RESERVED INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(ind, i.inode_num, offset))
				code = 2
			elif ind in blocks:
				blocks[ind].append((i.inode_num, offset, 1))
			else:
				blocks[ind] = [(i.inode_num, offset, 1)]

		offset = 268
		ind = i.double_ind
		if ind != 0:
			if ind < 0 or ind >= sup_block.total_blocks:
				print("INVALID DOUBLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(ind, i.inode_num, offset))
				code = 2
			elif ind in b_free:
				print("ALLOCATED BLOCK {} ON FREELIST\n".format(ind))
				code = 2
			elif ind < 8:
				print("RESERVED DOUBLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(ind, i.inode_num, offset))
				code = 2
			elif ind in blocks:
				blocks[ind].append((i.inode_num, offset, 2))
			else:
				blocks[ind] = [(i.inode_num, offset, 2)]

		offset = 65804
		ind = i.triple_ind
		if ind != 0:
			if ind < 0 or ind >= sup_block.total_blocks:
				print("INVALID TRIPLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(ind, i.inode_num, offset))
				code = 2
			elif ind in b_free:
				print("ALLOCATED BLOCK {} ON FREELIST\n".format(ind))
				code = 2
			elif ind < 8:
				print("RESERVED TRIPLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(ind, i.inode_num, offset))
				code = 2
			elif ind in blocks:
				blocks[ind].append((i.inode_num, offset, 3))
			else:
				blocks[ind] = [(i.inode_num, offset, 3)]

	for i in indirects:
		r_block = i.refd_block_num
		levelmessage = ""
		if i.level == 1:
			levelmessage = "SINGLE"
		elif i.level == 2:
			levelmessage = "DOUBLE"
		elif i.level == 3:
			levelmessage = "TRIPLE"

		if r_block < 0 or r_block >= sup_block.total_blocks:
			print("INVALID {} INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(levelmessage, r_block, i.inode_num, i.offset_of_refd))
			code = 2
		elif r_block in b_free:
			print("ALLOCATED BLOCK {} ON FREELIST\n".format(ind))
			code = 2
		elif r_block < 8:
			print("RESERVED {} INDIRECT BLOCK {} IN INODE {} AT OFFSET {}\n".format(levelmessage, r_block, i.inode_num, i.offset_of_refd))
			code = 2


	for b in range(8,sup_block.total_blocks):
		if i not in b_free and b not in blocks:
			print("UNREFERENCED BLOCK {}\n" b)
			code = 2
		if len(blocks[b]) > 1:
			for ref in blocks[b]:   #because we could have more than one duplicate reference and we need to print each one separately
				temp_offset = ref[1]
				temp_inode_num = ref[0]
				levelmessage = ""
				if ref[2] == 1:
					levelmessage = "SINGLE INDIRECT"
				elif ref[2] == 2:
					levelmessage = "DOUBLE INDIRECT"
				elif ref[2] == 3:
					levelmessage = "TRIPLE INDIRECT"
				print("DUPLICATE {} BLOCK {} IN INODE {} AT OFFSET {}\n".format(levelmessage, b, temp_inode_num, temp_offset))
			code = 2


	#inode allocation audits
	for i in range(0:sup_block.total_inodes)
		if i in i_free and i in inodes:
			print("ALLOCATED INODE {} ON FREELIST\n".format(num))
			code = 2
		elif i not in inodes and i not in i_free:
			print("UNALLOCATED INODE {} NOT ON FREELIST\n".format(num))
			code = 2


	#go through all dirents, make a dict with all the real links, key value is inode number
	#go through inode list, save inode number of each one and look up the value of the actual
	#link count in your new dictironary and compare against inode.link_count
	'''
	reallinks = {}
	for entry in dirents:
		inum = entry.inode_of_refd;
		if inum in reallinks:
			curval = reallinks[inum]
			reallinks[inum] = curval + 1
		else:
			reallinks[inum] = 1
	for i in inodes:
		inum = i.inode_num;
		if inum not in reallinks:
			#idk what this means
		elif reallinks[inum] != i.link_count
			print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(inum, reallinks[inum], i.link_count))
			code = 2
'''


	file.close()

	exit(status)