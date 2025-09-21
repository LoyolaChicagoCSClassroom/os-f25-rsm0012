#include "../rprintf.h"  // Make sure this is included

int putc(int c) {
    // Video memory starts at 0xB8000
    volatile unsigned short* vram = (unsigned short*)0xB8000;
    static int cursor_pos = 0;
    
    if (c == '\n') {
        // Move to next line (80 characters per line)
        cursor_pos = (cursor_pos + 80) / 80 * 80;
    } else {
        // Write character with gray color (0x07) on black background
        vram[cursor_pos++] = (0x07 << 8) | c;
    }
    
    // Scroll if reached bottom of screen
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
    
    return c;  // Return the character written
}

void main() {
    // Test scrolling by printing many lines
    for (int i = 0; i < 30; i++) {
        esp_printf(putc, "Line %d: This is test line number %d\n", i, i);
    }
    
    while(1); // Hang so you can see the output
}
