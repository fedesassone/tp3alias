/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "i386.h"

typedef struct str_pde_entry {
    unsigned char   present:1;
    unsigned char   rw:1;
    unsigned char   us:1;
    unsigned char   pwt:1;
    unsigned char   pcd:1;
    unsigned char   a:1;
    unsigned char   d:1;
    unsigned char   pat:1;
    unsigned char   g:1;
    unsigned short  disponible:3;
    unsigned int    base:20;
} __attribute__((__packed__)) pde_entry;


typedef struct str_pte_entry {
    unsigned char   present:1;
    unsigned char   rw:1;
    unsigned char   us:1;
    unsigned char   pwt:1;
    unsigned char   pcd:1;
    unsigned char   a:1;
    unsigned char   d:1;
    unsigned char   pat:1;
    unsigned char   g:1;
    unsigned short  disponible:3;
    unsigned int    base:20;
} __attribute__((__packed__)) pte_entry;


void mmu_inicializar();
void mmu_inicializar_dir_kernel();
int* mmu_inicializar_dir_tarea();
void copiarCodigo(unsigned int src, unsigned int dst);

unsigned int mmu_proxima_pagina_fisica_libre();
void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, unsigned char us, unsigned char rw);
void mmu_desmapear_pagina(unsigned int virtual, unsigned int cr3);

extern pde_entry pde[];
extern pte_entry pte[];


#endif	/* !__MMU_H__ */
