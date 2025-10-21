.text
.global _start
_start:
    adrp x1, L_str_1
    add  x1, x1, :lo12:L_str_1
    adrp x2, L_str_1_len
    add  x2, x2, :lo12:L_str_1_len
    ldr  x2, [x2]
    mov  x0, #1
    mov  x8, #64
    svc  #0
    adrp x1, L_str_2
    add  x1, x1, :lo12:L_str_2
    adrp x2, L_str_2_len
    add  x2, x2, :lo12:L_str_2_len
    ldr  x2, [x2]
    mov  x0, #1
    mov  x8, #64
    svc  #0
    adrp x0, L_str_3
    add  x0, x0, :lo12:L_str_3
    adrp x9, a
    add  x9, x9, :lo12:a
    str  x0, [x9]
    adrp x1, L_str_4
    add  x1, x1, :lo12:L_str_4
    adrp x2, L_str_4_len
    add  x2, x2, :lo12:L_str_4_len
    ldr  x2, [x2]
    mov  x0, #1
    mov  x8, #64
    svc  #0
    adrp x1, L_str_5
    add  x1, x1, :lo12:L_str_5
    adrp x2, L_str_5_len
    add  x2, x2, :lo12:L_str_5_len
    ldr  x2, [x2]
    mov  x0, #1
    mov  x8, #64
    svc  #0
    mov x0, #0
    mov x8, #93
    svc #0

.section .data
a:
    .quad L_str_0

.section .rodata
L_str_0: .ascii "hola"
L_str_0_len: .quad 4
L_str_1: .ascii "hola"
L_str_1_len: .quad 4
L_str_2: .ascii "\n"
L_str_2_len: .quad 1
L_str_3: .ascii "a"
L_str_3_len: .quad 1
L_str_4: .ascii "a"
L_str_4_len: .quad 1
L_str_5: .ascii "\n"
L_str_5_len: .quad 1
