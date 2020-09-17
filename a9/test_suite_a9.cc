/*
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

#include "gtest/gtest.h"

extern "C" {
#include <string.h>

#include "Hashtable.h"
#include "LinkedList.h"
#include "DirectoryParser.h"
#include "DocIdMap.h"
#include "DocSet.h"
#include "FileCrawler.h"
#include "MovieTitleIndex.h"
#include "QueryProcessor.h"
}


const char* kMovie1 =
    "1|movie|Sleepless in Seattle|Sleepless in Seattle|0|1984|-|120|Comedy,Romance";
const char* kMovie2 = "tt3247810|tvEpisode|Jonathan Kite|Jonathan Kite|0|2012|-|-|Comedy";
const char* kMovie3 = "tt0003609|movie|Alexandra|Alexandra|0|1915|-|-|-";
const char* kMovie4 = "tt0003620|short|Among the Mourners|Among the Mourners|0|1914|-|-|Comedy,Short"; 



TEST(DirectoryParser, IndexTheFile_OneFile) {
  //int IndexTheFile(char *file, uint64_t docId, Index index);
  char *f1 = (char*)(malloc(sizeof(char)*30));
  strcpy(f1, "data_tiny/fourth/fifth/tinyaa");
  // program to get unique words in this file:
  // awk 'BEGIN { FS = "|" } ; { print $3 }' data_tiny/fourth/fifth/tinyaa  |
  //   awk 'BEGIN{FS=" ";OFS="\n"}; {for (i=1;i<=NF;i++){print $i}; };' |
  //   sort | uniq | wc -l
  // 30 unique words
  // Check for:
  // coffee, something, or, the (2 different records), of, lucky

  // Create an index
  MovieTitleIndex ind = CreateMovieTitleIndex();
  ASSERT_NE(ind, nullptr);

  int num_records = IndexTheFile(f1, 42u, ind);
  EXPECT_EQ(num_records, 10);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 29);

  // There should be one entry for "Coffee"
  DocumentSet set = GetDocumentSet(ind, "coffee");
  ASSERT_NE(set, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);
  EXPECT_STREQ(set->desc, "coffee");

  HTKeyValue kvp;
  // Doc index has a doc_id of 42 in it
  EXPECT_EQ(LookupInHashtable(set->doc_index, 42u, &kvp), 0);
  LinkedList offset_list = (LinkedList)kvp.value;
  ASSERT_NE(offset_list, nullptr);
  EXPECT_EQ(NumElementsInLinkedList(offset_list), 1u);
  // TODO: Could check that the rowID is as expected.
  
  
  // There should (ideally) be one entry for "Or"
  set = GetDocumentSet(ind, "or");
  ASSERT_NE(set, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);
  EXPECT_STREQ(set->desc, "or");

  set = GetDocumentSet(ind, "the");
  ASSERT_NE(set, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);
  EXPECT_STREQ(set->desc, "the");
  // Doc index has a doc_id of 42 in it
  EXPECT_EQ(LookupInHashtable(set->doc_index, 42u, &kvp), 0);
  offset_list = (LinkedList)kvp.value;
  ASSERT_NE(offset_list, nullptr);
  // "the" was in two different records.
  EXPECT_EQ(NumElementsInLinkedList(offset_list), 2u);

  DestroyMovieTitleIndex(ind);
  free(f1);
}

// TODO(adrienne): add a check to make sure the fisrt and/or last movies aren't added twice.
TEST(DirectoryParser, ParseTheFiles) {
  // Given a docIdMap and an index,
  // the index should contain the info.
  DocIdMap docs = CreateDocIdMap();
  ASSERT_NE(docs, nullptr);
  // Some files
  // Not ideal; opportunity for improvement next time around.
  // TODO(adrienne): Figure how to improve.
  char *f1 = (char*)(malloc(sizeof(char)*30));
  strcpy(f1, "data_tiny/fourth/fifth/tinyaa");
  PutFileInMap(f1, docs);
  EXPECT_EQ(NumElemsInHashtable(docs), 1);

  char *f2 = (char*)(malloc(sizeof(char)*30));
  strcpy(f2, "data_tiny/tinybu"); 
  PutFileInMap(f2, docs);
  EXPECT_EQ(NumElemsInHashtable(docs), 2);

  MovieTitleIndex ind = CreateMovieTitleIndex();
  ASSERT_NE(ind, nullptr);

  ParseTheFiles(docs, ind);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 57);
  
  DocumentSet set = GetDocumentSet(ind, "coffee.");
  EXPECT_EQ(set, nullptr);
  set = GetDocumentSet(ind, "Coffee");
  EXPECT_NE(set, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);
  set = GetDocumentSet(ind, "coffee");
  EXPECT_NE(set, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);

  set = GetDocumentSet(ind, "tales");
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);

  set = GetDocumentSet(ind, "doudou");
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);

  set = GetDocumentSet(ind, "&");
  EXPECT_EQ(set, nullptr);
  
  set = GetDocumentSet(ind, "foobar");
  EXPECT_EQ(set, nullptr);

  DestroyMovieTitleIndex(ind);
  DestroyDocIdMap(docs);
  // Not freeing f1 because DestroyDocIdMap does
}

TEST(DocumentSet, CreateDestroy) {
  // Create a movieSet
  DocumentSet mset = CreateDocumentSet(((char*)"example"));
  ASSERT_NE(mset, nullptr);
  ASSERT_STREQ(mset->desc, "example");
  ASSERT_NE(mset->doc_index, nullptr);
  ASSERT_EQ(NumElemsInHashtable(mset->doc_index), 0);

  // Destroy movieSet
  DestroyDocumentSet(mset);
}

TEST(DocSet, AddMovieToSet) {
  // Create a movieSet
  DocumentSet mset = CreateDocumentSet(((char*)"example"));
  ASSERT_NE(mset, nullptr);
  ASSERT_STREQ(mset->desc, "example");
  ASSERT_NE(mset->doc_index, nullptr);
  EXPECT_EQ(NumElemsInHashtable(mset->doc_index), 0);

  int doc_id = 42;
  int row_id = 15;
  AddDocInfoToSet(mset, doc_id, row_id);
  EXPECT_EQ(NumElemsInHashtable(mset->doc_index), 1);

  // Add a movie with the same doc_id, should not have more docs
  AddDocInfoToSet(mset, doc_id, row_id + 1);
  EXPECT_EQ(NumElemsInHashtable(mset->doc_index), 1);

  AddDocInfoToSet(mset, doc_id + 1, row_id);
  EXPECT_EQ(NumElemsInHashtable(mset->doc_index), 2);

  // TODO(adrienne): Check that the right row number are in the doc_index

  EXPECT_EQ(DocumentSetContainsDoc(mset, doc_id), 0);
  EXPECT_NE(DocumentSetContainsDoc(mset, 75), 0);

  // Destroy movieSet
  DestroyDocumentSet(mset);
}

TEST(DocIdMap, Full) {
  // Create DocIdMap
  DocIdMap docs = CreateDocIdMap();
  EXPECT_NE(docs, nullptr);

  // Some files
  // Not ideal; opportunity for improvement next time around.
  char *f1 = (char*)(malloc(sizeof(char)*10));
  strcpy(f1, "file1");
  PutFileInMap(f1, docs);
  EXPECT_EQ(NumElemsInHashtable(docs), 1);

  HTKeyValue kvp;
  LookupInHashtable(docs, 1, &kvp);
  EXPECT_STREQ((const char*)kvp.value, "file1");

  char *f2 = (char*)(malloc(sizeof(char)*10));
  strcpy(f2, "file2");
  PutFileInMap(f2, docs);
  EXPECT_EQ(NumElemsInHashtable(docs), 2);

  HTKeyValue kvp2;
  LookupInHashtable(docs, 2, &kvp2);
  EXPECT_STREQ((const char*)kvp2.value, "file2");

  char *out = GetFileFromId(docs, 1);
  EXPECT_STREQ(out, "file1");

  DocIdIter docIter = CreateDocIdIterator(docs);

  int numItems = 0;
  while (HTIteratorHasMore(docIter) != 0) {
    HTKeyValue kvp3;
    HTIteratorGet(docIter, &kvp3);
    EXPECT_TRUE(numItems < 2);
    numItems++;
    HTIteratorNext(docIter);
  }

  EXPECT_EQ(numItems, 1); // Last element not counted

  DestroyDocIdIterator(docIter);

  DestroyDocIdMap(docs);
}

TEST(FileCrawler, CrawlFilesToMap) {
  // Create a DocIdMap
  DocIdMap docs = CreateDocIdMap();
  // Choose a directory
  CrawlFilesToMap("data_tiny/", docs);

  EXPECT_EQ(NumElemsInHashtable(docs), 10);

  // TODO: This assumes that the ids are unique, starting at 0. 
  int ids[11] = {0};

  HTIter iter = CreateHashtableIterator(docs);
  HTKeyValue kv;

  while (HTIteratorHasMore(iter) != 0) {
    HTIteratorGet(iter, &kv);
    //   printf("docId: %d\tfilename: %s\n", kv->key, kv->value);
    ids[kv.key]++;
    HTIteratorNext(iter);
  }
  // Get the last
  HTIteratorGet(iter, &kv);
  //   printf("docId: %d\tfilename: %s\n", kv->key, kv->value);
  ids[kv.key]++;

  // Should be a unique id for each file.
  for (int i=1; i<=10; i++) {
    EXPECT_EQ(ids[i], 1);
  }

  DestroyHashtableIterator(iter);
  DestroyDocIdMap(docs);
}

TEST(MovieTitleIndex, Full) {
  // Create a movie index
  MovieTitleIndex ind = CreateMovieTitleIndex();
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 0);
  
  // Add some movies to the index
  Movie *m1 = CreateMovie();
  char* title = (char*)(malloc(sizeof(char)*50));
  strcpy(title, "Sleepless in Seattle");

  m1->title = title;
  AddMovieTitleToIndex(ind, m1, 1, 19);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 3);
  
  // Check the indices
  DocumentSet set = GetDocumentSet(ind, "seattle");
  EXPECT_STREQ(set->desc, "seattle");
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);
  HTKeyValue kvp;
  LookupInHashtable(set->doc_index, 1, &kvp);

  EXPECT_EQ(NumElementsInLinkedList((LinkedList)kvp.value), 1u);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 3);

  // ==========================
  // Second Movie
    
  Movie *m2 = CreateMovie();
  char* title2 = (char*)(malloc(sizeof(char)*50));
  strcpy(title2, "For the love of the Foo. In Foo not Foo.");
  int doc_id2 = 42;
  int row_id2 = 4;
  
  m2->title = title2;
  // Index, Movie, doc_id, row_id
  AddMovieTitleToIndex(ind, m2, doc_id2, row_id2); 
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 9);

  DocumentSet set2 = GetDocumentSet(ind, "foo.");
  // Should not find anything with a punctuation
  ASSERT_EQ(set2, nullptr);

  set2 = GetDocumentSet(ind, "foo");
  ASSERT_NE(set2, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set2->doc_index), 1);
  EXPECT_STREQ(set2->desc, "foo");
  
  DocumentSet set3 = GetDocumentSet(ind, "in");
  ASSERT_NE(set3, nullptr);
  EXPECT_EQ(NumElemsInHashtable(set3->doc_index), 2);
  EXPECT_STREQ(set3->desc, "in");
  
  DestroyMovieTitleIndex(ind);
  DestroyMovie(m1);
  DestroyMovie(m2);

  //  free(title);
  //free(title2);
}


TEST(MovieTitleIndex, GetDocumentSet) {
  // TODO: Implement this test.

  // Create a movie index
  MovieTitleIndex ind = CreateMovieTitleIndex();
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 0);
  
  // Add some movies to the index
  Movie *m1 = CreateMovie();
  SetMovieTitle(m1, "Sleepless in Seattle");
  AddMovieTitleToIndex(ind, m1, 1, 19);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 3);

  // ==========================
  // Second Movie
    
  Movie *m2 = CreateMovie();
  int doc_id2 = 42;
  int row_id2 = 4;  
  SetMovieTitle(m2, "For the love of the Foo. In Foo not Foo.");
  AddMovieTitleToIndex(ind, m2, doc_id2, row_id2);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 9);
  
  Movie* m3 = CreateMovieFromRow(kMovie2);
  AddMovieTitleToIndex(ind, m3, doc_id2, row_id2 + 1);
  EXPECT_EQ(NumElemsInHashtable(ind->ht), 11);
  
  // Add a bunch of movies with different vals for the index

  // Get the set
  DocumentSet set = GetDocumentSet(ind, "seattle");
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 1);
  EXPECT_STREQ(set->desc, "seattle");

  set = GetDocumentSet(ind, "in");
  EXPECT_EQ(NumElemsInHashtable(set->doc_index), 2);
  EXPECT_STREQ(set->desc, "in");

  // TODO: Check that the movies are all in the set


  // DestroyDocumentSet(set);

    
  // Destroy the movie index
  DestroyMovieTitleIndex(ind);
    DestroyMovie(m1);
    DestroyMovie(m2);
    DestroyMovie(m3);
  
}




TEST(QueryProcessor, FindMovies) {

  
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
