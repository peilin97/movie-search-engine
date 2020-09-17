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
#include <cstring>

#include "gtest/gtest.h"

extern "C" {
#include "./FileParser.h"
#include "htll/LinkedList.h"
#include "Movie.h"
}

const char *movie_row_A = "9.3|The Shawshank Redemption|R|Crime|142|Tim Robbins,Morgan Freeman,Bob Gunton";
const char *movie_row_B = "7.4|Back to the Future Part III|PG|Adventure|118|Michael J. Fox,Christopher Lloyd,Mary Steenburgen";

void DestroyLLMovie(void *payload) {
    DestroyMovie((Movie *) payload);
}

// TestSuiteName TestCaseName
TEST(Movie, CreateDestroyMovie
) {
Movie *m1 = CreateMovie();
ASSERT_TRUE(m1
!= NULL);

ASSERT_EQ(m1
->content_rating, nullptr);
ASSERT_EQ(m1
->genre, nullptr);
ASSERT_EQ(m1
->actor_list, nullptr);
ASSERT_EQ(m1
->star_rating, 0);
ASSERT_EQ(m1
->duration, 0);
ASSERT_EQ(m1
->num_actors, 0);
ASSERT_EQ(m1
->title, nullptr);

DestroyMovie(m1);
}

TEST(Movie, CreateManualAndDestroyMovie
) {
Movie *m1 = CreateMovie();
m1->
star_rating = 7.5;
m1->
content_rating = "PG";
m1->
title = "Sleepless in Seattle";
m1->
duration = 125;
m1->
genre = "RomCom";
m1->
num_actors = 2;
char *actors[2] = {};
actors[0] = "Tom Hanks";
actors[1] = "Meg Ryan";
m1->
actor_list = actors;

// DestroyMovie(m1);
free(m1);
/*
 * DONE! (Peilin)
 * Because fields of m1 are not created by malloc, we should
 * use free() instead of DestroyMovie() to clear the memory;
 */
}

char *MallocString(const char *str) {
    char *cr = (char *) malloc(sizeof(char) * strlen(str) + 1);
    snprintf(cr, strlen(str), "%s", str);
    return cr;
}

TEST(Movie, CreateWithMallocdData
) {
Movie *m1 = CreateMovie();
m1->
star_rating = 7.5;
m1->
content_rating = MallocString("PG");
m1->
title = MallocString("Sleepless in Seattle");
m1->
duration = 125;
m1->
genre = MallocString("RomCom");
m1->
num_actors = 2;
// char* actors[2] = {};
/*
 * DONE! (Peilin)
 * use malloc to create the actors_list
 */
char **actors = (char **) malloc(2 * sizeof(char *));
actors[0] = MallocString("Tom Hanks");
actors[1] = MallocString("Meg Ryan");
m1->
actor_list = actors;

ASSERT_NE(m1
->content_rating, nullptr);
ASSERT_NE(m1
->title, nullptr);
ASSERT_NE(m1
->genre, nullptr);
ASSERT_NE(m1
->actor_list[0], nullptr);
ASSERT_NE(m1
->actor_list[1], nullptr);
ASSERT_NE(m1
->actor_list, nullptr);

DestroyMovie(m1);

}

TEST(Movie, CreateFromRow
) {
// Copying the string from a const to an array
// (to be more similar to the actual use case)
char row[1000];
snprintf(row,
1000, "%s", movie_row_A);
// Create from a "good" row

Movie *m1 = CreateMovieFromRow(row);
ASSERT_EQ(9.3, m1->star_rating);
ASSERT_EQ(0,
strcmp(m1
->content_rating, "R"));
ASSERT_EQ(0,
strcmp(m1
->title, "The Shawshank Redemption"));
ASSERT_EQ(142, m1->duration);
ASSERT_EQ(0,
strcmp(m1
->genre, "Crime"));
//  ASSERT_EQ(-1, m1->actor_list);
// DONE by Peilin: Check actors
ASSERT_EQ(0,
strcmp(m1
->actor_list[0], "Tim Robbins"));
ASSERT_EQ(0,
strcmp(m1
->actor_list[1], "Morgan Freeman"));
ASSERT_EQ(0,
strcmp(m1
->actor_list[2], "Bob Gunton"));


// TODO: Create from a improper row

DestroyMovie(m1);
}

TEST(FileParser, ReadGoodFile
) {
// read a file
LinkedList movie_list = ReadFile(const_cast<char *>("data/test"));

ASSERT_EQ(5u,
NumElementsInLinkedList(movie_list)
);

DestroyLinkedList(movie_list, &DestroyLLMovie
);
}

TEST(FileParser, ReadNonexistentFile
) {
// try to read a non-existent file
LinkedList movie_list = ReadFile(const_cast<char *>("bogus/file"));
ASSERT_TRUE(movie_list
== NULL);

}

TEST(FileParser, ReadBinaryFile
) {
// try to read a file that contains non-ASCII text
LinkedList movie_list;

movie_list = ReadFile(const_cast<char *>("htll/libhtll.a"));
ASSERT_TRUE(movie_list
== NULL);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
