/*
 *  Adrienne Slaughter
 *  5007 Spr 2020
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
#include <htll/Hashtable_priv.h>

#include "MovieIndex.h"
#include "htll/LinkedList.h"
#include "htll/Hashtable.h"
#include "Movie.h"
#include "MovieSet.h"

int CheckDuplicate(Movie *pMovie, MovieSet pSet);

void DestroyMovieSetWrapper(void *movie_set) {
    DestroyMovieSet((MovieSet) movie_set);
}

void toLower(char *str, int len) {
    for (int i = 0; i < len; i++) {
        str[i] = tolower(str[i]);
    }
}

Index BuildMovieIndex(LinkedList movies, enum IndexField field_to_index) {
    Index movie_index = CreateIndex();

    // STEP 4(Student): Check that there is at least one movie
    // What happens if there is not at least one movie?
    if (NumElementsInLinkedList(movies) == 0) {
        printf("no movie!");
        return NULL;
    }

    LLIter iter = CreateLLIter(movies);
    Movie *cur_movie;
    LLIterGetPayload(iter, (void **) &cur_movie);

    int result = AddMovieToIndex(movie_index, cur_movie, field_to_index);

    while (LLIterHasNext(iter)) {
        LLIterNext(iter);
        LLIterGetPayload(iter, (void **) &cur_movie);
        result = AddMovieToIndex(movie_index, cur_movie, field_to_index);
    }
    DestroyLLIter(iter);
    return movie_index;
}

char *AllocateString(char *token) {
    if (strcmp("-", token) == 0) {
        return NULL;
    } else {
        char *out = (char *) malloc((strlen(token) + 1) * sizeof(char));
        snprintf(out, strlen(token) + 1, "%s", token);
        return out;
    }
}

int AddMovieActorsToIndex(Index index, Movie *movie) {
    HTKeyValue kvp;
    HTKeyValue old_kvp;
    // STEP 6(Student): Add movies to the index via actors.
    //  Similar to STEP 5.
    // add movie to every actor key
    for (int which_actor = 0; which_actor < movie->num_actors; which_actor++) {
        kvp.key = ComputeKey(movie, Actor, which_actor);
        int insert_bucket = HashKeyToBucketNum(index, kvp.key);
        LinkedList insert_list = index->buckets[insert_bucket];
        if (LookupInHashtable(index, kvp.key, &kvp)==-1) {
            kvp.value = CreateMovieSet(movie->actor_list[which_actor]);
        } else {
            // kvp.value = GetMovieSet(index, movie->actor_list[which_actor]);
            // Check whether the movie has already in the movieSet
            MovieSet movieSet = kvp.value;
            if (CheckDuplicate(movie, movieSet) == -1) {
                return -1;
            }
        }
        if (which_actor == 0) {
            AddMovieToSet((MovieSet) kvp.value, movie);
        } else {
            Movie *copy = CreateMovie();
            copy->num_actors = movie->num_actors;
            copy->duration = movie->duration;
            copy->star_rating = movie->star_rating;
            copy->title = AllocateString(movie->title);
            copy->genre = AllocateString(movie->genre);
            copy->content_rating = AllocateString(movie->content_rating);
            char **actors_list = (char **) malloc(
                    copy->num_actors * sizeof(char *));
            for (int i = 0; i < copy->num_actors; i++) {
                *(actors_list + i) = AllocateString(movie->actor_list[i]);
            }
            copy->actor_list = actors_list;
            AddMovieToSet((MovieSet) kvp.value, copy);
        }
        PutInHashtable(index, kvp, &old_kvp);
    }
    return 0;
}

int AddMovieToIndex(Index index, Movie *movie, enum IndexField field) {
    if (field == Actor) {
        return AddMovieActorsToIndex(index, movie);
    }
    HTKeyValue kvp;
    HTKeyValue old_kvp;
    char rating_str[10];
    kvp.key = ComputeKey(movie, field, 0);
    int insert_bucket = HashKeyToBucketNum(index, kvp.key);
    LinkedList insert_list = index->buckets[insert_bucket];
    if (LookupInHashtable(index, kvp.key, &kvp)==-1) {
        // the relevant MovieSet doesn't exist
        switch (field) {
            case Genre:
                kvp.value = CreateMovieSet(movie->genre);
                break;
            case StarRating:
                snprintf(rating_str, 10, "%f", movie->star_rating);
                kvp.value = CreateMovieSet(rating_str);
                break;
            case ContentRating:
                kvp.value = CreateMovieSet(movie->content_rating);
                break;
        }
    } else {
        // MovieSet exists
        // Get the MovieSet
//        switch (field) {
//            case Genre:
//                kvp.value = GetMovieSet(index, movie->genre);
//                break;
//            case StarRating:
//                snprintf(rating_str, 10, "%f", movie->star_rating);
//                kvp.value = GetMovieSet(index, rating_str);
//                break;
//            case ContentRating:
//                kvp.value = GetMovieSet(index, movie->content_rating);
//                break;
        // }
        MovieSet movieSet = kvp.value;
        if (CheckDuplicate(movie, movieSet) == -1) {
            return -1;
        }
    }
    // STEP 5(Student): How do we add movies to the index?
    // The general idea:
    // Check hashtable to see if relevant MovieSet already exists
    // If it does, grab access to it from the hashtable
    // If it doesn't, create the new MovieSet and get the pointer to it
    // Put the new MovieSet into the Hashtable.

    // After we either created or retrieved the MovieSet from the Hashtable:
    AddMovieToSet((MovieSet) kvp.value, movie);
    return PutInHashtable(index, kvp, &old_kvp);
}

// Check whether the movie has already in the movieSet
// return 0 if it is not in the movieSet
// return -1 if it has already existed in the movieSet
int CheckDuplicate(Movie *movie, MovieSet movieSet) {
    LLIter iter = CreateLLIter(movieSet->movies);
    Movie *cur_movie;
    LLIterGetPayload(iter, (void **) &cur_movie);
    if (cur_movie == movie) {
        DestroyLLIter(iter);
        return -1;
    }
    while (LLIterHasNext(iter)) {
        LLIterNext(iter);
        LLIterGetPayload(iter, (void **) &cur_movie);
        if (cur_movie == movie) {
            DestroyLLIter(iter);
            return -1;
        }
    }
    DestroyLLIter(iter);
    return 0;
}

uint64_t ComputeKey(Movie *movie, enum IndexField which_field,
        int which_actor) {
    char rating_str[10];
    switch (which_field) {
        case Genre:
            return FNVHash64((unsigned char *) movie->genre,
                    strlen(movie->genre));
        case StarRating:
            snprintf(rating_str, 10, "%f", movie->star_rating);
            return FNVHash64((unsigned char *) rating_str,
                    strlen(rating_str));
        case ContentRating:
            return FNVHash64((unsigned char *) movie->content_rating,
                             strlen(movie->content_rating));
        case Actor:
            if (which_actor < movie->num_actors) {
                return
                FNVHash64((unsigned char *) movie->actor_list[which_actor],
                                 strlen(movie->actor_list[which_actor]));
            }
            break;
    }
    return -1u;
}

MovieSet GetMovieSet(Index index, const char *term) {
    HTKeyValue kvp;
    // char lower[strlen(term) + 1];
    // snprintf(lower, strlen(term) + 1, "%s", term);
    // toLower(lower, strlen(lower));
    int result = LookupInHashtable(index,
            FNVHash64((unsigned char *) term,
                    (unsigned int) strlen(term)), &kvp);
    if (result < 0) {
        printf("term couldn't be found: %s \n", term);
        return NULL;
    }
    return (MovieSet) kvp.value;
}

int DestroyIndex(Index index) {
    DestroyHashtable(index, &DestroyMovieSetWrapper);
    index = NULL;
    return 0;
}

Index CreateIndex() {
    return CreateHashtable(128);
}
