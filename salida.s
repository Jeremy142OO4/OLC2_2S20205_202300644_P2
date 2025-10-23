    .section .text
    .global _start
    .p2align 2
_start:
    .section .data
a__g0:
    .p2align 2
    .word 15
    .section .text
    mov w2, #0
    .section .text
    sub     sp, sp, #64
    mov     w3, #0
    cbnz    x2, 1f
    mov     x9, sp
    mov     w4, #'0'
    strb    w4, [x9]
    mov     x8, #64
    mov     x0, #1
    mov     x1, x9
    mov     x2, #1
    svc     #0
    add     sp, sp, #64
    b       5f
1:
    cmp     x2, #0
    bge     2f
    neg     x2, x2
    mov     w3, #1
2:
    add     x10, sp, #64
    mov     x11, x10
    mov     x5, #10
3:
    udiv    x7, x2, x5
    mul     x12, x7, x5
    sub     x8, x2, x12
    add     w8, w8, #'0'
    sub     x11, x11, #1
    strb    w8, [x11]
    mov     x2, x7
    cbnz    x2, 3b
    cbz     w3, 4f
    sub     x11, x11, #1
    mov     w8, #'-'
    strb    w8, [x11]
4:
    sub     x2, x10, x11
    mov     x8, #64
    mov     x0, #1
    mov     x1, x11
    svc     #0
    add     sp, sp, #64
5:
    .section .rodata
str_0:        .ascii "\n"
    .section .text
    adrp    x0, str_0
    add     x0, x0, :lo12:str_0
    mov     x8, #64
    mov     x1, x0
    mov     x2, #1
    mov     x0, #1
    svc     #0
    .section .text
    mov     x0, #0
    mov     x8, #93
    svc     #0
