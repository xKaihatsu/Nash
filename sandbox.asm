global _start
_start:
	mov rbp, rsp
	mov rdi, rsi

	mov rdi, [rbp]
	lea rsi, [rbp + 8]
	push rdi
	push rsi
	call main
	mov rdi, rax
	mov rax, 60
	syscall
printInteger:
	push rbp
	mov rbp, rsp
	mov rdi, [rbp + 16]
	test rdi, rdi
	jge .L1
	push rdi
	push '-'
	call printCharacter
	pop rdi
	neg rdi
.L1:
	mov rax, rdi
	mov rsi, 10
	cqo
	idiv rsi
	mov rdi, rdx
	test rax, rax
	jz .L2
	push rdi
	push rax
	call printInteger
	pop rdi
.L2:
	add rdi, '0'
	push rdi
	call printCharacter
	pop rbp
	ret 8
main:
	push rbp
	mov rbp, rsp
	push 42
	call printInteger
	mov rsp, rbp
	pop rbp
	ret
