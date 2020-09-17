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

#include "Hashtable.h"

#ifndef HASHTABLE_PRIV_H
#define HASHTABLE_PRIV_H


// This is the struct we use to represent an iterator.
typedef struct ht_itrec {
  Hashtable  ht;       // the HT we're pointing into
  int   which_bucket;  // which bucket are we in?
  LLIter bucket_iter;  // iterator for the bucket, or NULL
} HTIterRecord;


void ResizeHashtable(Hashtable ht);

int HashKeyToBucketNum(Hashtable ht, uint64_t key); 

double GetAlpha(Hashtable *hashtable);


#endif // HASHTABLE_PRIV_H
