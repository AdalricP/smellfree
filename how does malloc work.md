# Heap

You're writing along some code but you need some extra memory, well you can use the heap to allocate memory with malloc and free it when you don't need it anymore. 

Well how do you get the memory in the first place?

Well we use something called `mmap()` which creates a map in the virtual address space of the calling process.

`void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);`

The address is stored in `addr` and `length` specifies length of the mapping. If `addr` is `NULL` then the kernal chooses an address to create the mapping. If `addr` is not null then the kernal takes it as a hint to where to place the mapping.
It returns the address of the new mapping as the result of the call.


`int munmap(void *addr, size_t length)` is used for unmapping.

There's also `int brk(void *addr);`  which changes the size of an already used memory segment.

so well, why don't we just use `mmap` and `brk`? why use `malloc()` and `free()`? 

`malloc()` is just a neat mapping which helps us use `mmap` and `brk`, it calls `mmap` and `brk` but it also helps us organize and manage that memory.

## what the hell is a heap?
Think: Heap is a big chunk of memory. And you can do whatever you want in that memory region.

## wat malloc do
Say I want to allocate 8 bytes, I'll call `malloc(8)` which will block out 8 bytes on the heap for us, this will return the address of the heap where the 8 bytes are blocked out. We'll write some whatever on that address.

Cool, now if I call `malloc(8)` again it blocks out 8 more bytes, but how does malloc know where to block now? Which parts are free which arent?

We use the dlmalloc (douglas lea's implementation) for this

For each chunk we'll store the blocksize of that chunk before it

and 4 bytes before that as well for how `free()` is implemented (but let's ignore that for now)

When the first malloc is called, we obviously know the beginning of the heap, so we store the size we want to allocate, and return the address where the chunk part usable by the programmer starts.

Now when the second is called, how do we know where to put the new chunk?

We'll look at the address pointing to the beginning of the heap, move to the chunk that stores the size of what we just wrote, and that will give us where to go next.

The return address gives us the part where we can write data to; if we look at the address just before that we get the size of our current chunk! so we can add that size to get the next chunk
