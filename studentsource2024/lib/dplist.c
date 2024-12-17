/**
 * \author Tolga Kuntman
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    if (list == NULL || *list == NULL) return;

    dplist_node_t *curr = (*list)->head;
    dplist_node_t *next;

    while (curr != NULL) {
        next = curr->next;
        if (free_element) {
            (*list)->element_free(&(curr->element));
        }
        free(curr);
        curr = next;
    }
    free(*list);
    *list = NULL;

}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    if (insert_copy) {
        list_node->element = list->element_copy(element);
        if (list_node->element == NULL) {
            free(list_node);
            return NULL;
        }
    } else {
        list_node->element = element;
    }

    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
        }
    }
    //list->size++;
    return list;

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if (list == NULL || list->head == NULL) return list;  // If list is NULL or empty, do nothing

    dplist_node_t *ref_at_index;

    // Case 2: Remove the first node (index <= 0)
    if (index <= 0) {
        ref_at_index = list->head;
        list->head = ref_at_index->next;
        if (list->head != NULL) {
            list->head->prev = NULL;
        }
    } else {
        // Get the node reference at the specified index
        ref_at_index = dpl_get_reference_at_index(list, index);
        if (ref_at_index == NULL) return list;  // Index out of range
        if (ref_at_index == list->head && ref_at_index->next == NULL) {
            // Only one element in the list and we are removing it
            list->head = NULL;
        } else if (ref_at_index->next != NULL) { // Node is in the middle
            ref_at_index->prev->next = ref_at_index->next;
            ref_at_index->next->prev = ref_at_index->prev;
        } else { // Node is the last node
            if (ref_at_index->prev != NULL) {
                ref_at_index->prev->next = NULL;
            }
        }
    }

    // Free the element if required
    if (free_element && list->element_free != NULL) {
        list->element_free(&(ref_at_index->element));
    }

    // Free the node itself
    free(ref_at_index);
    //list->size--;

    return list;
}


int dpl_size(dplist_t *list) {
    // return list->size;
    if(list==NULL) return -1;
    if(list->head==NULL) return 0;
    int count=0;
    dplist_node_t *dummy = list->head;
    while (dummy != NULL) {
        count++;
        dummy = dummy->next;
    }
    return count;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    int count=0;
    if(list==NULL||list->head==NULL) return NULL;
    dplist_node_t *dummy = list->head;
    if(index<0) return list->head->element;
    while(dummy->next!=NULL&&count<index){
        count++;
        dummy=dummy->next;
    }
    return (dummy != NULL) ? dummy->element : NULL;

}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    
    if(list==NULL||list->head==NULL){
      return -1;
    }
    dplist_node_t *current= list->head;
    int index=0;
    while(current!=NULL){
      if (list->element_compare(element, current->element) == 0) {
            return index;
      }
      index++;
      current=current->next;
    }
    return -1;

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count=0;
    if(list==NULL||list->head==NULL) return NULL;
    dplist_node_t *dummy = list->head;
    while(dummy->next!=NULL&&count<index){
        count++;
        dummy=dummy->next;
    }
    return dummy;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL || list->head == NULL || reference == NULL) {
        return NULL;
    }

    dplist_node_t *dummy = list->head;
    while (dummy != NULL) {
        if (dummy == reference) {
            return dummy->element;
        }
        dummy = dummy->next;
    }

    return NULL;
}



