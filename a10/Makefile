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
CFLAGS = -g -Wall -I. -Iheaders -L. -lHtll -lA9 -lA8
LDFLAGS = -L.

#define common dependencies
OBJS = DirectoryParser_MT.o

HEADERS = DirectoryParser_MT.h

# compile everything
all: main benchmarker

benchmarker: Benchmarker.c $(OBJS)
	$(CC) -o benchmarker Benchmarker.c $(OBJS) $(CFLAGS) -lpthread
	@echo -e ${YELLOW}
	@echo ===========================
	@echo Run benchmarker by running ./benchmarker dir_name/
	@echo \(dirname  is the file of movies to use for benchmark\)
	@echo Example:
	@echo ./main data_tiny/
	@echo -e ${BLUE}$
	@echo \(DON\'T FORGET THE TRAILING SLASH\)
	@echo -e ${YELLOW}
	@echo ===========================
	@echo -e ${NC}

main: main.c $(OBJS)
	$(CC) -o main main.c $(OBJS) $(CFLAGS) -lpthread
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

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean: FORCE
	/bin/rm -f *.o *~ main benchmarker

FORCE:
