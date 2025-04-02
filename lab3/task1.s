global infile
global outfile

section .data
    ; Constants for file descriptors
    extern strlen
    stdout: equ 1
    newline: db 10  ; Define newline character
    infile dd 0     ; Input file descriptor
    outfile dd 1    ; Output file descriptor
    buffer db 1     ; Buffer for reading/writing

section .text
    ; System call numbers
    CLOSE EQU 6
    OPEN EQU 5
    READ EQU 3
    EXIT EQU 1 
    WRITE EQU 4
    global _start
    
_start:
    ; Start of program
    call main           ; Call function to process arguments
    call encodeInput    ; Call function to encode input file

main:
    ; Function to process command line arguments
    push ebp
    mov ebp,  esp      ; mov argc to ebp
    mov esi, [ebp+16]  ; Get argv
    mov edi, 1         ; Initialize argument index

    ; Loop through all arguments
    start_loop:
        ; Check if we have reached the end of arguments
        cmp edi, [ebp+8] 
        je end_loop

        ; Check the flags
        mov edx, esi       ; Point to current argument
        cmp byte[edx], '-'
        jne argPrint       ; If not a flag, print the argument
        inc edx            ; Move to the next character
        cmp byte[edx], 'o' ; Check for output flag
        je output_check
        cmp byte[edx], 'i' ; Check for input flag
        je input_check

        ; If the flag is not recognized, move to printing the next argument
        jmp argPrint       

        ; Handling output flag
        output_check:
            inc edx
            mov eax, OPEN    ; Open file
            mov ebx, edx    ; Filename
            mov ecx, 0      ; File mode
            mov edx, 0777   ; Permissions
            int 0x80        ; Call kernel
            mov [outfile], eax  ; Save file descriptor
            jmp argPrint

        ; Handling input flag
        input_check:
            inc edx
            mov eax, OPEN    ; Open file
            mov ebx, edx    ; Filename
            mov ecx, 0      ; File mode
            mov edx, 0644   ; Permissions
            int 0x80        ; Call kernel
            mov [infile], eax   ; Save file descriptor
            jmp argPrint


        ; Print the current argument to stdout
        argPrint: 
            mov ebx, stdout    ; File descriptor for stdout
            push esi           ; Push the address of the current argument
            call strlen        ; Call strlen to get argument length
            mov edx, eax       ; Move length to edx
            mov ecx, esi       ; Move argument pointer to ecx
            mov eax, WRITE         ; Syscall number for write
            int 0x80           ; Call kernel to write to stdout

            add esp, 4         ; Restore stack pointer after push

            ; Print newline character
            mov ebx, stdout   ; File descriptor for stdout
            mov ecx, newline  ; Newline character
            mov edx, 1        ; Length of newline character
            mov eax, WRITE        ; Syscall number for write
            int 0x80          ; Call kernel

            ; Move to the next argument
            mov esi, [ebp+ 16 + 4*edi]
            inc edi           ; Increment argument index

            ; Jump back to start of loop
            jmp start_loop

    ; End of loop
    end_loop:
        pop ebp
        ret

encodeInput:
    ; Function to encode the input file
    encoder_loop:
        ; Read input and encode
        mov eax, READ            ; Read syscall
        mov ebx, [infile]       ; Input file descriptor
        mov ecx, buffer         ; Input buffer
        mov edx, 1              ; Read 1 byte
        int 0x80                ; Call kernel
        cmp eax, 0              ; Check for end of file
        je done                 

        ; Encode character
        cmp byte [buffer], 'A'
        jl write
        cmp byte [buffer], 'z'
        jg write
        add byte [buffer], 1

        ; Write encoded character to output
        write:
            mov eax, WRITE
            mov ebx, [outfile]    ; Output file descriptor
            mov ecx, buffer       ; Output buffer
            mov edx, 1            ; Write 1 byte
            int 0x80              ; Call kernel

            jmp encoder_loop

        done:
            ; Close input and output files
            mov eax, CLOSE
            mov ebx, [infile]
            int 0x80

            mov eax, CLOSE
            mov ebx, [outfile]
            int 0x80

            ; Exit program
            mov eax, EXIT
            xor ebx, ebx
            int 0x80