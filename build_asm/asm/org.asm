global _start
section .text
_start:
    push rbp
    mov rbp, rsp
    mov rax, 2
    push rax
    mov rax, 3
    push rax
    mov rax, 4
    pop rbx
    mov rcx, rbx
    xor rdx, rdx
    div rcx
    mov rax, rdx
    pop rbx
    imul rax, rbx
    push rax
    mov rax, 10
    pop rbx
    add rax, rbx
    sub rsp, 8
    mov [rbp-8], rax
    mov rax, 69
    sub rsp, 8
    mov [rbp-16], rax
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 60
    syscall
