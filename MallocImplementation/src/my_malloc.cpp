#include <assert.h>
#include <my_malloc.h>
#include <stdio.h>
#include <sys/mman.h>
#include <iostream>
using namespace std;

// A pointer to the head of the free list.
node_t *head = NULL;

// The heap function returns the head pointer to the free list. If the heap
// has not been allocated yet (head is NULL) it will use mmap to allocate
// a page of memory from the OS and initialize the first free node.
node_t *heap() {
  if (head == NULL) {
    // This allocates the heap and initializes the head node.
    head = (node_t *)mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE,
                          MAP_ANON | MAP_PRIVATE, -1, 0);
    head->size = HEAP_SIZE - sizeof(node_t);
    head->next = NULL;
  }

  return head;
}

// Reallocates the heap.
void reset_heap() {
  if (head != NULL) {
    munmap(head, HEAP_SIZE);
    head = NULL;
    heap();
  }
}

// Returns a pointer to the head of the free list.
node_t *free_list() { return head; }

// Calculates the amount of free memory available in the heap.
size_t available_memory() {
  size_t n = 0;
  node_t *p = heap();
  while (p != NULL) {
    n += p->size;
    p = p->next;
  }
  return n;
}

// Returns the number of nodes on the free list.
int number_of_free_nodes() {
  int count = 0;
  node_t *p = heap();
  while (p != NULL) {
    count++;
    p = p->next;
  }
  return count;
}

// Prints the free list. Useful for debugging purposes.
void print_free_list() {
  node_t *p = heap();
  while (p != NULL) {
    printf("Free(%zd)", p->size);
    p = p->next;
    if (p != NULL) {
      printf("->");
    }
  }
  printf("\n");
}

// Finds a node on the free list that has enough available memory to
// allocate to a calling program. This function uses the "first-fit"
// algorithm to locate a free node.
//
// PARAMETERS:
// size - the number of bytes requested to allocate
//
// RETURNS:
// found - the node found on the free list with enough memory to allocate
// previous - the previous node to the found node
//
void find_free(size_t size, node_t **found, node_t **previous) {
  heap();
  bool foundfree = false;
  if(head->next == NULL) {
    if(free_list()->size >= size + sizeof(header_t) + sizeof(node_t)) {
      *found = head;
      *previous = NULL;
    } else {
      *found = NULL;
      *previous = NULL;
    }
  } else {
    *previous = head;
    *found = (*previous)->next;
    while((*found)->size < size + sizeof(header_t) + sizeof(node_t) && (*found)->next != NULL) {
      if((*found)->size >= size + sizeof(header_t) + sizeof(node_t)) {
        foundfree = true;
        break;
      }
      *previous = *found;
      *found = (*found)->next;
    }
    if(!foundfree) {
      *found = NULL;
      *previous = NULL;
    }
  }
}

// Splits a found free node to accommodate an allocation request.
//
// The job of this function is to take a given free_node found from
// `find_free` and split it according to the number of bytes to allocate.
// In doing so, it will adjust the size and next pointer of the `free_block`
// as well as the `previous` node to properly adjust the free list.
//
// PARAMETERS:
// size - the number of bytes requested to allocate
// previous - the previous node to the free block
// free_block - the node on the free list to allocate from
//
// RETURNS:
// allocated - an allocated block to be returned to the calling program
//
void split(size_t size, node_t **previous, node_t **free_block,
           header_t **allocated) {
  heap();
  assert(*free_block != NULL);
  size_t actual_size = size + sizeof(header_t);
  node_t *p_free_block = *free_block;
  *free_block = (node_t *)(((char *)*free_block) + actual_size);
  (*free_block)->size = p_free_block->size - actual_size;
  //cout << (*previous)->size << "\n";
  if(*previous == NULL) {
    head = *free_block;
    //cout << (*free_block)->size << "\n";
  } else {
    (*previous)->next = *free_block;
  }
  *allocated = (header_t *)p_free_block;
  (*allocated)->size = size;
  //cout << (*allocated)->size << "\n";
  (*allocated)->magic = MAGIC;
}

// Returns a pointer to a region of memory having at least the request `size`
// bytes.
//
// PARAMETERS:
// size - the number of bytes requested to allocate
//
// RETURNS:
// A void pointer to the region of allocated memory
//
void *my_malloc(size_t size) {
  heap();
  node_t *found;
  node_t *previous;
  header_t *allocated;
  find_free(size, &found, &previous);
  //cout << found->size << "\n";
  if(found == NULL) {
    return NULL;
  }
  split(size, &previous, &found, &allocated);
  //cout << allocated->size << "\n";
  node_t *ret = (node_t *)(((char *)allocated) + sizeof(header_t));
  //cout << ret->size << "\n";
  return ret;
}

// Merges adjacent nodes on the free list to reduce external fragmentation.
//
// This function will only coalesce nodes starting with `free_block`. It will
// not handle coalescing of previous nodes (we don't have previous pointers!).
//
// PARAMETERS:
// free_block - the starting node on the free list to coalesce
//
void coalesce(node_t *free_block) {
  size_t block_size = free_block->size + sizeof(node_t);
  node_t *curr_address = free_block;
  cout << (size_t)((char*)free_block->next - (char*)free_block) << "\n";
  if(((char*)(curr_address) + block_size) == (char*)curr_address->next) {
    cout << "poop\n";
  }
  while(((char *)curr_address) + block_size == (char *) curr_address->next) {
    if(curr_address->next == NULL){
      break;
    }
    cout << curr_address->next << "  " << curr_address->next->next << "\n";
    curr_address->next = curr_address->next->next;
    cout << curr_address->next << "\n";
    curr_address = curr_address->next;
    block_size = curr_address->size;
  }
}

// Frees a given region of memory back to the free list.
//
// PARAMETERS:
// allocated - a pointer to a region of memory previously allocated by my_malloc
//
void my_free(void *allocated) {
  header_t *p = (header_t *)allocated;
  //cout << p->magic << "\n";
  p = (header_t *)(((char *)p) - sizeof(header_t));
  //cout << p->magic << "\n";
  assert(p->magic == MAGIC);
  node_t* free_space = (node_t *)p;
  free_space->size = p->size + sizeof(header_t);
  free_space->next = head;
  head = free_space;
  coalesce(heap());
}
