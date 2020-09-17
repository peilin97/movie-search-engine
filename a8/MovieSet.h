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

#ifndef MOVIESET_H
#define MOVIESET_H

#include "htll/Hashtable.h"
#include "Movie.h"

/**
 * A MovieSet is a set of movies. It contains a string to describe the
 * movies, and a linked list of Movie structs.
 * It is intended to keep collections of related Movies.
 *
 */
typedef struct movieSet {
  char *desc;
  LinkedList movies;
} *MovieSet;

/**
 * Adds a Movie to the given set.
 * Returns: 0 if successful.
 *
 */
int AddMovieToSet(MovieSet set, Movie *movie);

/**
 * Destroys the MovieSet.
 *
 * Frees the Movies in the set, and all char*s that are fields of the struct.
 */
void DestroyMovieSet(MovieSet set);

/**
 * Creates a MovieSet with the provided description.
 *
 * Returns: A pointer that points to a struct movieSet.
 */
MovieSet CreateMovieSet(char *desc);

#endif
