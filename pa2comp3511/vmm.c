#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Data structure of meta_data 
struct 
  __attribute__((__packed__)) // compiler directive, avoid "gcc" padding bytes to struct
  meta_data {
    size_t size;             // 8 bytes (in 64-bit OS)
    char free;               // 1 byte ('f' or 'o')
    struct meta_data *next;  // 8 bytes (in 64-bit OS)
    struct meta_data *prev;  // 8 bytes (in 64-bit OS)
};

// calculate the meta data size and store as a constant (exactly 25 bytes)
const size_t meta_data_size = sizeof(struct meta_data); 

// Global variables
void *start_heap = NULL; // pointing to the start of the heap, initialize in main()
struct meta_data dummy_head_node; // dummy head node of a doubly linked list, initialize in main()
struct meta_data *head = &dummy_head_node;

// The implementation of the following functions are given:
void list_add(struct meta_data *new, struct meta_data *prev, struct meta_data *next);
void list_add_tail(struct meta_data *new, struct meta_data *head);
void init_list(struct meta_data *list);

// Students are required to implement these functions below
void *mm_malloc(size_t size);
void mm_free(void *p);
void mm_print();


int main() {
    start_heap = sbrk(0);
    init_list(head);
    // Assume there are at most 26 different malloc/free
    // Here is the mapping rule
    // a=>0, b=>1, ..., z=>25
    void *pointers[26] = {NULL};
    // TODO: Complete the main function here
    
/*
	Implementation of main

	Step 1 	: Check if the input has a '\n' and then delete the '\n' and output with a format of "=== <input> ===\n".
	Step 2 	: Create a while loops that stops when the input from the input file is empty or NULL.
	Step 3 	: Compare each of the input word. If the frist six words match with "malloc" then go to first condition for mm_malloc function.
		Step 3.1  : Do a for loop to retrieve the wanted size. Retrieved by concatinating the elements from the 10th element to the end.
		Step 3.2  : Call the mm_malloc function. The pointers indexing method is determined by TA. Done by substracting the 8th elemnt with the 'a' char.
	Step 4 	: If the first four word match with "free", go to the second condition for mm_free function.
		Step 4.1  : Call the mm_free function. The pointers indexing method is determined by TA. Done by substracting the 8th element with 'a' char.	
	Step 5	: If the first word of the input does not match woth any of the first condition, print "ERROR" (Consider the scope for this assignment only for "malloc" and "free")
 */

    char userinput[100];
    while(fgets(userinput,100, stdin) != NULL){
    	if(userinput[strlen(userinput)-1] == '\n'){
    	    userinput[strlen(userinput)-1] = 0;
    	}

    	printf("=== %s ===\n",userinput);
     
    	if (userinput[0] == 'm' && userinput[1] == 'a' && userinput[2] == 'l' && userinput[3] == 'l' && userinput[4] == 'o' && userinput[5] == 'c'){
    	    int num_input = strlen(userinput);
    	    int i;
    	    char sizestr[] = "";
    	    for (i=9; i<num_input; i++){
    		strncat(sizestr, &userinput[i], 1);
    	    }	    
    	    pointers[userinput[7]-'a'] = mm_malloc(atoi(sizestr));
    	}
    	else if(userinput[0] == 'f' && userinput[1] == 'r' && userinput[2] == 'e' && userinput[3] == 'e'){
    	    mm_free(pointers[userinput[5]-'a']);
    	}
    	else{
    	    printf("ERROR: wrong input");
    	}
    	mm_print();			
    }
    return 0;
}

void *mm_malloc(size_t size) {
    // TODO: Complete mm_malloc here   

/*
    Implementation of mm_malloc
    
    Step 1  : For the first malloc, where there is only head with no node, initialize the new node and create a new heap space with the size of the input (name: size) 
        Step 1.1  : Use the sbrk function to increase the allocated memory.
        Step 1.2  : Change the free status to 'o'; means occupied and size equal to the size input (ignoring the size of the metadata).
        Step 1.3  : Use the availale list_add_tail function to add the new node to the back of the linked list.
    Step 2  : Create a while loop to find the empty space with a sufficient block size. The loop stop when the address is the same as head's.
    Step 3  : Create 3 conditions of malloc method.
        Step 3.1  : The first condition is when the available size is bigger or equal than the input size but smaller than the input size + metadata size.
            Step 3.1.1  : Change the free status to 'o'; means occupied.
        Step 3.2  : The second condition is when the available size is bigger than or equal to the size + metadata size.
            Step 3.2.1  : Create a new metadata struct that points to the address of the current + input size + metadata size. This new struct is used as the new free space in between 2 nodes.
            Step 3.2.2  : Set the free status of the new struct as 'f'; means free. Set the new struct size as to the size of current pointed space size - input size - metadata size.
            Step 3.2.3  : Use the available list_add function to add the new struct in between the current pointed node and the node next to the poiinted node.
            Step 3.2.4  : Set the current pointed node's free status to 'o' to represent occupied and size of the input size.
        Step 3.3  : The third condition is when there is no available space with sufficient size. 
            Step 3.3.1  : Set the curr variable to sbrk(0) to get the current address then expand the allocated memory using sbrk function with the size of input size + metada size.
            Step 3.3.2  : Set the new node free status to 'o' to represent occupied and the size of the input size.
            Step 3.3.3  : Use the available list_add_tail function to add the new node to the back of the linked list.
*/
    struct meta_data *curr = sbrk(0);
    //initialize for the first time
    if (!(head->next->size)){
    	sbrk(size + meta_data_size);
    	curr->free = 'o';
    	curr->size = size;
    	list_add_tail(curr, head);	
    	return (curr+1);
    }

    curr = head->next;
    while ((((curr->size) < size) || ((curr->free) == 'o')) && (curr != head)){
	   curr = curr->next;
    }

    //if it is the exact size or bigger than size but smaller than the size + metadata size
    if(((curr->size) >= size) && (curr->size < (size + meta_data_size))){
	   curr->free='o';
	   return (curr+1);
    }

    //if the free space is bigger than size + metadata size
    else if ((curr->size) >= (size + meta_data_size)){
    	struct meta_data * new = (void*)((void*)curr + size + meta_data_size);
    	new->size = curr->size - size - meta_data_size;
    	new->free = 'f';
    	list_add(new, curr, curr->next);
    	curr->size = size;
    	curr->free = 'o';
    	return (curr+1);
    }

    //if there is no available space with sufficient size 
    else {
    	curr = sbrk(0);
    	sbrk(size + meta_data_size);
    	curr->free = 'o';
    	curr->size = size;
    	list_add_tail(curr, head);
    	return curr+1; 
   }
}

void mm_free(void *p) {
    // TODO: Complete mm_free here

/*
    Implementation of mm_free
    
    Step 1  : Create a temporary struct of meta_data that point to the the address of the node that head points to.
    Step 2  : Do a while loop that stops when the temp address is the same as head's.
    Step 3  : While in the loop, check whether the address of the block (temp+1) is equal to the pointer p's address.
    Step 4  : If found, then change the value of free from 'o' to 'f'.
*/

    struct meta_data *temp = head->next;
    while (temp != head) {
    	if ((void*)(temp+1) == p) {
    	    temp->free = 'f';
    	}
    	temp = temp->next;
    }
}

void init_list(struct meta_data *list) {
    list->next = list;
    list->prev = list;
}

void list_add(struct meta_data *new,
            struct meta_data *prev,
            struct meta_data *next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void list_add_tail(struct meta_data *new,
                    struct meta_data *head) {
    list_add(new, head->prev, head);
}


void mm_print() {
    struct meta_data *cur = head->next;
    int i = 1;
    while ( cur != head ) {
        printf("Block %02d: [%s] size = %d bytes\n", 
             i,  // block number - counting from bottom
            (cur->free=='f') ? "FREE" : "OCCP", // free or occupied
            cur->size ); // size, in term of bytes
        i = i+1;
        cur = cur->next;
    } 
}
