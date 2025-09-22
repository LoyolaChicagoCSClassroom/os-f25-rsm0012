# Multiboot constants
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

# Multiboot header (must be in first 8KB of kernel)
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Stack
.section .bss
.align 16
stack_bottom:
.skip 16384  # 16 KiB stack
stack_top:

# Entry point
.section .text
.global _start
.type _start, @function

_start:
    # Set up stack
    mov $stack_top, %esp
    
    # Call C main function
    call main
    
    # Hang if main returns
    cli
1:  hlt
    jmp 1b

.size _start, . - _start
