#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// timestamp
int timestamp;
// page frame number (index in the coremap)
int page_frame_number;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	// variables to hold index = the page frame number to return
	int index = page_frame_number;
	// and hold the minimum time
	int time_index = timestamp;
	// loop through
	for(int i=0; i<memsize; i++){
		// find the minimum time and store the time and page frame number
		if(coremap[i].timestamp < time_index){
			time_index = coremap[i].timestamp;
			index = i;
		}
	}
	// return page frame number which is the index in the coremap
	return index;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	// on each access to page update the timestamp
	timestamp++;
	// the page table gets set to latest timestamp
	int frame = p->frame >> PAGE_SHIFT;
	coremap[frame].timestamp = timestamp;
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// initilize timestamp and page frame number to 0
	timestamp = 0;
	page_frame_number = 0;
}
