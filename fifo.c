#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int oldest_page = 0;

// node structure for linked list
struct node{
	unsigned int frame;
	struct node *next;
	struct node *last;
};
// head of linked list
struct node *head;
// store size of linked list too
int linked_list_size;
/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {
	// get the head of the list's frame
	int page_to_evict = head->frame;
	// pass the last node to the new head
	(head->next)->last = head->last;
	// set next to the new head
	head = head->next;
	// reduce size
	linked_list_size--;
	// return the page to evict
	return page_to_evict;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
	// if there are no elements in the list
	if (linked_list_size == 0) {
		// this PTE is the first to be added
		// frame is pages frame
		head->frame = p->frame >> PAGE_SHIFT;
		// next is still null
		// this is now the last node
		head->last = head;
		// size increases to 1
		linked_list_size++;
	} else {
		// if there is room or the frame hasnt been added to the list yet
		if (linked_list_size < memsize || coremap[p->frame >> PAGE_SHIFT].in_list == 0) {
			// if there is no room
			if (linked_list_size >= memsize) {
				// evict another page, make room
				fifo_evict();
			}
			// make a new node
			struct node *new_node = (struct node *)malloc(sizeof(struct node));
			// its frame is the PTE's frame
			new_node->frame = p->frame >> PAGE_SHIFT;
			// this is the new last node, so set last node's next to this node
			(head->last)->next = new_node;
			// again this is the last node
			head->last = new_node;
			// next last is null, we'll only store last in head
			new_node->next = NULL;
			new_node->last = NULL;
			// set the value in the page's frame so that we know its in the list now
			coremap[p->frame >> PAGE_SHIFT].in_list = 1;
			// size increases
			linked_list_size++;
		}
	}
	return;
}

/* Initialize any data structures needed for this
 * replacement algorithm
 */
void fifo_init() {
	// size is 0
	linked_list_size = 0;
	// allocate space for the list
	head = (struct node *)malloc(sizeof(struct node));
	// no elements for next or last
	head->next = NULL;
	head->last = NULL;
}
