; Kernel entry point
[BITS 32]
[EXTERN kernel_main]    ; C function we'll call

global _start

_start:
    ; Call the kernel main function
    call kernel_main

    ; If kernel_main returns, hang
    jmp $
