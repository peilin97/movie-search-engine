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
#include <ctype.h>

#include "MovieTitleIndex.h"
#include "LinkedList.h"
#include "Hashtable.h"
#include "Movie.h"
#include "DocSet.h"
#include "Assert007.h"
#include "Util.h"

void DestroyMovieSetWrapper(void *movie_set) {
    DestroyDocumentSet((DocumentSet) movie_set);
}

MovieTitleIndex CreateMovieTitleIndex() {
    MovieTitleIndex ind = (MovieTitleIndex) malloc(sizeof(struct title_index));
    ind->ht = CreateHashtable(128);
    return ind;
}

int DestroyMovieTitleIndex(MovieTitleIndex index) {
    DestroyHashtable(index->ht, DestroyMovieSetWrapper);
    free(index);
    return 0;
}

// Assumes Index is a hashtable with key=title word,
//  and value=hashtable with key doc id and value linked list of rows
int AddMovieTitleToIndex(MovieTitleIndex index,
                         Movie *movie,
                         uint64_t doc_id,
                         int row_id) {
    HTKeyValue kvp;

    // THINK: How to choose this number?
    // What are the pros and cons of choosing the wrong number?
    // Basically, the max number of words possible in a title.
    int numFields = 1000;

    char *token[numFields];
    char *rest = strdup(movie->title);
    char *free_rest = rest;
    int i = 0;
    token[i] = strtok_r(rest, " ", &rest);
    while (token[i] != NULL) {
        toLower(token[i], strlen(token[i]));
        CleanString(token[i]);
        i++;
        token[i] = strtok_r(rest, " ", &rest);
    }

    for (int j = 0; j < i; j++) {
        if (strlen(token[j]) == 0) continue;
        // If this key is already in the hashtable, get the DocSet.
        // Otherwise, create a DocSet and put it in.
        int result = LookupInHashtable(index->ht,
                                       FNVHash64((unsigned char *) token[j],
                                                 (unsigned int) strlen(token[j])), &kvp);
        HTKeyValue old_kvp;

        if (result < 0) {
            kvp.value = CreateDocumentSet(token[j]);
            kvp.key = FNVHash64((unsigned char *) token[j], strlen(token[j]));
            PutInHashtable(index->ht, kvp, &old_kvp);
        }

        AddDocInfoToSet((DocumentSet) kvp.value, doc_id, row_id);
    }
    free(free_rest);
    return 0;
}

DocumentSet GetDocumentSet(MovieTitleIndex index, const char *term) {
    HTKeyValue kvp;
    int low_len = strlen(term);
    char lower[low_len + 1];
    snprintf(lower, low_len + 1, "%s", term);
    toLower(lower, low_len + 1);
    int result = LookupInHashtable(index->ht,
                                   FNVHash64((unsigned char *) lower,
                                             (unsigned int) low_len),
                                   &kvp);
    if (result < 0) {
        return NULL;
    }
    return (DocumentSet) kvp.value;
}
