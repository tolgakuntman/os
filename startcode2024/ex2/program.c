#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dplist.h"

void test_dplist() {
    printf("Testing doubly linked list functions with char* elements...\n");

    // 1. Create a new list (Case 1: Empty list)
    dplist_t *list = dpl_create();
    assert(list != NULL);
    assert(dpl_size(list) == 0);
    printf("List creation (empty list) passed!\n");

    // Insert at the start of the list (Case 2)
    list = dpl_insert_at_index(list, strdup("Start"), 0); // Should add "Start" to the empty list
    assert(dpl_size(list) == 1);
    assert(strcmp(dpl_get_element_at_index(list, 0), "Start") == 0);
    printf("Insert at start of list passed!\n");

    // Insert at the end of the list (Case 3)
    list = dpl_insert_at_index(list, strdup("End"), dpl_size(list)+5); // Insert "End" at end
    assert(dpl_size(list) == 2);
    assert(strcmp(dpl_get_element_at_index(list, 1), "End") == 0);
    printf("Insert at end of list passed!\n");

    // Insert in the middle of the list (Case 4)
    list = dpl_insert_at_index(list, strdup("Middle"), 1); // Insert "Middle" at index 1
    assert(dpl_size(list) == 3);
    assert(strcmp(dpl_get_element_at_index(list, 1), "Middle") == 0);
    printf("Insert in middle of list passed!\n");

    // Check overall list order after inserts
    assert(strcmp(dpl_get_element_at_index(list, 0), "Start") == 0);
    assert(strcmp(dpl_get_element_at_index(list, 1), "Middle") == 0);
    assert(strcmp(dpl_get_element_at_index(list, 2), "End") == 0);
    printf("List order after inserts passed!\n");

    // Remove from start of the list (Case 2)
    list = dpl_remove_at_index(list, 0); // Remove "Start"
    assert(dpl_size(list) == 2);
    assert(strcmp(dpl_get_element_at_index(list, 0), "Middle") == 0);
    printf("Remove from start of list passed!\n");

    // Remove from end of the list (Case 3)
    list = dpl_remove_at_index(list, dpl_size(list) +35); // Remove "End"
    assert(dpl_size(list) == 1);
    assert(strcmp(dpl_get_element_at_index(list, 0), "Middle") == 0);
    printf("Remove from end of list passed!\n");

    // Remove from middle of the list (Case 4)
    list = dpl_insert_at_index(list, strdup("End"), 1); // Reinsert "End" at end
    list = dpl_insert_at_index(list, strdup("New Start"), 0); // Insert "New Start" at start
    assert(dpl_size(list) == 3);
    list = dpl_remove_at_index(list, 1); // Remove "Middle"
    assert(dpl_size(list) == 2);
    assert(strcmp(dpl_get_element_at_index(list, 0), "New Start") == 0);
    assert(strcmp(dpl_get_element_at_index(list, 1), "End") == 0);
    printf("Remove from middle of list passed!\n");

    // Test removing until the list is empty (Case 1: Empty list after removals)
    list = dpl_remove_at_index(list, 1); // Remove "End"
    list = dpl_remove_at_index(list, 0); // Remove "New Start"
    assert(dpl_size(list) == 0);
    printf("Remove until list is empty passed!\n");

    // Test freeing the list (already empty case)
    dpl_free(&list);
    //assert(&list == NULL);
    printf("List free passed!\n");

    printf("All test cases for insertion and removal in different states passed!\n");
}

int main() {
    test_dplist();
    return 0;
}
