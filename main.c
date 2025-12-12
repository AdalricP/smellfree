#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define heap_size (1024*1024)
#define LARGE_SIZE (64*1024)

void* heap_start=NULL;
void* heap_end=NULL;
void* smelloc(size_t size){
    if (size >= LARGE_SIZE) {
        void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) return NULL;
        return ptr;
    }
    if(heap_start == NULL){
        heap_start = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (heap_start == MAP_FAILED) return NULL;
        heap_end = heap_start;
    }
    size_t aligned_size = (size + 7) & ~7;
    size_t chunk_size = sizeof(char) + sizeof(size_t) + aligned_size;
    if (heap_end + chunk_size > heap_start + heap_size) {
        void* new_heap = mmap(heap_end, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (new_heap == MAP_FAILED) return NULL;
    }
    *(char*)heap_end = 0;
    heap_end += sizeof(char);
    *(size_t*)heap_end = chunk_size;
    heap_end += sizeof(size_t);
    void* final_ptr = heap_end;
    heap_end += aligned_size;
    return final_ptr;
}
void smellfree(void* ptr) { // deodarant implementation
    if (ptr == NULL) return;
    char* chunk_start = (char*)ptr - sizeof(char) - sizeof(size_t);
    if (chunk_start < (char*)heap_start || chunk_start >= (char*)heap_end) return;
    *chunk_start = 1; 
}


int main(void) {
    void *a = malloc(10);
    void *b = smelloc(10);
    
    printf("%p\n", a);
    printf("%p\n", b);
    free(a);
    smellfree(b);
    return 0;
}