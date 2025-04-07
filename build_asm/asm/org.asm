global _start
section .text
_start:
    push rbp
    mov rbp, rsp
    mov rax, 9
    mov rdi, 0
    mov rsi, 88
    mov rdx, 3
    mov r10, 34
    mov r8, -1
    mov r9, 0
    syscall
    push rax
    mov rbx, [rsp]
    mov qword [rbx], 10
    mov rbx, [rsp]
    push rbx
    mov rax, 1
    pop rbx
    mov [rbx + 8], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 2
    pop rbx
    mov [rbx + 16], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 3
    pop rbx
    mov [rbx + 24], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 4
    pop rbx
    mov [rbx + 32], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 5
    pop rbx
    mov [rbx + 40], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 6
    pop rbx
    mov [rbx + 48], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 7
    pop rbx
    mov [rbx + 56], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 8
    pop rbx
    mov [rbx + 64], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 9
    pop rbx
    mov [rbx + 72], rax
    mov rbx, [rsp]
    push rbx
    mov rax, 10
    pop rbx
    mov [rbx + 80], rax
    pop rax
    sub rsp, 8
    mov [rbp-8], rax
    mov rax, 0
    sub rsp, 8
    mov [rbp-16], rax
    mov rax, 1
    push rax
    mov rax, [rbp-8]
    mov rax, [rax]
    pop rbx
    sub rax, rbx
    sub rsp, 8
    mov [rbp-24], rax
.while_start_0:
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-16]
    pop rbx
    cmp rax, rbx
    setl al
    movzx rax, al
    cmp rax, 0
    je .while_end_0
    ; Début de bloc
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    add rax, 1
    imul rax, 8
    pop rbx
    add rbx, rax
    mov rax, [rbx]
    sub rsp, 8
    mov [rbp-32], rax
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    add rax, 1
    imul rax, 8
    pop rbx
    add rbx, rax
    mov rax, [rbx]
    push rax
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    add rax, 1
    imul rax, 8
    pop rbx
    add rbx, rax
    pop rax
    mov [rbx], rax
    mov rax, [rbp-32]
    push rax
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-24]
    add rax, 1
    imul rax, 8
    pop rbx
    add rbx, rax
    pop rax
    mov [rbx], rax
    mov rax, 1
    push rax
    mov rax, [rbp-16]
    pop rbx
    add rax, rbx
    mov [rbp-16], rax
    mov rax, 1
    push rax
    mov rax, [rbp-24]
    pop rbx
    sub rax, rbx
    mov [rbp-24], rax
    add rsp, 8
    ; Fin de bloc
    jmp .while_start_0
.while_end_0:
    mov rax, 0
    sub rsp, 8
    mov [rbp-32], rax
.while_start_1:
    mov rax, [rbp-8]
    mov rax, [rax]
    push rax
    mov rax, [rbp-32]
    pop rbx
    cmp rax, rbx
    setl al
    movzx rax, al
    cmp rax, 0
    je .while_end_1
    ; Début de bloc
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-32]
    add rax, 1
    imul rax, 8
    pop rbx
    add rbx, rax
    mov rax, [rbx]
    ; Convertir et afficher l'entier
    sub rsp, 32
    mov rcx, rsp
    add rcx, 31
    mov byte [rcx], 0x0A
    dec rcx
    mov r10, rax
    test r10, r10
    jns .print_positive_0
    neg r10
    mov byte [rcx], 0x2D
    dec rcx
.print_positive_0:
    mov rax, r10
    mov r9, 10
.convert_loop_0:
    xor rdx, rdx
    div r9
    add dl, '0'
    mov [rcx], dl
    dec rcx
    test rax, rax
    jnz .convert_loop_0
    lea rsi, [rcx+1]
    mov rdx, rsp
    add rdx, 31
    sub rdx, rcx
    mov rax, 1
    mov rdi, 1
    syscall
    add rsp, 32
    mov rax, 1
    push rax
    mov rax, [rbp-32]
    pop rbx
    add rax, rbx
    mov [rbp-32], rax
    ; Fin de bloc
    jmp .while_start_1
.while_end_1:
    mov rax, 60
    mov rdi, 0
    syscall
