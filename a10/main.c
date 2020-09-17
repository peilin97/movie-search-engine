#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "Hashtable.h"
#include "LinkedList.h"

#include "MovieSet.h"
#include "DocIdMap.h"
#include "DirectoryParser.h"
#include "DirectoryParser_MT.h"
#include "FileParser.h"
#include "FileCrawler.h"
#include "MovieIndex.h"
#include "MovieTitleIndex.h"
#include "Movie.h"
#include "QueryProcessor.h"
#include "MovieReport.h"
#include "Assert007.h"
#include "Util.h"

DocIdMap docs;
MovieTitleIndex docIndex;

#define kMaxRowLength 1000

/**
 * Open the specified file, read the specified row into the
 * provided pointer to the movie.
 */
int CreateMovieFromFileRow(char *file, long row_id, Movie** movie) {
  FILE *cfPtr;

  if ((cfPtr = fopen(file, "r")) == NULL) {
    printf("File could not be opened\n");
    return -1;
  } else {
    Assert007(cfPtr != NULL);
    char row[kMaxRowLength];

    int i = 0;

    while (i < row_id) {
      char* res = fgets(row, kMaxRowLength, cfPtr);
      if (res == NULL) break;
      i++;
    }
    char* res = fgets(row, kMaxRowLength, cfPtr);
    // Got the line; create a movie from it
    *movie = CreateMovieFromRow(row);
    Assert007(movie != NULL);
    fclose(cfPtr);
    return 0;
  }
}

void DoPrep(char *dir) {
  printf("Crawling directory tree starting at: %s\n", dir);
  // Create a DocIdMap
  docs = CreateDocIdMap();
  CrawlFilesToMap(dir, docs);

  printf("Crawled %d files.\n", NumElemsInHashtable(docs));

  // Create the index
  docIndex = CreateMovieTitleIndex();

  // Index the files
  printf("Parsing and indexing files...\n");

  // TODO(student): You can change this to you your MT or not. 
  //  ParseTheFiles(docs, docIndex);
  ParseTheFiles_MT(docs, docIndex, 5);
  printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
}

void RunQuery(char *term) {
  SearchResultIter results = FindMovies(docIndex, term);
  LinkedList movies = CreateLinkedList();

  if (results == NULL) {
    printf("No results for this term. Please try another.\n");
    DestroyLinkedList(movies, NullFree);
    return;
  } else {
    SearchResult sr = (SearchResult)malloc(sizeof(*sr));
    if (sr == NULL) {
      printf("Couldn't malloc SearchResult in main.c\n");
      DestroyLinkedList(movies, NullFree);
      return;
    }
    int result;
    char *filename;
    // Get the last
    SearchResultGet(results, sr);
    filename = GetFileFromId(docs, sr->doc_id);

    Movie *movie;
    CreateMovieFromFileRow(filename, sr->row_id, &movie);
    InsertLinkedList(movies, movie);

    // Check if there are more
    while (SearchResultIterHasMore(results) != 0) {
      result =  SearchResultNext(results);
      if (result < 0) {
        printf("error retrieving result\n");
        break;
      }
      SearchResultGet(results, sr);
      char *filename = GetFileFromId(docs, sr->doc_id);

      Movie *movie;
      //      Movie* movie = (Movie*)malloc(sizeof(Movie));// aha
      CreateMovieFromFileRow(filename, sr->row_id, &movie);
      InsertLinkedList(movies, movie);
    }

    free(sr);
    DestroySearchResultIter(results);
  }
  // Now that you have all the search results, print them out nicely.
  Index index = BuildMovieIndex(movies, Type);
  PrintReport(index);

  DestroyIndex(index);
}

void RunQueries() {
  char input[1000];
  while (1) {
    printf("\nEnter a term to search for, or q to quit: ");
    scanf("%s", input);

    if (strlen(input) == 1 &&
        (input[0] == 'q')) {
          printf("Thanks for playing! \n");
          return;
      }

    printf("\n");
    RunQuery(input);
  }
}

int main(int argc, char *argv[]) {
  // Check arguments
  if (argc != 2) {
    printf("Wrong number of arguments.\n");
    printf("usage: main <directory_to_crawl>\n");
    return 0;
  }

  DoPrep(argv[1]);
  RunQueries();

  DestroyMovieTitleIndex(docIndex);
  DestroyDocIdMap(docs);
}
