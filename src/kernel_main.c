// Multiboot header using inline assembly
#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6
const unsigned int multiboot_header[] __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

#include "../rprintf.h"
#include "../interrupt.h"  // Add this for interrupt functions

int putc(int data) {
    // Video memory starts at 0xB8000
    volatile unsigned short* vram = (unsigned short*)0xB8000;
    static int cursor_pos = 0;
    
    if (data == '\n') {
        // Move to next line (80 characters per line)
        cursor_pos = ((cursor_pos / 80) + 1) * 80;
    } else if (data == '\r') {
        // Carriage return - move to beginning of current line
        cursor_pos = (cursor_pos / 80) * 80;
    } else {
        // Write character with gray color (0x07) on black background
        vram[cursor_pos] = (0x07 << 8) | (unsigned char)data;
        cursor_pos++;
    }
    
    // Scroll if reached bottom of screen (25 rows total, 0-24)
    if (cursor_pos >= 80 * 25) {
        // Scroll entire screen up by one line
        for (int i = 0; i < 80 * 24; i++) {
            vram[i] = vram[i + 80];
        }
        // Clear the bottom line
        for (int i = 80 * 24; i < 80 * 25; i++) {
            vram[i] = (0x07 << 8) | ' ';
        }
        // Move cursor to start of last line
        cursor_pos = 80 * 24;
    }
    
    return data;
}

// Wrapper function for esp_printf compatibility
int putc_wrapper(int data) {
    putc(data);
    return data;
}

void main() {
    // Clear the screen first
    volatile unsigned short* vram = (unsigned short*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vram[i] = (0x07 << 8) | ' ';
    }
    
    // Initialize interrupt system for keyboard input
    remap_pic();  // Set up the programmable interrupt controller
    load_gdt();   // Load the global descriptor table
    init_idt();   // Initialize the interrupt descriptor table
    asm("sti");   // Enable interrupts
    
    // Print welcome message
    esp_printf(putc_wrapper, "CS310 Homework 2: Keyboard Interrupts\r\n");
    esp_printf(putc_wrapper, "Interrupts enabled. Type to test keyboard input:\r\n");
    esp_printf(putc_wrapper, "\r\n");
    
    // Infinite loop - wait for keyboard interrupts
    while(1) {
        asm("hlt");  // Halt until next interrupt
    }
}
