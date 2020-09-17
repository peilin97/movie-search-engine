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
#include <assert.h>
#include <string.h>

#include <ctype.h>
#include <unistd.h>


#include "htll/LinkedList.h"
#include "MovieIndex.h"
#include "Movie.h"
#include "MovieReport.h"
#include "./FileParser.h"

void DestroyNothing(void *thing) {
    // Helper method to destroy the LinkedList.
}

void DestroyMovieWrapper1(void *a_movie) {
    DestroyMovie((Movie *) a_movie);
}

int main(int argc, char *argv[]) {
    // STEP 8(Student): Check args, do the right thing.
    enum IndexField field;
    char *filename = NULL;
    int opt = getopt(argc, argv, "s:c:g:a:");
    if (opt == -1) {
        printf("Empty argument!\n");
        printf("Provide a flag and a movie");
        printf(" file to complete the movie index.\n");
        printf("Acceptable flags: -s, -c, -g, -a.\n");
        return 1;
    }
    switch (opt) {
        case 's':
            field = StarRating;
            filename = optarg;
            break;
        case 'c':
            field = ContentRating;
            filename = optarg;
            break;
        case 'g':
            field = Genre;
            filename = optarg;
            break;
        case 'a':
            field = Actor;
            filename = optarg;
            break;
        case '?':
            if (optopt == 's' || optopt == 'c' ||
            optopt == 'g' || optopt == 'a') {
                fprintf(stderr,
                        "Option -%c requires an argument.\n",
                        optopt);
            } else if (isprint(optopt))
                fprintf(stderr,
                        "Unknown option `-%c'.\n",
                        optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
    }
    LinkedList movie_list = ReadFile(filename);
    Index index;
    // STEP 9(Student): Create the index properly.
    index = BuildMovieIndex(movie_list, field);
    PrintReport(index);
    DestroyLinkedList(movie_list, &DestroyNothing);
    DestroyIndex(index);

    return 0;
}

// reference
// https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt