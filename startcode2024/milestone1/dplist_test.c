#define _GNU_SOURCE

#include "dplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name, "%s", ((my_element_t*)element)->name); // asprintf requires _GNU_SOURCE
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free(((my_element_t*)*element)->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return (((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1;
}

void ck_assert_msg(bool result, char * msg) {
    if (!result) printf("%s\n", msg);
}

void yourtest1() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    // Test empty list operations
    ck_assert_msg(dpl_size(list) == 0, "Failure: Expected size of empty list to be 0.");
    ck_assert_msg(dpl_get_element_at_index(list, 0) == NULL, "Failure: Expected NULL for element at index 0 in empty list.");
    
    // Test insertion at head
    my_element_t elem1 = {1, "Element1"};
    list = dpl_insert_at_index(list, &elem1, -100, true);
    ck_assert_msg(dpl_size(list) == 1, "Failure: Expected list size to be 1 after head insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 0))->id == 1, "Failure: Expected element ID to be 1 at index 0.");

    // Test insertion at tail
    my_element_t elem2 = {2, "Element2"};
    list = dpl_insert_at_index(list, &elem2, 1, true);
    ck_assert_msg(dpl_size(list) == 2, "Failure: Expected list size to be 2 after tail insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 1))->id == 2, "Failure: Expected element ID to be 2 at index 1.");

    // Test insertion in the middle
    my_element_t elem3 = {3, "Element3"};
    list = dpl_insert_at_index(list, &elem3, 1, true);
    ck_assert_msg(dpl_size(list) == 3, "Failure: Expected list size to be 3 after middle insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 1))->id == 3, "Failure: Expected element ID to be 3 at index 1 after middle insertion.");

    // Test out-of-bound insertion
    my_element_t elem4 = {4, "Element4"};
    list = dpl_insert_at_index(list, &elem4, 10, true);
    ck_assert_msg(dpl_size(list) == 4, "Failure: Expected list size to be 4 after out-of-bound insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 3))->id == 4, "Failure: Expected element ID to be 4 at index 3 after out-of-bound insertion.");

    // Test removal at head
    list = dpl_remove_at_index(list, -100, true);
    ck_assert_msg(dpl_size(list) == 3, "Failure: Expected list size to be 3 after removing head.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 0))->id == 3, "Failure: Expected element ID to be 3 at new head.");

    // Test removal at tail
    list = dpl_remove_at_index(list, 2, true);
    ck_assert_msg(dpl_size(list) == 2, "Failure: Expected list size to be 2 after removing tail.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 1))->id == 2, "Failure: Expected element ID to be 2 at new tail.");

    // Test out-of-bound removal
    list = dpl_remove_at_index(list, 10, true);
    ck_assert_msg(dpl_size(list) == 1, "Failure: Expected list size to be 1 after out-of-bound removal.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 0))->id == 3, "Failure: Expected element ID to be 3 at index 0 after out-of-bound removal.");
    
    // Test retrieval by reference
    dplist_node_t *ref = dpl_get_reference_at_index(list, 0);
    ck_assert_msg(((my_element_t*)dpl_get_element_at_reference(list, ref))->id == 3, "Failure: Expected element ID to be 3 at reference index 0.");
    //test empty list access
    list = dpl_remove_at_index(list, -10, true);
    list = dpl_remove_at_index(list, 0, true);
    list = dpl_remove_at_index(list, 10, true);
    //test empty list indexing
    ref = dpl_get_reference_at_index(list, 100);
    ck_assert_msg(ref == NULL, "Failure: Expected 100 to be NULL after freeing.");
    ref = dpl_get_reference_at_index(list, -100);
    ck_assert_msg(ref == NULL, "Failure: Expected -100 to be NULL after freeing.");
    ref = dpl_get_reference_at_index(list, 0);
    ck_assert_msg(ref == NULL, "Failure: Expected -100 to be NULL after freeing.");
    // Test freeing list with elements
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: Expected list to be NULL after freeing.");

    printf("All tests passed!\n");
}

int main(void) {
    yourtest1();
    return 0;
}
