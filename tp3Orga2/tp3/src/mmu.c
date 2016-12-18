/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "screen.h"
 #include "defines.h"

unsigned int proxima_pagina_libre;

void mmu_inicializar() {
	proxima_pagina_libre 		= 0x30000;
}

void mmu_inicializar_dir_kernel(){
	int* page_directory = (int*) 0x27000;  // PAGE_DIRECTORY_KERNEL = 0x27000
	page_directory[0] = 0x28000 + 0x3;		 // PAGE_TABLE_KERNEL_1 = 0x28000, seteo p=1 y r/w=1
	page_directory[1] = 0x29000 + 0x3;       // PAGE_TABLE_KERNEL_2 = 0x30000
	int i;
	for (i = 2; i < 1024; ++i) { //pongo todo el resto de las posiciones en cero.
		page_directory[i]= 0x0;
	}
	int* page_table_1 = (int*) 0x28000;
	for (i = 0; i < 1024; ++i) {
		page_table_1[i] = ((i << 12) | 3);
	}
	int* page_table_2 = (int*) 0x29000;
	for (i = 0; i < 1024; ++i) {
		if(i<895){
			page_table_2[i] = (((i+1024) << 12) | 3);	
		}else{
			page_table_2[i] = 0x0;
		}
	}
}

int* mmu_inicializar_dir_tarea( unsigned int id_tarea){
	//paginasNavios.idtarea = id_tarea;
	int* page_directory = (int*) mmu_proxima_pagina_fisica_libre();  // PAGE_DIRECTORY_KERNEL = 0x27000
	int i;
	for (i = 2; i < 1024; ++i) { //pongo todo el resto de las posiciones en cero.
		page_directory[i]= 0x0;
	}
	int* page_table_1 = (int*) mmu_proxima_pagina_fisica_libre();
	
	for (i = 0; i < 1024; ++i) {
		page_table_1[i] = ((i << 12) | 7); //101
	}
	//paginasNavios.p1 = (unsigned int) page_table_1;

	int* page_table_2 = (int*) mmu_proxima_pagina_fisica_libre();
	
	for (i = 0; i < 1024; ++i) {
		if(i<896){
			page_table_2[i] = (((i+1024) << 12) | 7);// es 5 porque los atributos son 1 user 0 read/w 1 present
		}else{
			page_table_2[i] = 0x0;
		}
	}
	//mapeamos
	page_directory[0] = (int)page_table_1 + 0x7;
	page_directory[1] = (int)page_table_2 + 0x7;
	mmu_mapear_pagina(DIR_VIRTUAL_TAREA,(unsigned int) page_directory,0x10000 + id_tarea,1,1);//mappeamos la primera pagina
	mmu_mapear_pagina(DIR_VIRTUAL_TAREA + 0x1000,(unsigned int) page_directory,0x10000 + id_tarea + 0x1000,1,1);
	mmu_mapear_pagina(TASK_ANCLA,(unsigned int) page_directory,TASK_ANCLA_FIS,1,0);// el ancla es de solo lectura
	

	//copiamos codigo

	copiarCodigo( 0x10000 + id_tarea         ,  AREA_MAR_INICIO + id_tarea); 		 //pagina uno
	copiarCodigo( 0x10000 + id_tarea + 0x1000,  AREA_MAR_INICIO + id_tarea +0x1000); //pagina dos


	return page_directory;
}

void copiarCodigo(unsigned int src, unsigned int dest)
{
    unsigned int i;
    for(i = 0; i<4096; i++)
    {
        *((char *) (dest + i)) =  *((char *) (src + i));
    }
}


void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, 
	unsigned int fisica, unsigned char us, unsigned char rw){
	pde_entry* CR3 = (pde_entry*)(cr3 & 0xFFFFF000); 
	pde_entry* PDE = &(CR3[virtual>>22]);    
	if (!(PDE->present)){
		PDE->base = mmu_proxima_pagina_fisica_libre() >> 12;
		PDE->present = 1;
		PDE->rw = rw;
		PDE->us = us;
		pte_entry* pte_indice = (pte_entry*) ((PDE->base) << 12);
		int i;
		for (i = 0; i < 1024; i++){
			pte_indice[i].present = 0;
			pte_indice[i].us = us;
		}
	}
	pte_entry* PTable = (pte_entry*) ((PDE->base) << 12);
	pte_entry* PTE = (pte_entry*) &(PTable[(virtual>>12) & 0x3ff]);
	PTE->base = fisica >> 12;
	PTE->present = 1;
	PTE->rw = rw;
	PTE->us = us;
	tlbflush();
}
void mmu_desmapear_pagina(unsigned int virtual, unsigned int cr3){
	pde_entry* CR3 = (pde_entry*)(cr3 & 0xFFFFF000); 
	pde_entry* PDE = &(CR3[virtual>>22]);    
	if((PDE->present == 1)){
		pte_entry* PTable = (pte_entry*) ((PDE->base) << 12);
		pte_entry* PTE = (pte_entry*) &(PTable[(virtual>>12) & 0x3ff]);
		pte_entry* pte_indice = (pte_entry*) ((PDE->base)<<12);
		PTE->present = 0;
		PTE->us = 0;
		int QuedaVacia = 1;
		int i;
		for ( i = 0; i < 1024 && QuedaVacia; i++)
		{
			if(pte_indice[i].present == 1){
				QuedaVacia = 0;
			}
		}
		if (QuedaVacia)
		{
			PDE->present = 0;
		}
		tlbflush();
		}

}

unsigned int mmu_proxima_pagina_fisica_libre(){
	unsigned int pagina = proxima_pagina_libre;
	proxima_pagina_libre = proxima_pagina_libre + 0x1000;
	return pagina;
}

