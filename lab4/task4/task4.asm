global main
extern printf   
section .data

MASK: dw 0x1582
STATE: dw 0x1839
line: db "%04hX", 10, 0

section .text
main:

    push ebp  ;push the value on ebp to stack    
    mov ebp, esp ;move the value on esp to ebp    
   
    call rand_min	    
    leave 
    ret

rand_min:

    push ebp  ;push the value on ebp to stack    
    mov ebp, esp ;move the value on esp to ebp 

    mov esi,20 ;esi=20


loop: 
    cmp esi,0
    je .return

    mov edx, [STATE]
    xor edx,[MASK]
    mov word[STATE], dx

    ;checking how many 1 there is in STATE
    jp .STATE_even

    stc 
    
    .STATE_even:

    mov ebx, 0
    adc ebx, 0 

    push edx
    push line
    call printf
    add esp, 8
    
    shr word[STATE],1
    shl ebx,15
    add word[STATE], bx

    dec esi
    
    jmp loop
   

.return:
 mov esp, ebp
 pop ebp 
 ret
