global main
extern printf
extern puts

    section .data
hex: db "%02hhX ", 10, 0
;line2: db "%X", 10, 0
x_struct: dd 5
    x_num: db 0xaa, 1,2,0x44,0x4f

    section .text
main:
    push ebp  ;push the value on ebp to stack
    mov ebp, esp ;move the value on esp to ebp

    push x_struct
    call print_multi
    add esp, 4
	
    leave
    ret

print_multi:
    push ebp  ;push the value on ebp to stack
    mov ebp, esp ;move the value on esp to ebp

    mov esi, [ebp+8]
    mov edi, [esi] ;edi gets arr length
    add esi, 4
    dec esi
    add esi, edi

loop:

    push dword[esi]
    push hex
    call printf
    add esp, 8

    dec esi
    dec edi
    jnz loop

    mov esp, ebp
    pop ebp 
    ret
