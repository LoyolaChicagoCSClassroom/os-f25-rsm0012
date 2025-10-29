#include "page.h"
#include <stddef.h>

struct ppage physical_page_array[128];
struct ppage *free_physical_pages = NULL;

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));

void init_pfa_list(void) {
    for (int i = 0; i < 128; i++) {
        physical_page_array[i].physical_addr = (void *)(i * 0x200000);  
        
        if (i < 127) {
            physical_page_array[i].next = &physical_page_array[i + 1];
        } else {
            physical_page_array[i].next = NULL;  
        }
        
        if (i > 0) {
            physical_page_array[i].prev = &physical_page_array[i - 1];
        } else {
            physical_page_array[i].prev = NULL; 
        }
    }
    
    free_physical_pages = &physical_page_array[0];
}

struct ppage *allocate_physical_pages(unsigned int npages) {
    if (free_physical_pages == NULL || npages == 0) {
        return NULL;
    }
    
    struct ppage *allocated_list = free_physical_pages;
    
    struct ppage *current = free_physical_pages;
    unsigned int count = 0;
    
    while (current != NULL && count < npages - 1) {
        current = current->next;
        count++;
    }
    
    if (current == NULL || count < npages - 1) {
        return NULL;
    }
    
    struct ppage *new_free_head = current->next;
    
    current->next = NULL;
    
    free_physical_pages = new_free_head;
    
    if (new_free_head != NULL) {
        new_free_head->prev = NULL;
    }
    
    allocated_list->prev = NULL;
    
    return allocated_list;
}

void free_physical_pages_list(struct ppage *ppage_list) {
    if (ppage_list == NULL) {
        return;
    }
    
    struct ppage *current = ppage_list;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = free_physical_pages;
    
    if (free_physical_pages != NULL) {
        free_physical_pages->prev = current;
    }
    
    free_physical_pages = ppage_list;
    free_physical_pages->prev = NULL;
}
void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd) {
 
    uint32_t vaddr_int = (uint32_t)vaddr;
    uint32_t pd_index = vaddr_int >> 22;  
    

    uint32_t pt_index = (vaddr_int >> 12) & 0x3FF; 
    
    if (!pd[pd_index].present) {
        pd[pd_index].present = 1;
        pd[pd_index].rw = 1;          
        pd[pd_index].user = 0;        
        pd[pd_index].frame = ((uint32_t)&pt[0]) >> 12;  // Page table address
    }
    
    struct ppage *current = pglist;
    while (current != NULL) {
        pt[pt_index].present = 1;
        pt[pt_index].rw = 1;           
        pt[pt_index].user = 0;         
        pt[pt_index].frame = ((uint32_t)current->physical_addr) >> 12;
        
        // Move to next page in list and next page table entry
        current = current->next;
        pt_index++;
        
        // Handle page table overflow (shouldn't happen in our simple case)
        if (pt_index >= 1024) {
            break;
        }
    }
    
    return vaddr;
}
