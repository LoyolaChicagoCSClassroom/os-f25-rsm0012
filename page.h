#ifndef __PAGE_H__
#define __PAGE_H__
#include <stdint.h>

struct ppage {
    struct ppage *next;
    struct ppage *prev;
    void *physical_addr;
};

// Global pointer to free physical pages list
extern struct ppage *free_physical_pages;

// Function declarations
void init_pfa_list(void);
struct ppage *allocate_physical_pages(unsigned int npages);
void free_physical_pages_list(struct ppage *ppage_list);

// Page Directory Entry
struct page_directory_entry {
   uint32_t present       : 1;   // Page present in memory
   uint32_t rw            : 1;   // Read-only if clear
   uint32_t user          : 1;   // Supervisor only if clear
   uint32_t writethru     : 1;   // Cache this directory as write-thru only
   uint32_t cachedisabled : 1;   // Disable cache on this page table?
   uint32_t accessed      : 1;   // Has it been accessed?
   uint32_t pagesize      : 1;   // Page size (0 = 4KB)
   uint32_t ignored       : 2;   // Ignored bits
   uint32_t os_specific   : 3;   // OS-specific bits
   uint32_t frame         : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed));

// Page Table Entry
struct page {
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed?
   uint32_t dirty      : 1;   // Has the page been written to?
   uint32_t unused     : 7;   // Unused and reserved bits
   uint32_t frame      : 20;  // Frame address
} __attribute__((packed));

// Function declaration for map_pages
void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd);

#endif
