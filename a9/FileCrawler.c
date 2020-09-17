/*
 *  Modified by Peilin Guo, Mar 30 2020
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "FileCrawler.h"
#include "DocIdMap.h"
#include "LinkedList.h"


void CrawlFilesToMap(const char *dir, DocIdMap map) {
    // STEP 3: Implement the file crawler.
    // Use namelist (declared below) to find files and put in map.
    // NOTE: There may be nested folders.
    // Be sure to lookup how scandir works. Don't forget about memory use.
    struct stat s;
    struct dirent **namelist;
    int n;
    n = scandir(dir, &namelist, 0, alphasort);
    for (int i = 2; i < n; i++) {
        char path[100];
        snprintf(path, 100, "%s%s", dir, namelist[i]->d_name);
        stat(path, &s);
        if (S_ISDIR(s.st_mode) == 0) {
            // when it is not a folder
            printf("adding file to map: %s\n", path);
            char *malloc_path = (char *) malloc((strlen(path) + 1) * sizeof(char));
            snprintf(malloc_path, strlen(path) + 1, "%s", path);
            printf("malloc_path = %s\n", malloc_path);
            PutFileInMap(malloc_path, map);
        } else {
            // if it is a nested folder
            strcat(path, "/");
            printf("crawling dir: %s\n", path);
            CrawlFilesToMap(path, map);
        }
    }
    for (int i = 0; i < n; i++) {
        free(namelist[i]);
        namelist[i] = NULL;
    }
    free(namelist);
    namelist = NULL;
}
