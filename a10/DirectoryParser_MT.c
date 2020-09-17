/*
 *  Modified by Peilin Guo
 *  Apr 8, 2020
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
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#include "DirectoryParser.h"
#include "MovieTitleIndex.h"
#include "Movie.h"
#include "DocIdMap.h"


#define BUFFER_SIZE 1000

//=======================
// To minimize the number of files we have, I'm
// putting the private function prototypes for
// the DirectoryParser here.

/**
 * Helper function to index a single file. 
 *
 * \return a pointer to the number of records (lines) indexed from the file
 */
void *IndexAFile_MT(void *toBeIter);

pthread_mutex_t ITER_MUTEX = PTHREAD_MUTEX_INITIALIZER;  // global variable
pthread_mutex_t INDEX_MUTEX = PTHREAD_MUTEX_INITIALIZER;  // global variable

MovieTitleIndex movieIndex;

int ParseTheFiles_MT(DocIdMap docs, MovieTitleIndex index, int num_threads) {
    // Create the iterator
    // Create the threads
    // Spawn the threads to work on the function IndexTheFile_MT

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    HTIter iter = CreateHashtableIterator(docs);
    movieIndex = index;
    pthread_t tid[num_threads];
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&tid[i], NULL, IndexAFile_MT, iter);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
    }
    while (HTIteratorHasMore(iter)) {
        HTIteratorNext(iter);
        pthread_t tid[num_threads];
        for (int i = 0; i < num_threads; i++) {
            pthread_create(&tid[i], NULL, IndexAFile_MT, iter);
        }
        for (int i = 0; i < num_threads; i++) {
            pthread_join(tid[i], NULL);
        }
    }
    DestroyHashtableIterator(iter);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Took %f seconds to execute. \n", cpu_time_used);

    return 0;
}

void *IndexAFile_MT(void *docname_iter) {
    int *num_records = (int *) malloc(sizeof(int));
    *num_records = 0;
    pthread_mutex_lock(&ITER_MUTEX);
    char *file;
    uint64_t id;
    // Lock the iterator
    // Get the filename, unlock the iterator
    HTIter iter = (HTIter) docname_iter;
    HTKeyValue cur_row;
    HTIteratorGet(iter, &cur_row);
    file = (char *) cur_row.value;
    id = (uint64_t) cur_row.key;
    pthread_mutex_unlock(&ITER_MUTEX);
    // printf("file after ITER_MUTEX: %s\n", file);

    // Read the file
    // Create movie from row
    // Lock the index
    // Add movie to index
    // Unlock the index

    FILE *cfPtr;
    if ((cfPtr = fopen(file, "r")) == NULL) {
        printf("%s could not be opened\n", file);
        return 0;
    } else {
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, cfPtr) != NULL) {
            Movie *movie = CreateMovieFromRow(buffer);
            pthread_mutex_lock(&INDEX_MUTEX);
            int result = AddMovieTitleToIndex(movieIndex,
                    movie, id, *num_records);
            pthread_mutex_unlock(&INDEX_MUTEX);
            if (result < 0) {
                fprintf(stderr, "Didn't add MovieToIndex.\n");
            }
            *num_records += 1;
            DestroyMovie(movie);  // Done with this now
        }
        fclose(cfPtr);
    }

    free(num_records);
    return 0;
}
