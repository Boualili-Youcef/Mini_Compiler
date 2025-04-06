global _start
section .text
_start:
    push rbp
    mov rbp, rsp
    mov rax, 10
    sub rsp, 8
    mov [rbp-8], rax
    mov rax, 5
    sub rsp, 8
    mov [rbp-16], rax
    ; Début du if
    mov rax, [rbp-16]
    push rax
    mov rax, 5
    pop rbx
    add rax, rbx
    push rax
    mov rax, [rbp-8]
    pop rbx
    cmp rax, rbx
    sete al
    movzx rax, al
    cmp rax, 0
    je .if_end_0
    ; Début de bloc
    mov rax, 52
    sub rsp, 8
    mov [rbp-24], rax
    ; Début du if
    mov rax, 55
    push rax
    mov rax, [rbp-24]
    pop rbx
    cmp rax, rbx
    sete al
    movzx rax, al
    cmp rax, 0
    je .if_end_1
    ; Début de bloc
    mov rax, 52
    mov rdi, rax
    mov rax, 60
    syscall
    ; Fin de bloc
.if_end_1:
    ; Fin du if
    mov rax, 69
    mov rdi, rax
    mov rax, 60
    syscall
    add rsp, 8
    ; Fin de bloc
.if_end_0:
    ; Fin du if
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 60
    syscall
