# Custom malloc() – Heap Memory Simulator

This project reimplements the C standard library’s malloc, free, calloc, and realloc using a fixed-size static array to simulate the heap.
It includes two allocators:
1.Minimal Allocator – straightforward linked-list design for education.
2.Advanced Allocator – optimized allocator with boundary tags and segregated bins.

## Features
•Works without brk() or mmap()
•Demonstrates heap fragmentation and coalescing
•Printable heap layout for debugging
•Easily extendable (add best-fit, next-fit, or alignment policies)

## Build Instructions

```
make            # builds both versions
./mymalloc_min  # run minimal allocator demo
./mymalloc_adv  # run advanced allocator demo
```

## Example Output

```
=== Testing Minimal Allocator ===
Heap dump:
 Block 0x1000 | size=992 | free=0 | next=0x1400
 Block 0x1400 | size=508 | free=1 | next=(nil)

=== Testing Advanced Allocator ===
=== Heap bins ===
Bin[0]: [50]
Bin[2]: [300]
```

## Learning Outcomes
•Understand internal design of dynamic memory allocators.
•Learn linked-list and block-header management.
•Visualize fragmentation, block splitting, and merging.
•Gain confidence in systems-level C programming.
