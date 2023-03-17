  global main
  extern printf
  extern puts

  section .data
line: db "argc:%d", 10, 0

  section .text
main:
  push ebp  ;push the value on ebp to stack
  mov ebp, esp ;move the value on esp to ebp

  mov eax, dword[ebp + 8]	;printing argc
  push eax
  push line
  call printf
  add esp, 8
	
	
loop:

  mov eax, dword[ebp + 12]	
  push dword[eax]
  call puts
  add esp, 4

  add dword[ebp + 12], 4
  add dword[ebp + 8], -1
    
  jnz loop
  
  xor eax, eax
  leave
  ret
