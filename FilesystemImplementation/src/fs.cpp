#include "fs.h"
#include <iostream>

using namespace std;

myFileSystem::myFileSystem(char diskName[16]) {
  // open the file with the above name
  // this file will act as the "disk" for your file system
  disk.open(diskName);
}

int myFileSystem::create_file(char name[8], int size) {
  // create a file with this name and this size
  // Step 1: Check to see if we have sufficient free space on disk by
  //   reading in the free block list. To do this:
  // Move the file pointer to the start of the disk file.
  // Read the first 128 bytes (the free/in-use block information)
  // Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size

  // Step 2: we look for a free inode on disk
  // Read in an inode
  // Check the "used" field to see if it is free
  // If not, repeat the above two steps until you find a free inode
  // Set the "used" field to 1
  // Copy the filename to the "name" field
  // Copy the file size (in units of blocks) to the "size" field

  // Step 3: Allocate data blocks to the file
  // for(i=0;i<size;i++)
  // Scan the block list that you read in Step 1 for a free block
  // Once you find a free block, mark it as in-use (Set it to 1)
  // Set the blockPointer[i] field in the inode to this block number.
  // end for

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the disk file
  // Write out the 128 byte free block list to the disk file
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode to the disk file
  char *buf = new char[128];
  int count = 0;
  disk.seekg(0);
  disk.read(buf, 128);
  for(int i = 0; i < 128; i++) {
    if(count == size) {
      break;
    } else if(buf[i] == 0) {
      count += 1;
    }
  }
  if(count < size) {
    return -1;
  }
  idxNode node;
  int num = -1;
  for(int i = 0; i < 16; i++) {
    disk.read((char*)&node, 48);
    if(node.used == 0) {
      num = i;
      break;
    }
  }
  if(num == -1) {
    return -1;
  }
  node.used = 1;
  strcpy(node.name, name);
  node.size = size;
  count = 0;
  for(int i = 0; i < 128; i++) {
    if(count == size) {
      break;
    }
    if(buf[i] == 0) {
      buf[i] = 1;
      node.blockPointers[count] = i;
      count += 1;
    }
  }
  disk.seekg(0);
  disk.write(buf, 128);
  disk.seekg(128 + (num * 48));
  disk.write((char *)&node, 48);
  return 1;
}  // End Create

int myFileSystem::delete_file(char name[8]) {
  // Delete the file with this name
  // Step 1: Locate the inode for this file
  // Move the file pointer to the 1st inode (129th byte)
  // Read in an inode
  // If the inode is free, repeat above step.
  // If the inode is in use, check if the "name" field in the
  //   inode matches the file we want to delete. If not, read the next
  //   inode and repeat

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields as follows:
  // for(i=0;i< inode.size; i++)
  // freeblockList[ inode.blockPointer[i] ] = 0;

  // Step 3: mark inode as free
  // Set the "used" field to 0.

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
  char *free_list = new char[128];
  idxNode node;
  disk.seekg(128);
  bool found = false;
  int count = 0;
  for(int i = 0; i < 16; i++) {
    disk.read((char*)&node, 48);
    if(node.used == 0) {
      continue;
    } else if(node.used == 1){
      if(strcmp(node.name, name) == 0) {
        found = true;
        break;
      }
    }
    count += 1;
  }
  if(found == false){
    return -1;
  }
  disk.seekg(0);
  disk.read(free_list, 128);
  for(int i = 0; i < node.size; i++) {
    free_list[node.blockPointers[i]] = 0;
  }
  node.used = 0;
  disk.seekg(0);
  disk.write(free_list, 128);
  disk.seekg(128 + (48 * count));
  disk.write((char *)&node, 48);
  return 1;
}  // End Delete

int myFileSystem::ls() {
  // List names of all files on disk
  // Step 1: read in each inode and print
  // Move file pointer to the position of the 1st inode (129th byte)
  // for(i=0;i<16;i++)
  // Read in an inode
  // If the inode is in-use
  // print the "name" and "size" fields from the inode
  // end for
  idxNode node;
  disk.seekg(128);
  for(int i = 0; i < 16; i++) {
    disk.read((char*)&node, 48);
    if(node.used == 1) {
      cout << node.name << " " << node.size << "\n";
    }
  }
  return 1;
}  // End ls

int myFileSystem::read(char name[8], int blockNum, char buf[1024]) {
  // read this block from this file
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in an inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., to byte #
  //   addr*1024 in the file)

  // Read in the block => Read in 1024 bytes from this location
  //   into the buffer "buf"
  idxNode node;
  int addr;
  bool found = false;
  disk.seekg(128);
  for(int i = 0; i < 16; i++) {
    disk.read((char*)&node, 48);
    if(strcmp(node.name, name) == 0) {
      found = true;
      break;
    }
  }
  if(found == false) {
    return -1;
  }
  if(blockNum >= node.size) {
    return -1;
  }
  addr = node.blockPointers[blockNum];
  disk.seekg((addr * 1024));
  disk.read(buf, 1024);
  return 1;
}  // End read

int myFileSystem::write(char name[8], int blockNum, char buf[1024]) {
  // write this block to this file
  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., byte # addr*1024)

  // Write the block! => Write 1024 bytes from the buffer "buff" to
  //   this location
  idxNode node;
  int addr;
  bool found = false;
  disk.seekg(128);
  for(int i = 0; i < 16; i++) {
    disk.read((char*)&node, 48);
    if(strcmp(node.name, name) == 0) {
      found = true;
      break;
    }
  }
  if(found == false) {
    return -1;
  }
  if(blockNum >= node.size) {
    return -1;
  }
  addr = node.blockPointers[blockNum];
  disk.seekg((addr * 1024));
  disk.write(buf, 1024);
  return 1;
}  // end write

int myFileSystem::close_disk() {
  disk.close();
  return 1;
}
