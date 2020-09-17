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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "FileParser.h"
#include "Movie.h"
#include "MovieIndex.h"

#define NUM_FIELDS 6
#define MAX_ROW_LENGTH 1000
#define MAX_NUM_ACTORS 10

const char *DELIM_ROW = "|";
const char *DELIM_ACTORS = ",";


char *CheckAndAllocateString(char *token) {
    if (strcmp("-", token) == 0) {
        return NULL;
    } else {
        char *out = (char *) malloc((strlen(token) + 1) * sizeof(char));
        snprintf(out, strlen(token) + 1, "%s", token);
        return out;
    }
}

int CheckInt(char *token) {
    if (strcmp("-", token) == 0) {
        return -1;
    } else {
        return atoi(token);
    }
}

double CheckDouble(char *token) {
    if (strcmp("-", token) == 0) {
        return -1;
    } else {
        return atof(token);
    }
}

/**
 * star_rating|title|content_rating|genre|duration|actors_list
 * 7.4|Back to the Future Part III|PG|Adventure|118|Michael J. Fox,Christopher Lloyd,Mary Steenburgen
 */
Movie *CreateMovieFromRow(char *data_row) {
    Movie *mov = CreateMovie();
    if (mov == NULL) {
        printf("Couldn't create a Movie.\n");
        return NULL;
    }

    // STEP 2(Student): Parse the row to create and populate a Movie.
    // Use the strtok
    /*
    char row[MAX_ROW_LENGTH];
    for (int i=0; data_row[i]!='\0';i++) {
        row[i] = data_row[i];
    }*/
    // Split out star rating
    char *star_rating = strtok(data_row, DELIM_ROW);
    mov->star_rating = CheckDouble(star_rating);
    // Split out title
    char *title = strtok(NULL, DELIM_ROW);
    mov->title = CheckAndAllocateString(title);
    // Split out content rating
    char *content_rating = strtok(NULL, DELIM_ROW);
    mov->content_rating = CheckAndAllocateString(content_rating);
    // Split out genre
    char *genre = strtok(NULL, DELIM_ROW);
    mov->genre = CheckAndAllocateString(genre);
    // Split out duration
    char *duration = strtok(NULL, DELIM_ROW);
    mov->duration = CheckInt(duration);
    // Split out actors list
    char *actors = strtok(NULL, DELIM_ROW);
    char **actors_list = (char **) malloc(MAX_NUM_ACTORS * sizeof(char *));
    int num_actors = 0;
    char *actor = strtok(actors, DELIM_ACTORS);
    while (actor != NULL) {
        *(actors_list + num_actors) = CheckAndAllocateString(actor);
        num_actors += 1;
        actor = strtok(NULL, DELIM_ACTORS);
    }
    mov->num_actors = num_actors;
    mov->actor_list = actors_list;

    return mov;
}

// Returns a LinkedList of Movie structs from the specified file
LinkedList ReadFile(char *filename) {
    FILE *cfPtr;

    LinkedList movie_list = CreateLinkedList();

    if ((cfPtr = fopen(filename, "r")) == NULL) {
        printf("File could not be opened\n");
        DestroyLinkedList(movie_list, NULL);
        return NULL;
    } else {
        char *row = NULL;
        ssize_t read;
        size_t len = 0;

        while ((read = getline(&row, &len, cfPtr)) != -1) {
            // Got the line; create a movie from it
            // Delete '\n' from the row

            char newRow[1000];
            for (int i = 0; *(row + i) != '\n'; i++) {
                newRow[i] = *(row + i);
            }
            MoviePtr movie = CreateMovieFromRow(newRow);
            if (movie != NULL) {
                InsertLinkedList(movie_list, movie);
            }
        }
        free(row);
        fclose(cfPtr);
    }
    return movie_list;
}
