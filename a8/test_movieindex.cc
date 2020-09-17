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
#include <stdint.h>
#include <assert.h>


#include "gtest/gtest.h"

extern "C" {
#include "./MovieIndex.h"
#include "htll/LinkedList.h"
#include "MovieIndex.h"
#include "Movie.h"
#include "MovieReport.h"
#include "FileParser.h"
#include <string.h>
}

const char *movie_row_A = "9.3|The Shawshank Redemption|R|Crime|142|Tim Robbins,Morgan Freeman,Bob Gunton";
const char *movie_row_B = "7.4|Back to the Future Part III|PG|Adventure|118|Michael J. Fox,Christopher Lloyd,Mary Steenburgen";

void DestroyLLMovie(void *payload) {
    DestroyMovie((Movie *) payload);
}

void DestroyNothing2(void *thing) {
    // Helper method to destroy the LinkedList.
}

TEST(MovieSet, CreateDestroy
) {
MovieSet set = CreateMovieSet("My test set");
ASSERT_NE(set,
nullptr);
DestroyMovieSet(set);
}

TEST(MovieSet, AddOneMovie
) {
MovieSet set = CreateMovieSet("My test set");
ASSERT_NE(set,
nullptr);
ASSERT_EQ(NumElementsInLinkedList(set->movies),
0);

char row[1000];
strcpy(row, movie_row_A
);
MoviePtr movie = CreateMovieFromRow(row);
int res = AddMovieToSet(set, movie);
ASSERT_EQ(res,
0);
ASSERT_EQ(NumElementsInLinkedList(set->movies),
1);

DestroyMovieSet(set);
}

TEST(MovieIndex, CreateDestroy
) {
Index index = CreateIndex();
ASSERT_NE(index,
nullptr);
DestroyIndex(index);
}

TEST(MovieIndex, AddMovieToIndex
) {
// Copying the string from a const to an array
// (to be more similar to the actual use case)
char row[1000];
strcpy(row, movie_row_A
);
// Take a movie
MoviePtr m1 = CreateMovieFromRow(row);
strcpy(row, movie_row_B
);
MoviePtr m2 = CreateMovieFromRow(row);

// Create Index
Index index = CreateIndex();

// Add movie to index
AddMovieToIndex(index, m1, ContentRating
);

// DONE! Peilin: Check that movie is in index
// HTKeyValuePtr result;
// ASSERT_EQ(LookupInHashtable(index, ComputeKey(m1, ContentRating, 0), result), 0);

// Check size/num elements (should be num of types, not of movies)
ASSERT_EQ(NumElemsInHashtable(index),
1);

// Try to insert movie again
int result = AddMovieToIndex(index, m1, ContentRating);
// DONE! Peilin: test CheckDuplicate
ASSERT_EQ(NumElemsInHashtable(index),
1);
ASSERT_EQ(result,
-1);

// Add another movie to the index (same IndexType)
AddMovieToIndex(index, m2, ContentRating
);
// ASSERT_EQ(LookupInHashtable(index, ComputeKey(m2, ContentRating, 0), result), 0);
ASSERT_EQ(NumElemsInHashtable(index),
2);
// DONE! Peilin check the num elements

// Destroy movie index
DestroyIndex(index);
}

TEST(MovieIndex, BuildMovieIndexFromFile
) {
LinkedList movie_list = ReadFile(const_cast<char *>("data/test"));
ASSERT_EQ(5u,
NumElementsInLinkedList(movie_list)
);
Index index = BuildMovieIndex(movie_list, Genre);

// Do a few spot checks.
// E.g., Pull out a MovieSet from the Hashtable;
// Check to see if the set has expected number of movies,
// a particular movie, etc.
ASSERT_EQ(NumElemsInHashtable(index),
2);
// DONE! Peilin

DestroyLinkedList(movie_list, &DestroyNothing2
);
// DONE! Peilin: add DestroyLinkedList
DestroyIndex(index);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
