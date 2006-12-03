BITS 32
global call_func

call_func:
        push ebp
        mov ebp, esp
        mov eax, 0x12345678
        call eax
        leave
        ret
