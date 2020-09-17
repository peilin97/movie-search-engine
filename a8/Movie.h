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

#ifndef MOVIE_H
#define MOVIE_H

/**
 * A Movie is a struct that holds information about a Movie.
 *
 * Specifies fields of Movies we care about, to index.
 */
enum IndexField {Genre,
                Actor,
                StarRating,
                ContentRating};

/**
 * A Movie is a struct that holds information about a Movie.
 * Unset fields should be -1 if they are ints; NULL if they are pointers.
 */
typedef struct movie {
  char *title;
  double star_rating;
  char *content_rating;
  char *genre;
  int duration;
  char **actor_list; // This looks like a **, but it's really just an array of strings.
  int num_actors;
} Movie, *MoviePtr;


/**
 *  Creates a movie.
 *
 *  Returns a pointer to an unpopulated Movie (initialized with default/null values).
 */
Movie* CreateMovie();

/**
 *  Destroys a movie, freeing up all char*s in the struct.
 */
void DestroyMovie(Movie* movie);

#endif // MOVIE
