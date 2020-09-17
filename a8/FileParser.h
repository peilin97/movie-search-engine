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

#ifndef _FILEPARSER_H
#define _FILEPARSER_H

#include "htll/LinkedList.h"
#include "Movie.h"
#include "MovieIndex.h"


/**
 * Reads in the specified file with a given structure and reads in each row,
 * creating a Movie from each row.
 *
 * star_rating|title|content_rating|genre|duration|actors_list
 * 7.4|Back to the Future Part III|PG|Adventure|118|Michael J. Fox,Christopher Lloyd,Mary Steenburgen
 *
 * Fields are separated by a pipe (|), and a dash (-) specifies an empty value.
 *
 * INPUT: filename: the data file with movies
 *
 * Returns a LinkedList of Movie structs from the specified file
 */

LinkedList ReadFile(char* filename);

/**
 * Given a char* that is a row in the data file,
 * Creates and populates a Movie struct accordingly.
 * Expected sample row:
 *
 * star_rating|title|content_rating|genre|duration|actors_list
 * 7.4|Back to the Future Part III|PG|Adventure|118|Michael J. Fox,Christopher Lloyd,Mary Steenburgen
 *
 * Fields are separated by a pipe (|), and a dash (-) specifies an empty value.
 *
 * Assumes that, where appropriate, new strings and arrays will be malloc'd for the
 * new Movie to point at. 
 *
 * Returns: A pointer to a Movie struct that has been allocated and populated
 *    with the values in the dataRow.
 * 
 */

Movie* CreateMovieFromRow(char *dataRow);

#endif  // _FILEPARSER_H
