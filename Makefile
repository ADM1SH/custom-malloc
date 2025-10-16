# Makefile for the custom memory allocator project.

# Compiler and compiler flags.
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude

# Source files for the minimal and advanced allocators.
SRC_MIN = src/mymalloc_min.c
SRC_ADV = src/mymalloc_adv.c

# Source files for the test suites.
SRC_TEST_MIN = src/tests_min.c
SRC_TEST_ADV = src/tests_adv.c
SRC_TEST_STRESS = src/tests_stress.c

# Object files are generated from the source files.
OBJ_MIN = $(SRC_MIN:.c=.o)
OBJ_ADV = $(SRC_ADV:.c=.o)
OBJ_TEST_MIN = $(SRC_TEST_MIN:.c=.o)
OBJ_TEST_ADV = $(SRC_TEST_ADV:.c=.o)

# The default target builds both the minimal and advanced allocators.
all: min adv

# Target to build the minimal allocator with its test suite.
min: $(OBJ_MIN) $(OBJ_TEST_MIN)
	$(CC) $(CFLAGS) -o mymalloc_min $(OBJ_MIN) $(OBJ_TEST_MIN)

# Target to build the advanced allocator with its test suite.
adv: $(OBJ_ADV) $(OBJ_TEST_ADV)
	$(CC) $(CFLAGS) -o mymalloc_adv $(OBJ_ADV) $(OBJ_TEST_ADV)

# Target to build and run the stress test for the minimal allocator.
test_min_stress: $(OBJ_MIN)
	$(CC) $(CFLAGS) -c $(SRC_TEST_STRESS) -o src/tests_stress_min.o
	$(CC) $(CFLAGS) -o test_min_stress $(OBJ_MIN) src/tests_stress_min.o

# Target to build and run the stress test for the advanced allocator.
# The ADVANCED_ALLOCATOR macro is defined to switch to the advanced allocator.
test_adv_stress: $(OBJ_ADV)
	$(CC) $(CFLAGS) -DADVANCED_ALLOCATOR -c $(SRC_TEST_STRESS) -o src/tests_stress_adv.o
	$(CC) $(CFLAGS) -o test_adv_stress $(OBJ_ADV) src/tests_stress_adv.o

# Target to clean up the project directory by removing object files and executables.
clean:
	rm -f src/*.o mymalloc_min mymalloc_adv test_min_stress test_adv_stress