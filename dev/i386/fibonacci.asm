fibonacci(int):
        push    ebp
        mov     ebp, esp
        push    ebx
        sub     esp, 4
        cmp     DWORD PTR [ebp+8], 0
        jne     .L2
        mov     eax, 0
        jmp     .L3
.L2:
        cmp     DWORD PTR [ebp+8], 1
        jne     .L4
        mov     eax, 1
        jmp     .L3
.L4:
        mov     eax, DWORD PTR [ebp+8]
        sub     eax, 1
        sub     esp, 12
        push    eax
        call    fibonacci(int)
        add     esp, 16
        mov     ebx, eax
        mov     eax, DWORD PTR [ebp+8]
        sub     eax, 2
        sub     esp, 12
        push    eax
        call    fibonacci(int)
        add     esp, 16
        add     eax, ebx
.L3:
        mov     ebx, DWORD PTR [ebp-4]
        leave
        ret
        