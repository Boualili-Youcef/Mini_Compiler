global _start
section .text
_start:
    push rbp
    mov rbp, rsp
    mov rax, 5
    sub rsp, 8
    mov [rbp-8], rax
    mov rax, 60
    mov rdi, [rbp-8]
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
