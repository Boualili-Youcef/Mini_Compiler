global _start
_start:
    mov rax, 1
    add rax, 2

    mov rax, 60 ; Le syscall pour quitter
    mov rdi, 0 ; code de retour
    syscall
