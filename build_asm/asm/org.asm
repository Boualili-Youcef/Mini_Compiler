global _start
section .text
_start:
    push rbp
    mov rbp, rsp
    mov rax, 10
    sub rsp, 8
    mov [rbp-8], rax
    ; Début du if
    mov rax, 25
    push rax
    mov rax, 25
    pop rbx
    cmp rax, rbx
    setne al
    movzx rax, al
    push rax
    mov rax, 3
    push rax
    mov rax, 2
    push rax
    mov rax, 1
    pop rbx
    add rax, rbx
    pop rbx
    cmp rax, rbx
    setg al
    movzx rax, al
    push rax
    mov rax, 2
    push rax
    mov rax, [rbp-8]
    pop rbx
    cmp rax, rbx
    setg al
    movzx rax, al
    pop rbx
    or rax, rbx
    pop rbx
    and rax, rbx
    cmp rax, 0
    je .if_else_0
    ; Début de bloc
    mov rax, 69
    mov rdi, rax
    mov rax, 60
    syscall
    ; Fin de bloc
    jmp .if_else_0
.if_else_0:
    ; Début de bloc
    ; Début du if
    mov rax, 1
    push rax
    mov rax, 1
    pop rbx
    cmp rax, rbx
    setg al
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
    ; Fin de bloc
.if_end_0:
    ; Fin du if
    mov rax, [rbp-8]
    mov rdi, rax
    mov rax, 60
    syscall
