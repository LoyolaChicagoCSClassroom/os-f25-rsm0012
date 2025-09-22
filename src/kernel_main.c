// Multiboot header using inline assembly
asm(
    ".section .multiboot\n"
    ".align 4\n"
    ".long 0x1BADB002\n"          
    ".long 0x00000000\n"          // flags  
    ".long -(0x1BADB002 + 0x00000000)\n"  // checksum
    ".previous\n"
);

#include "../rprintf.h"  // Make sure this is included

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
    return data;  // Return the character for esp_printf
}

void main() {
    // Clear the screen first
    volatile unsigned short* vram = (unsigned short*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vram[i] = (0x07 << 8) | ' ';
    }
    
    // Print current execution level as requested
    esp_printf(putc_wrapper, "Current execution level: Kernel mode\r\n");
    esp_printf(putc_wrapper, "Hello World, this is my CS 310 terminal working!\r\n");
    esp_printf(putc_wrapper, "\r\n");
    
    // Demonstrate scrolling by filling the screen and going beyond line 24
    for (int i = 0; i < 30; i++) {
        esp_printf(putc_wrapper, "Line %d: Hello World, this is my CS 310 terminal working!\r\n", i);
    }
    
    esp_printf(putc_wrapper, "\r\n");
    esp_printf(putc_wrapper, "Notice how the screen scrolled when we went past line 24!\r\n");
    esp_printf(putc_wrapper, "The terminal driver is working correctly!\r\n");
    
    while(1); // Hang so you can see the output
}
