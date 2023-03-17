global main
extern printf
extern puts
extern malloc
extern free

    section .data
x_struct: dd 6
    x_val: db 1,0xf0,1,2,0x44,0x4f
y_struct: dd 5
    y_val: db 1,1,2,0x44,1
line: db "%X", 10, 0
hex: db "%02hhX ", 10, 0

x_length: dd 0
y_length: dd 0 

    section .text
main:
    push ebp  ;push the value on ebp to stack
    mov ebp, esp ;move the value on esp to ebp

    push y_struct
    push x_struct
    call maxMin
    add esp, 8

    push ebx
    push eax
    call add_multi
    add esp, 8


    mov edi, eax
    push edi
    call print_multi
    add esp, 4

    push edi
    call free
    add esp, 4

    leave
    ret

add_multi:
    push ebp  ;push the value on ebp to stack
    mov ebp, esp ;move the value on esp to ebp
    mov esi, [ebp+8] ;x_length
    mov edi, [ebp+12];y_length

    mov ebx, [esi]
    add ebx, 4
    push ebx
    call malloc ;after this line eax gets a pointer

    mov dword[eax], ebx  ;writing res array length
    add dword[eax], -4

    mov ebx, [esi]
    mov dword[x_length], ebx ;x_length gets x length

    mov ebx, [edi]
    mov dword[y_length], ebx ;y_length gets y length

    mov esi, [ebp+8] ;x[0]
    add esi, 4

    mov edi, [ebp+12] ;y[0]
    add edi, 4
 
    mov ebx, eax  ;z[0]
    add ebx, 4   
    clc

.loop:
 
    inc dword[x_length]
    dec dword[x_length]
    jz .return

    inc dword[y_length]
    dec dword[y_length]
    jz .y_empty

    mov edx, [esi] ;get x[i]
    mov ecx, [edi] ;get y[i]
    adc dl, cl     ;get x[i]+y[i]+carry

    mov byte[ebx], dl  ;write the result to z[i]

    inc edi ;moving y
    dec dword[y_length]

    jmp .after_if

.y_empty:

    mov edx, [esi]     ;get x[i]
    adc dl, 0          ;get x[i]+carry

    mov byte[ebx], dl  ;write the result to z[i] 

.after_if:

    inc ebx ;moving z
    inc esi ;moving x
    dec dword[x_length]
    jmp .loop

.return:
      
    mov esp, ebp
    pop ebp 
    ret


maxMin:
    push ebp  ;push the value on ebp to stack
    mov ebp, esp ;move the value on esp to ebp
    mov esi, [ebp+8] ;x_length
    mov edi, [ebp+12];y_length

    mov eax, [esi]
    mov ebx, [edi]

    cmp eax, ebx
    jg .x_bigger
    
    mov eax, edi ;y_bigger
    mov ebx, esi  

    jmp .return  

.x_bigger:

    mov eax, esi
    mov ebx, edi 

.return:

    mov esp, ebp
    pop ebp 
    ret


print_multi:
    push ebp  ;push the value on ebp to stack
    mov ebp, esp ;move the value on esp to ebp

    mov esi, [ebp+8]
    mov edi, [esi] ;edi gets arr length
    add esi, 4

loop:

    push dword[esi]
    push hex
    call printf
    add esp, 8

    inc esi
    dec edi
    jnz loop

    mov esp, ebp
    pop ebp 
    ret
