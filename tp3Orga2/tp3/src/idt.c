/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "idt.h"
  #include "colors.h"
  #include "isr.h"
#include "screen.h"
idt_entry idt[255] = { };

idt_descriptor IDT_DESC = {
    sizeof(idt) - 1,
    (unsigned int) &idt
};



#define IDT_ENTRY(numero)                                                                                        \
    idt[numero].offset_0_15 = (unsigned short) ((unsigned int)(&_isr ## numero) & (unsigned int) 0xFFFF);        \
    idt[numero].segsel = (unsigned short) 0x00a8;                                                                  \
    idt[numero].attr = (unsigned short) 0x8e00;                                                                  \
    idt[numero].offset_16_31 = (unsigned short) ((unsigned int)(&_isr ## numero) >> 16 & (unsigned int) 0xFFFF);


void idt_inicializar() {
        IDT_ENTRY(0);
        IDT_ENTRY(1);
        IDT_ENTRY(2);
        IDT_ENTRY(3);
        IDT_ENTRY(4);
        IDT_ENTRY(5);
        IDT_ENTRY(6);
        IDT_ENTRY(7);
        IDT_ENTRY(8);
        IDT_ENTRY(9);
        IDT_ENTRY(10);
        IDT_ENTRY(11);
        IDT_ENTRY(12);
        IDT_ENTRY(13);
        IDT_ENTRY(14);
        //IDT_ENTRY(15);
        IDT_ENTRY(16);
        IDT_ENTRY(17);
        IDT_ENTRY(18);
        IDT_ENTRY(19);
        IDT_ENTRY(32);//RELOJ
        IDT_ENTRY(33);//TECLADO
        idt[80].offset_0_15 = (unsigned short) ((unsigned int)(&_isr80) & (unsigned int) 0xFFFF);        
        idt[80].segsel = (unsigned short) 0x00a8;                                                                  
        idt[80].attr = (unsigned short) 0xee00;// LE PONGO DPL 3 YA QUE VA A SER USADA POR UN USUARIO                                                                  
        idt[80].offset_16_31 = (unsigned short) ((unsigned int)(&_isr80) >> 16 & (unsigned int) 0xFFFF);
        idt[102].offset_0_15 = (unsigned short) ((unsigned int)(&_isr102) & (unsigned int) 0xFFFF);        
        idt[102].segsel = (unsigned short) 0x00a8;                                                                  
        idt[102].attr = (unsigned short) 0xee00;// LE PONGO DPL 3 YA QUE VA A SER USADA POR UN USUARIO                                                                  
        idt[102].offset_16_31 = (unsigned short) ((unsigned int)(&_isr102) >> 16 & (unsigned int) 0xFFFF);

    }


