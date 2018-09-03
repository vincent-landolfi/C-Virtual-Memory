#include <stdio.h>
#include <stdlib.h>

struct DLLNode {
  int num;
  struct DLLNode *next;
  struct DLLNode *prev;
};

int main()
{
	struct DLLNode* head = (struct DLLNode*)malloc(sizeof(struct DLLNode));
	struct DLLNode* curr = head;
	// build up doubly linked list
	int i = 0;
	while (i < 149) {
		struct DLLNode* nextDLL = (struct DLLNode*)malloc(sizeof(struct DLLNode));
		curr->num = i;
		curr->next = nextDLL;
		nextDLL->prev = curr;
		curr = nextDLL;
		i++;
	}
	// destroy doubly linked list
	curr = head;
	i = 0;
	while (i < 150) {
		head = curr->next;
		curr->prev = NULL;
		curr->next = NULL;
		free(curr);
		curr = head;
	}
	return 0;
}
