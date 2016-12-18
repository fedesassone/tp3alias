; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32


;; PIC
extern fin_intr_pic1
extern atender_int
extern atender_reloj
;; SYSCALL ; estan en sched.c (fue la forma que encontre que me las acepte)
extern llamada
extern atender_int66
extern matar_tarea
;; Scheduler
extern tarea_actual 
;; SCREEN
extern actualizarBufferEstado_UltimoProblema
extern debug_info

extern cargarBufferMapa
extern cargarBufferEstado
extern muestroMapa

extern handler_teclado

;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1
global _isr32
global _isr33
global _isr80
global _isr102

_isr%1:
    mov dword [debug_info + 00], eax        ;eax
    mov eax, INT_%1 
    mov dword [debug_info + 04], eax        ;la direccion de memoria del error
    mov dword [debug_info + 08], INT_len_%1 ;len del error
    mov dword [debug_info + 16], ebx        ;ebx
    mov dword [debug_info + 20], ecx        ;ecx
    mov dword [debug_info + 24], edx        ;edx
    mov dword [debug_info + 28], esi        ;esi
    mov dword [debug_info + 32], edi        ;edi
    mov dword [debug_info + 36], ebp        ;ebp
    mov dword [debug_info + 40], esp        ;esp
   
    mov eax, [esp+12] ; eip
    mov dword [debug_info + 44], eax        ;eip
    mov eax, cr0
    mov dword [debug_info + 48], eax        ;cr0          
    mov eax, cr2
    mov dword [debug_info + 52], eax        ;cr2
    mov eax, cr3
    mov dword [debug_info + 56], eax        ;cr3
    mov eax, cr4
    mov dword [debug_info + 60], eax        ;cr4
    

    xor eax, eax
    mov ax, cs
    mov word [debug_info + 64], ax          ;cs
    mov ax, ds
    mov word [debug_info + 68], ax          ;ds
    mov ax, es
    mov word [debug_info + 72], ax          ;es
    mov ax, fs
    mov word [debug_info + 76], ax          ;fs
    mov ax, gs
    mov word [debug_info + 80], ax          ;gs
    mov ax, ss
    mov word [debug_info + 84], ax          ;ss
    
    xor eax, eax
    pushf                                   ; obtenemos el registro
    pop ax                                  ; eflags
    mov dword [debug_info + 88], eax

    call tarea_actual
    mov dword [debug_info + 12], eax        ;id tarea actual 

    call actualizarBufferEstado_UltimoProblema

    call matar_tarea ; mata a la tarea que provoco la interrupcion y a su bandera o viceversa
                     ; y devuelve en ax el selector de idle
    

    mov [tss_selector], ax
    sti
    jmp far [tss_offset] ;paso a la proxima tarea o bandera segun corresponda

%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler
reloj_numero:           dd 0x00000000
reloj:                  db '|/-\'

; Tareas
tss_offset:   dd 0x0 
tss_selector: dw 0x0



;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14

ISR 16
ISR 17
ISR 18
ISR 19

;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
_isr32:
  pushad
  pushfd

  call fin_intr_pic1
  
  cmp byte [muestroMapa],0x1
  je .muestroMapa
  .muestroEst:
  call cargarBufferEstado
  jmp .sigo
  .muestroMapa:
  call cargarBufferMapa
  .sigo:
  call proximo_reloj
  ;schedulizar
  ;xchg bx,bx
  call atender_reloj ;call atender reloj 
  ;esto me devuelve un selector tss
  mov [tss_selector], ax
  ;xchg bx,bx
  jmp far [tss_offset] ; volvi de anteder reloj salto tareasig


  ; listo?
  popfd
  popad

  iret
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
_isr33:
  pushad
  pushfd

  call fin_intr_pic1
  xor eax, eax
  in al, 0x60
  push eax
  call handler_teclado
  add esp,4
  popfd
  popad
  
  iret
;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
_isr80:
        pushad
        pushfd

         push ecx
         push ebx
         push eax

        ;xchg bx,bx
        call llamada

        add esp,12
        
        cmp byte [muestroMapa],0x1
        je .muestroMapa
        .muestroEst:
        call cargarBufferEstado
        jmp .sigo
        .muestroMapa:
        call cargarBufferMapa
        .sigo:

        xor eax, eax
        mov ax, 0xc0
        mov [tss_selector], ax
        ;xchg bx,bx
        jmp far [tss_offset] ;saltamos a la idle por el resto del quantum
        

        popfd
        popad
        
        iret

  _isr102:
  pushad
  pushfd
  ;xchg bx,bx
  call atender_int66 ; me fijo si la llamó una tarea. Si es así, borro la tarea y a su bandera.Ademas pongo corriendoBandera en 0
  mov [tss_selector], ax
  ;xchg bx,bx
  jmp far [tss_offset]
  ;salto a idle

  popfd
  popad
  
  iret
;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
proximo_reloj:
    pushad

    inc DWORD [reloj_numero]
    mov ebx, [reloj]
    cmp ebx, 0x4
    jl .ok
        mov DWORD [reloj_numero], 0x0
        mov ebx, 0
    .ok:
        add ebx, reloj
        imprimir_texto_mp ebx, 1, 0x0f, 24, 79
        ;imprimir_texto_mp INT_1, INT_len_1, 0x07, 5, 0
    popad
    ret

INT_0: db 'Divide Error', 0
INT_len_0 equ    $ - INT_0
INT_1: db 'RESERVED', 0
INT_len_1 equ    $ - INT_1
INT_2: db 'NMI Interrupt', 0
INT_len_2 equ    $ - INT_2
INT_3: db 'BreakPoint', 0
INT_len_3 equ    $ - INT_3
INT_4: db 'Overflow', 0
INT_len_4 equ    $ - INT_4
INT_5: db 'BOUND Range Exceeded', 0
INT_len_5 equ    $ - INT_5
INT_6: db 'Invalid Opcode' , 0
INT_len_6 equ    $ - INT_6
INT_7: db 'Device Not Available', 0
INT_len_7 equ    $ - INT_7
INT_8: db 'Doble Fault', 0
INT_len_8 equ    $ - INT_8
INT_9: db 'Coprocessor Segment Overrun (reserved)', 0
INT_len_9 equ    $ - INT_9
INT_10: db 'Invalid TSS', 0
INT_len_10 equ    $ - INT_10
INT_11: db 'Segment Not Present', 0
INT_len_11 equ    $ - INT_11
INT_12: db 'Stack-Segment Fault', 0
INT_len_12 equ    $ - INT_12
INT_13: db 'General Protection', 0
INT_len_13 equ    $ - INT_13
INT_14: db 'Page Fault', 0
INT_len_14 equ    $ - INT_14
INT_15: db 'Intel Reserved.', 0
INT_len_15 equ    $ - INT_15
INT_16: db 'X-87 FPU Floating-Point Error', 0
INT_len_16 equ    $ - INT_16
INT_17: db 'Alignment Check Fault', 0
INT_len_17 equ    $ - INT_17
INT_18: db 'Machine Check ', 0
INT_len_18 equ    $ - INT_18
INT_19: db 'SIMD Floating Point', 0
INT_len_19 equ    $ - INT_19