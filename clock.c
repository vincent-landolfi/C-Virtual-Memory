#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// clock hand
int clock;
// refrenced frames set to 0 and 1
int* ref_frames;


/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int clock_evict() {
	// clocks hand movement starts at 0
	int clock_movement = clock;
	// loop till memsize
	while(clock_movement < memsize) {
		// check if the frame has been referenced if it has unreference set to 0
		if (ref_frames[clock_movement] == 1){
			ref_frames[clock_movement] = 0;
		} else { 
			// if the frame has not been referenced set to 1
			ref_frames[clock_movement] = 1;
			// set the clock to the moved position of the clock hand
			clock = clock_movement;
			return clock_movement;
		}
		// if frame not referenced continue loop increment the clocks hand movement
		clock_movement++;
		clock_movement = clock_movement % memsize;
	}
	return -1;
}


/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	// on each call to access a page set the frame that is called to referenced = 1
	int frame = p->frame >> PAGE_SHIFT;
	ref_frames[frame] = 1;
	return;
}


/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	// clocks hand starts at 0th position
	clock = 0;
	ref_frames = malloc(sizeof(int)*memsize);
	// set all referenced frames to 0 as none have been referenced yet
	for(int i=0; i<memsize; i++){
		ref_frames[i] = 0;
	}
}
