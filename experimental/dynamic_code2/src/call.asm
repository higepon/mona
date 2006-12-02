BITS 32
global call_func

call_func:
        push ebp
        mov ebp, esp
        sub esp, 8
        mov eax, dword [ebp + 8]
        call eax
        leave
        ret
