GLOBAL  _read_msw,_lidt
GLOBAL  _int_08_hand,_int09Handler
GLOBAL  _mascaraPIC1,_mascaraPIC2,_Cli,_Sti
GLOBAL  _debug

EXTERN  int_08, int09


SECTION .text


_Cli:
	cli			; limpia flag de interrupciones
	ret

_Sti:

	sti			; habilita interrupciones por flag
	ret


_read_msw:
        smsw    ax		; Obtiene la Machine Status Word
        retn


_lidt:				; Carga el IDTR
        push    ebp
        mov     ebp, esp
        push    ebx
        mov     ebx, [ss: ebp + 6] ; ds:bx = puntero a IDTR 
	rol	ebx,16		    	
	lidt    [ds: ebx]          ; carga IDTR
        pop     ebx
        pop     ebp
        retn

_eoi:
    mov al, 20h
    out 20h, al

    ret

_int_08_hand:				; Handler de INT 8 ( Timer tick)
    push    ds
    push    es                      ; Se salvan los registros
    pusha                           ; Carga de DS y ES con el valor del selector
    mov     ax, 10h			; a utilizar.
    mov     ds, ax
    mov     es, ax                  
    call    int_08                 

    popa                            
    pop     es
    pop     ds

    call _eoi

    iret

_int09Handler:
    ; Save the current execution context
    push ds
    push es
    pusha

    ; Set up the handler execution context
    mov ax, 10h
    mov ds, ax
    mov es, ax

    ; Call the handler
    call int09

    ; Set the context back 
    popa
    pop es
    pop ds

    ; Tell the PIC we're done and exit
    call _eoi

    iret

; Debug para el BOCHS, detiene la ejecució; Para continuar colocar en el BOCHSDBG: set $eax=0
;


_debug:
        push    bp
        mov     bp, sp
        push	ax
vuelve:	mov     ax, 1
        cmp	ax, 0
	jne	vuelve
	pop	ax
	pop     bp
        retn