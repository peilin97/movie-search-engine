# define the commands we'll use for compilation and library building
AR = ar
ARFLAGS = rcs
CC = gcc

GOOGLE_TEST_INCLUDE=${HOME}/src/googletest-release-1.8.0/googletest/include/

RED='\033[1;31m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'
YELLOW='\033[1;33m'
ORANGE='\033[0;33m'
NC='\033[0m' # No Color

# define useful flags to cc/ld/etc.
CFLAGS = -g -Wall -I. -Iheaders -Ia8 -L. -lHtll -lA8
LDFLAGS = -L.

#define common dependencies
OBJS = DocIdMap.o DirectoryParser.o FileCrawler.o \
	QueryProcessor.o \
	DocSet.o MovieTitleIndex.o
HEADERS = DirectoryParser.h FileCrawler.h DocIdMap.h\
	DocSet.h \
	MovieTitleIndex.h

# compile everything
all: main test 

main: main.c $(OBJS)
	$(CC) -o main main.c $(OBJS) $(CFLAGS)
	@echo -e ${YELLOW}
	@echo ===========================
	@echo Run main by running ./main dir/
	@echo \(where dir is the directory to look for files to index\)
	@echo Example:
	@echo ./main data_tiny/
	@echo -e ${BLUE}$
	@echo \(DON\'T FORGET THE TRAILING SLASH\)
	@echo -e ${YELLOW}
	@echo ===========================
	@echo -e ${NC}

%.o: %.c $(HEADERS) FORCE
	$(CC) $(CFLAGS) -c $<

test: test_suite.o $(OBJS)
	g++ -g -o test_suite test_suite_a9.o $(OBJS)\
		 -L${HOME}/lib/gtest -lgtest -lpthread $(CFLAGS)
	@echo -e ${YELLOW}
	@echo ===========================
	@echo Run tests by running ./test_suite
	@echo ===========================
	@echo -e ${NC}

test_suite.o: test_suite_a9.cc
	g++ -g -c -Wall -I $(GOOGLE_TEST_INCLUDE) test_suite_a9.cc \
		-o test_suite_a9.o $(CFLAGS)

clean: FORCE
	/bin/rm -f *.o *~ main test_suite

FORCE:
