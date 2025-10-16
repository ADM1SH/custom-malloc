CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude

SRC_MIN = src/mymalloc_min.c
SRC_ADV = src/mymalloc_adv.c
SRC_TEST_MIN = src/tests_min.c
SRC_TEST_ADV = src/tests_adv.c

OBJ_MIN = $(SRC_MIN:.c=.o)
OBJ_ADV = $(SRC_ADV:.c=.o)
OBJ_TEST_MIN = $(SRC_TEST_MIN:.c=.o)
OBJ_TEST_ADV = $(SRC_TEST_ADV:.c=.o)

all: min adv

min: $(OBJ_MIN) $(OBJ_TEST_MIN)
	$(CC) $(CFLAGS) -o mymalloc_min $(OBJ_MIN) $(OBJ_TEST_MIN)

adv: $(OBJ_ADV) $(OBJ_TEST_ADV)
	$(CC) $(CFLAGS) -o mymalloc_adv $(OBJ_ADV) $(OBJ_TEST_ADV)

clean:
	rm -f src/*.o mymalloc_min mymalloc_adv