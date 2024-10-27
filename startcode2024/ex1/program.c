#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include <assert.h>
#include "dplist.h"

void test_dplist() {
    printf("Testing doubly linked list functions...\n");

    // 1. Create a new list
    dplist_t *list = dpl_create();
    assert(list != NULL);
    assert(dpl_size(list) == 0);
    printf("List creation passed!\n");

    // 2. Insert elements (characters) at various positions
    list = dpl_insert_at_index(list, 'A', 0); // Insert 'A' at index 0 (empty list)
    assert(dpl_size(list) == 1);
    assert(dpl_get_element_at_index(list, 0) == 'A');
    printf("Insert 'A' at index 0 passed!\n");

    list = dpl_insert_at_index(list, 'B', 1); // Insert 'B' at index 1 (end of list)
    assert(dpl_size(list) == 2);
    assert(dpl_get_element_at_index(list, 1) == 'B');
    printf("Insert 'B' at index 1 passed!\n");

    list = dpl_insert_at_index(list, 'C', 0);  // Insert 'C' at index 0 (beginning of list)
    assert(dpl_size(list) == 3);
    assert(dpl_get_element_at_index(list, 0) == 'C');
    printf("Insert 'C' at index 0 (non-empty list) passed!\n");

    list = dpl_insert_at_index(list, 'D', 2); // Insert 'D' at index 2 (middle of list)
    assert(dpl_size(list) == 4);
    assert(dpl_get_element_at_index(list, 2) == 'D');
    printf("Insert 'D' at index 2 passed!\n");

    // 3. Test list size
    assert(dpl_size(list) == 4);
    printf("List size passed!\n");

    // 4. Test getting elements by index
    assert(dpl_get_element_at_index(list, 0) == 'C');
    assert(dpl_get_element_at_index(list, 1) == 'A');
    assert(dpl_get_element_at_index(list, 2) == 'D');
    assert(dpl_get_element_at_index(list, 3) == 'B');
    printf("Get element at index passed!\n");

    // 5. Test removing elements (you'll need to implement the `dpl_remove_at_index` function)
    // Uncomment the tests below once you implement this function
    
    list = dpl_remove_at_index(list, 1);  // Remove element at index 1 ('A')
    assert(dpl_size(list) == 3);
    assert(dpl_get_element_at_index(list, 1) == 'D');
    printf("Remove at index 1 passed!\n");

    list = dpl_remove_at_index(list, 0);  // Remove element at index 0 ('C')
    assert(dpl_size(list) == 2);
    assert(dpl_get_element_at_index(list, 0) == 'D');
    printf("Remove at index 0 passed!\n");

    list = dpl_remove_at_index(list, 1);  // Remove element at index 1 ('B')
    assert(dpl_size(list) == 1);
    assert(dpl_get_element_at_index(list, 0) == 'D');
    printf("Remove at index 1 passed!\n");
    

    // 6. Test freeing the list
    dpl_free(&list);
    //assert(list == NULL);
    printf("List free passed!\n");

    printf("All tests passed!\n");
}

int main() {
    test_dplist();
    return 0;
}


