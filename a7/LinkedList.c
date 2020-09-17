// author: Peilin Guo
// date: Mar 8, 2020

// CS 5007, Northeastern University, Seattle
// Summer 2019
// Adrienne Slaughter
//
// Inspired by UW CSE 333; used with permission.
//
// This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
// It is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.

#include "LinkedList.h"
#include "LinkedList_priv.h"
#include "Assert007.h"

#include <stdio.h>
#include <stdlib.h>

// Creates a LinkedList.
// The customer is responsible for calling DestroyLinkedList()
// to destroy and free the LinkedList when done.
//
// Returns a LinkedList; NULL if there's an error.
LinkedList CreateLinkedList() {
    LinkedList list = (LinkedList) malloc(sizeof(LinkedListHead));
    if (list == NULL) {
        // out of memory
        return (LinkedList) NULL;
    }
    // Step 1.
    // initialize the newly allocated record structure
    list->head = NULL;
    list->tail = NULL;
    list->num_elements = 0;

    return list;
}

// Destroys a LinkedList.
// All structs associated with a LinkedList will be
// released and freed. Payload_free_function will
// be used to free the payloads in the list.
//
// INPUT: A pointer to a LinkedList.
// INPUT: A pointer to a function used to free the payload.
//
// Returns 0 if the destroy was successful; non-zero for failure.
int DestroyLinkedList(LinkedList list,
                      LLPayloadFreeFnPtr payload_free_function) {
    Assert007(list != NULL);
    Assert007(payload_free_function != NULL);

    // Step 2.
    // Free the payloads, as well as the nodes
    while (list->num_elements != 0) {
        LinkedListNode *temp = list->head;
        list->head = list->head->next;
        list->num_elements -= 1U;
        payload_free_function(temp->payload);
        DestroyLinkedListNode(temp);
    }
    free(list);

    return 0;
}

unsigned int NumElementsInLinkedList(LinkedList list) {
    Assert007(list != NULL);
    return list->num_elements;
}

