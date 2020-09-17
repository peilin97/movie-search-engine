/*
 *  Created by Adrienne Slaughter
 *  CS 5007 Summer 2019
 *  Northeastern University, Seattle
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  See <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DocSet.h"
#include "Hashtable.h"
#include "Util.h"

int AddDocInfoToSet(DocumentSet set, uint64_t docId, int rowId) {
    // STEP 4: Implement AddDocInfoToSet.
    // Make sure there are no duplicate rows or docIds.
    HTKeyValue kvp;
    HTKeyValue old_kvp;
    kvp.key = docId;
    if (LookupInHashtable(set->doc_index, kvp.key, &kvp) == -1) {
        kvp.value = CreateLinkedList();
    } else {
        LinkedList rowList = kvp.value;
        // Check whether rowId is in the rowList
        LLIter iter = CreateLLIter(rowList);
        int cur_row;
        LLIterGetPayload(iter, (void **) &cur_row);
        if (cur_row == rowId) {
            DestroyLLIter(iter);
            return -1;
        }
        while (LLIterHasNext(iter)) {
            LLIterNext(iter);
            LLIterGetPayload(iter, (void **) &cur_row);
            if (cur_row == rowId) {
                DestroyLLIter(iter);
                return -1;
            }
        }
        DestroyLLIter(iter);
    }
    AppendLinkedList(kvp.value, rowId);
    PutInHashtable(set->doc_index, kvp, &old_kvp);
    return 0;
}

int DocumentSetContainsDoc(DocumentSet set, uint64_t docId) {
    // STEP 5: Implement DocumentSetContainsDoc
    HTKeyValue kvp;
    kvp.key = docId;
    return LookupInHashtable(set->doc_index, kvp.key, &kvp);
}

void PrintOffsetList(LinkedList list) {
    printf("Printing offset list\n");
    LLIter iter = CreateLLIter(list);
    int *payload;
    while (LLIterHasNext(iter) != 0) {
        LLIterGetPayload(iter, (void **) &payload);
        printf("%d\t", *((int *) payload));
        LLIterNext(iter);
    }
}


DocumentSet CreateDocumentSet(char *desc) {
    DocumentSet set = (DocumentSet) malloc(sizeof(struct docSet));
    if (set == NULL) {
        // Out of memory
        printf("Couldn't malloc for movieSet %s\n", desc);
        return NULL;
    }
    int len = strlen(desc);
    set->desc = (char *) malloc((len + 1) * sizeof(char));
    if (set->desc == NULL) {
        printf("Couldn't malloc for movieSet->desc");
        return NULL;
    }
    snprintf(set->desc, len + 1, "%s", desc);
    set->doc_index = CreateHashtable(16);
    return set;
}

void DestroyNothing(void *thing) {
    // Helper method to destroy the LinkedList.
}

void DestroyOffsetList(void *val) {
    LinkedList list = (LinkedList) val;
    DestroyLinkedList(list, &DestroyNothing);
}

void DestroyDocumentSet(DocumentSet set) {
    // Free desc
    free(set->desc);
    // Free doc_index
    DestroyHashtable(set->doc_index, &DestroyOffsetList);
    // Free set
    free(set);
}
