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
    ck_assert_msg(dpl_get_index_of_element(list, NULL) == -1, "Failure: Expected -1 for index of element in NULL list.");

    // Test insertion at head
    my_element_t elem1 = {1, "Element1"};
    list = dpl_insert_at_index(list, &elem1, -100, true);
    ck_assert_msg(dpl_size(list) == 1, "Failure: Expected list size to be 1 after head insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 0))->id == 1, "Failure: Expected element ID to be 1 at index 0.");
    ck_assert_msg(dpl_get_index_of_element(list, &elem1) == 0, "Failure: Expected index 0 for first inserted element.");

    // Test insertion at tail
    my_element_t elem2 = {2, "Element2"};
    list = dpl_insert_at_index(list, &elem2, 1, true);
    ck_assert_msg(dpl_size(list) == 2, "Failure: Expected list size to be 2 after tail insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 10))->id == 2, "Failure: Expected element ID to be 2 at index 1.");
    ck_assert_msg(dpl_get_index_of_element(list, &elem2) == 1, "Failure: Expected index 1 for second inserted element.");

    // Test insertion in the middle
    my_element_t elem3 = {3, "Element3"};
    list = dpl_insert_at_index(list, &elem3, 1, true);
    ck_assert_msg(dpl_size(list) == 3, "Failure: Expected list size to be 3 after middle insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 1))->id == 3, "Failure: Expected element ID to be 3 at index 1 after middle insertion.");
    ck_assert_msg(dpl_get_index_of_element(list, &elem3) == 1, "Failure: Expected index 1 for middle inserted element.");

    // Test out-of-bound insertion
    my_element_t elem4 = {4, "Element4"};
    list = dpl_insert_at_index(list, &elem4, 10, true);
    ck_assert_msg(dpl_size(list) == 4, "Failure: Expected list size to be 4 after out-of-bound insertion.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, 10))->id == 4, "Failure: Expected element ID to be 4 at index 3 after out-of-bound insertion.");
    ck_assert_msg(dpl_get_index_of_element(list, &elem4) == 3, "Failure: Expected index 3 for out-of-bound inserted element.");

    // Test removal at head and check index of removed element
    list = dpl_remove_at_index(list, 0, true);
    ck_assert_msg(dpl_size(list) == 3, "Failure: Expected list size to be 3 after removing head.");
    ck_assert_msg(dpl_get_index_of_element(list, &elem1) == -1, "Failure: Expected -1 for removed element.");
    ck_assert_msg(((my_element_t*)dpl_get_element_at_index(list, -100))->id == 3, "Failure: Expected element ID to be 3 at new head.");

    // Test index retrieval for non-existing element
    my_element_t elem5 = {5, "Nonexistent"};
    ck_assert_msg(dpl_get_index_of_element(list, &elem5) == -1, "Failure: Expected -1 for non-existing element.");

    // Test retrieval by reference
    dplist_node_t *ref = dpl_get_reference_at_index(list, 0);
    ck_assert_msg(((my_element_t*)dpl_get_element_at_reference(list, ref))->id == 3, "Failure: Expected element ID to be 3 at reference index 0.");

    // Test free list and ensure dpl_get_index_of_element works with NULL list
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: Expected list to be NULL after freeing.");
    ck_assert_msg(dpl_get_index_of_element(list, &elem3) == -1, "Failure: Expected -1 for element in NULL list after freeing.");

    printf("All tests passed!\n");
}
void second_test() {
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);

    // 1. Test with NULL list
    dplist_node_t *dummy_ref = NULL;
    ck_assert_msg(dpl_get_element_at_reference(NULL, dummy_ref) == NULL, "Failure: Expected NULL for NULL list.");

    // 2. Test with an empty list
    ck_assert_msg(dpl_size(list) == 0, "Failure: Expected empty list.");
    ck_assert_msg(dpl_get_element_at_reference(list, dummy_ref) == NULL, "Failure: Expected NULL for empty list.");

    // Insert elements into the list for further tests
    my_element_t elem1 = {1, "First"};
    list = dpl_insert_at_index(list, &elem1, 0, true);
    ck_assert_msg(dpl_size(list) == 1, "Failure: Expected list size 1 after inserting elem1.");

    my_element_t elem2 = {2, "Second"};
    list = dpl_insert_at_index(list, &elem2, 1, true);
    ck_assert_msg(dpl_size(list) == 2, "Failure: Expected list size 2 after inserting elem2.");

    // 3. Test with NULL reference in a non-empty list
    ck_assert_msg(dpl_get_element_at_reference(list, NULL) == NULL, "Failure: Expected NULL for NULL reference.");

    // 4. Test with a valid reference
    dplist_node_t *head_ref = dpl_get_reference_at_index(list, 10);
    ck_assert_msg(((my_element_t*)dpl_get_element_at_reference(list, head_ref))->id == elem2.id, "Failure: Head reference does not match.");

    // 5. Test with a reference that does not belong to the list
    dplist_t *other_list = dpl_create(element_copy, element_free, element_compare);
    other_list = dpl_insert_at_index(other_list, &elem1, 0, true);
    dplist_node_t *foreign_ref = dpl_get_reference_at_index(other_list, 0);
    ck_assert_msg(dpl_get_element_at_reference(list, foreign_ref) == NULL, "Failure: Expected NULL for reference not in the list.");

    // Clean up
    dpl_free(&other_list, true);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: Expected list to be NULL after freeing.");
    ck_assert_msg(other_list == NULL, "Failure: Expected other_list to be NULL after freeing.");

    printf("All tests in second_test passed!\n");
}


int main(void) {
    yourtest1();
    second_test();
    return 0;
}
