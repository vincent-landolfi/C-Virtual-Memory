#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

//extern int memsize;

extern int debug;

extern struct frame *coremap;

// temp array to store each line from tracefile
char temp[MAXLINE];

// number of lines in the tracefile
static int tracefile_lines;

// store tracefile addresses
static addr_t *tracefile_adresses;

// store the current address' location in tracefile_addresses
static int current_address_location;

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	// store the largest next access time
	int largest_next_access = 0;
	// frame of page to evict
	int page_to_evict = 0;
	// counter for loop
	int counter = 0;
	// flag to tell us if we found a frame that is never used again
	int found_never_used_again = 0;
	// go through the coremap
	while (counter < memsize && !found_never_used_again) {
		// if largest next access is less than this frame next access time
		if (largest_next_access < coremap[counter].next_access_time || coremap[counter].next_access_time == -1) {
			// new largext next_acess
			largest_next_access = coremap[counter].next_access_time;
			// the new page to evict is at frame i
			page_to_evict = counter;
			// if the next access time is -1
			if (coremap[counter].next_access_time == -1) {
				// we found one that will be never used again
				found_never_used_again = 1;
			}
		}
		// increment counter
		counter++;
	}
	// return the frame of page to evict
	return page_to_evict;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	// counter int to increment during the while loop (to go through tracefile_addresses)
	// starts at one ahead of the current adress location, since we're currently at current
	// address location, we're looking ahead
	unsigned long counter = current_address_location+1;
	// bool to tell us if we found next access time
	int found_next_access = 0;
	// while we haven't gone through the entire tracefile_adresses and we haven't found
	// the next access time
	while(counter < tracefile_lines && !found_next_access) {
		// since the code goes in order of the traces, we can look at the current trace
		// to see when the current page will be used again
		if (tracefile_adresses[current_address_location] == tracefile_adresses[counter]) {
			// set the next access time of that frame which is just the difference between i and
			// the current location, since thats how far ahead it is used next
			coremap[p->frame >> PAGE_SHIFT].next_access_time = counter-current_address_location;
			// we found the next access time
			found_next_access = 1;
		}
		// increment counter
		counter++;
	}
	// if we exited the loop without finding the next access, it's never used again
	if (!found_next_access) {
		// set next access time to -1
		coremap[p->frame >> PAGE_SHIFT].next_access_time = -1;
	}
	// move to the next address in the tracefile
	current_address_location++;
	// dont need to return anything
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	/* Like it says in the sim.h file, for this function we will need
	to look into the trace file to see which page gets used in the longest
	amount of time.
	*/
	// instantiate current_address_location
	current_address_location = 0;
	// setting variable for when we open the tracefile
	FILE *tracefile_pointer;
	FILE *tracefile_pointer2;
	// instatiate number of lines in tracefile to 0
	tracefile_lines = 0;
	// counter for storing the frame numbers in the trace_frames var
	int counter = 0;
	// temp address to holder variable, gets scanned from each line of tracefile
	addr_t curr_line_address;
	// type char to pass into sscanf
	char type;
	// quick check to see if tracefile is null, and if not make sure it opens properly
	if (tracefile == NULL || (tracefile_pointer = fopen(tracefile,"r")) == NULL) {
		// show error
		perror("Could not open tracefile");
		// exit
		exit(1);
	}
	if (tracefile == NULL || (tracefile_pointer2 = fopen(tracefile,"r")) == NULL) {
		perror("broken");
		exit(1);
	}
	// go through all the lines in the tracefile
	while(fgets(temp,MAXLINE,tracefile_pointer) != NULL) {
		// increment the line counter
		tracefile_lines++;
	}
	// now that we have the number of lines in the tracefile, we can properly
	// allocate space to store each address in the tracefile_adresses var by
	// applying lines * address size
	tracefile_adresses = (addr_t *)malloc(tracefile_lines * sizeof(addr_t));
	// go through all the lines in the tracefile again
	while(fgets(temp,MAXLINE,tracefile_pointer2) != NULL) {
		// get the address from the current line
		sscanf(temp, "%c %lx", &type, &curr_line_address);
		// shift the bits to get the frame number, store address in tracefile_adresses
		tracefile_adresses[counter] = curr_line_address >> PAGE_SHIFT;
		// increment counter
		counter++;
	}
	// close the files
	fclose(tracefile_pointer);
	fclose(tracefile_pointer2);
}
