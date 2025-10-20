#ifndef __PAGE_H__
#define __PAGE_H__

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

#endif