LinkedListNodePtr CreateLinkedListNode(void *data) {
    LinkedListNodePtr node = (LinkedListNodePtr) malloc(sizeof(LinkedListNode));
    if (node == NULL) {
        // Out of memory
        return NULL;
    }
    node->payload = data;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

int DestroyLinkedListNode(LinkedListNode *node) {
    Assert007(node != NULL);
    node->payload = NULL;
    node->next = NULL;
    node->prev = NULL;
    free(node);
    return 0;
}

// Adds an element to the head of a LinkedList, with the payload
// as the given pointer.
// The customer is responsible for allocating the payload.
//
// INPUT: A pointer to the LinkedList that will be added to,
// INPUT: A pointer to a payload for a node in the linkedList.
//
// Returns 0 if the insertion was successful; non-zero for failure.
int InsertLinkedList(LinkedList list, void *data) {
    Assert007(list != NULL);
    Assert007(data != NULL);
    LinkedListNodePtr new_node = CreateLinkedListNode(data);
    if (new_node == NULL) {
        return 1;
    }
    if (list->num_elements == 0) {
        Assert007(list->head == NULL);  // debugging aid
        Assert007(list->tail == NULL);  // debugging aid
        list->head = new_node;
        list->tail = new_node;
        new_node->next = new_node->prev = NULL;
        list->num_elements = 1U;
        return 0;
    }
    // Step 3.
    // typical case; list has >=1 elements
    new_node->next = list->head;
    list->head->prev = new_node;
    list->head = new_node;
    list->num_elements += 1U;

    return 0;
}

// Adds an element to the tail of a LinkedList, with the payload
// as the given pointer.
// The customer is responsible for allocating the payload.
//
// INPUT: A pointer to the LinkedList that will be added to,
// INPUT: A pointer to a payload for a node in the linkedList.
//
// Returns 0 if the insertion was successful; non-zero for failure.
int AppendLinkedList(LinkedList list, void *data) {
    Assert007(list != NULL);

    // Step 5: implement AppendLinkedList.  It's kind of like
    // InsertLinkedList, but add to the end instead of the beginning.
    LinkedListNodePtr new_node = CreateLinkedListNode(data);
    if (new_node == NULL) {
        return 1;
    }

    // for the empty list
    if (list->num_elements == 0) {
        list->head = new_node;
        list->tail = new_node;
        new_node->next = new_node->prev = NULL;
        list->num_elements = 1U;
        return 0;
    }

    // for the non-empty list
    new_node->prev = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
    list->num_elements += 1U;

    return 0;
}

// Copies the pointer to the payload at the head of a list
// to the provided dataPtr, and removes the head node from the list.
//
// INPUT: A pointer to the LinkedList.
// INPUT: A pointer to a pointer that will be updated
//        with the payload at the head of the list. .
//
// Returns 0 if the pop was successful; non-zero for failure.
int PopLinkedList(LinkedList list, void **data) {
    Assert007(list != NULL);
    Assert007(data != NULL);

    // Step 4: implement PopLinkedList.  Make sure you test for
    // and empty list and fail.  If the list is non-empty, there
    // are two cases to consider: (a) a list with a single element in it
    // and (b) the general case of a list with >=2 elements in it.
    // Be sure to call free() to deallocate the memory that was
    // previously allocated by InsertLinkedList().
    // for the empty list
    if (list->num_elements == 0) {
        return 1;
    }
    *data = list->head->payload;  // dereference
    LinkedListNode *nodeToRemove = list->head;
    // for the list has one element
    if (list->num_elements == 1U) {
        list->head = list->tail = NULL;
    } else {  // for the list with >=2 elements
        list->head = list->head->next;
        list->head->prev = NULL;
    }
    list->num_elements -= 1U;
    free(nodeToRemove);
    return 0;
}

// Copies the pointer to the payload at the tail of a list
// to the provided dataPtr, and removes the tail node from the list.
//
// INPUT: A pointer to the LinkedList.
// INPUT: A pointer to a pointer that will be updated
//        with the payload at the tail of the list.
//
// Returns 0 if the slice was successful; non-zero for failure.
int SliceLinkedList(LinkedList list, void **data) {
    Assert007(list != NULL);
    Assert007(data != NULL);

    // Step 6: implement SliceLinkedList.
    // for the empty list
    if (list->num_elements == 0) {
        return 1;
    }
    *data = list->tail->payload;  // dereference
    LinkedListNode *nodeToRemove = list->tail;
    // for the list has one element
    if (list->num_elements == 1U) {
        list->head = list->tail = NULL;
    } else {  // for the list with >=2 elements
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }
    list->num_elements -= 1U;
    free(nodeToRemove);

    return 0;
}

void SortLinkedList(LinkedList list,
                    unsigned int ascending,
                    LLPayloadComparatorFnPtr compare) {
    Assert007(list != NULL);
    if (list->num_elements < 2) {
        return;
    }

    int swapped;
    do {
        LinkedListNodePtr curnode = list->head;
        swapped = 0;

        while (curnode->next != NULL) {
            // compare this node with the next; swap if needed
            int compare_result = compare(curnode->payload,
                    curnode->next->payload);

            if (ascending) {
                compare_result *= -1;
            }
            if (compare_result < 0) {
                // swap
                void *tmp;
                tmp = curnode->payload;
                curnode->payload = curnode->next->payload;
                curnode->next->payload = tmp;
                swapped = 1;
            }
            curnode = curnode->next;
        }
    } while (swapped);
}

void PrintLinkedList(LinkedList list) {
    printf("List has %lu elements. \n", list->num_elements);
}


LLIter CreateLLIter(LinkedList list) {
    Assert007(list != NULL);
    Assert007(list->num_elements > 0);

    LLIter iter = (LLIter) malloc(sizeof(struct ll_iter));
    Assert007(iter != NULL);

    iter->list = list;
    iter->cur_node = list->head;
    return iter;
}

int LLIterHasNext(LLIter iter) {
    Assert007(iter != NULL);
    return (iter->cur_node->next != NULL);
}

// Steps the iterator to the next element in the LinkedList.
//
// INPUT: A pointer to an existing iterators.
//
// Returns 0 if it was successful.
int LLIterNext(LLIter iter) {
    Assert007(iter != NULL);

    // Step 7: if there is another node beyond the iterator, advance to it,
    // and return 0. If there isn't another node, return 1.
    if (LLIterHasNext(iter)) {
        iter->cur_node = iter->cur_node->next;
        return 0;
    }
    return 1;
}

int LLIterGetPayload(LLIter iter, void **data) {
    Assert007(iter != NULL);
    *data = iter->cur_node->payload;
    return 0;
}


int LLIterHasPrev(LLIter iter) {
    Assert007(iter != NULL);
    return (iter->cur_node->prev != NULL);
}

// Steps the iterator to the previous element in the LinkedList
// if there is one.
//
// INPUT: A pointer to an existing iterators.
//
// Returns 0 if successful; 1 otherwise.
int LLIterPrev(LLIter iter) {
    Assert007(iter != NULL);
    // Step 8:  if there is another node beyond the iterator, go to it,
    // and return 0. If not return 1.
    if (LLIterHasPrev(iter)) {
        iter->cur_node = iter->cur_node->prev;
        return 0;
    }
    return 1;
}

int DestroyLLIter(LLIter iter) {
    Assert007(iter != NULL);
    iter->cur_node = NULL;
    iter->list = NULL;
    free(iter);
    return 0;
}

int LLIterInsertBefore(LLIter iter, void *payload) {
    Assert007(iter != NULL);
    if ((iter->list->num_elements <= 1) ||
        (iter->cur_node == iter->list->head)) {
        // insert item
        return InsertLinkedList(iter->list, payload);
    }

    LinkedListNodePtr new_node = CreateLinkedListNode(payload);
    if (new_node == NULL) return 1;

    new_node->next = iter->cur_node;
    new_node->prev = iter->cur_node->prev;
    iter->cur_node->prev->next = new_node;
    iter->cur_node->prev = new_node;

    iter->list->num_elements++;

    return 0;
}

int LLIterDelete(LLIter iter, LLPayloadFreeFnPtr payload_free_function) {
    Assert007(iter != NULL);
    void **data = &(iter->cur_node->payload);
    LinkedListNode *nodeToDelete = iter->cur_node;
    // Step 9: implement LLIterDelete.
    // Be sure to call the payload_free_function to free the payload
    // the iterator is pointing to, and also free any LinkedList
    // data structure element as appropriate.
    // - Case 1: the list becomes empty after deleting.
    if (iter->list->num_elements == 1U) {
        iter->list->tail = iter->list->head = NULL;
        iter->list->num_elements = 0;
        iter->cur_node = NULL;
        payload_free_function(nodeToDelete->payload);
        free(nodeToDelete);
        return 1;
    }
    // - Case 2: iter points at head
    if (nodeToDelete == iter->list->head) {
        PopLinkedList(iter->list, data);
        iter->cur_node = iter->list->head;

    } else if (nodeToDelete == iter->list->tail) {
        // - Case 3: iter points at tail
        SliceLinkedList(iter->list, data);
        iter->cur_node = iter->list->tail;
    } else {
        // iter points in the middle of a list
        LLIterNext(iter);
        nodeToDelete->prev->next = nodeToDelete->next;
        nodeToDelete->next->prev = nodeToDelete->prev;
        iter->list->num_elements -= 1U;
        free(nodeToDelete);
    }
    payload_free_function(nodeToDelete->payload);
    return 0;
}
