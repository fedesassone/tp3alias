/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "gdt.h"
#include "mmu.h"  


tss tarea_inicial;
tss tarea_idle;

tss tss_navios[CANT_TAREAS];
tss tss_banderas[CANT_TAREAS];

void tss_inicializar() {

		// IDLE
	tarea_idle.esp 		= 0x0002A000;
	tarea_idle.ebp 		= 0x0002A000;
	tarea_idle.eip  	= 0x40000000;
	tarea_idle.cr3 		= 0x27000;//KERNEL PAGE DIRECTORY
	tarea_idle.es 		= 0x98;//seg datos 0
	tarea_idle.cs 		= 0xa8;//SEG Codigo 0
	tarea_idle.ss 		= 0x98;
	tarea_idle.ds 		= 0x98;
	tarea_idle.fs 		= 0x98;
	tarea_idle.gs 		= 0x98;
	tarea_idle.eflags 	= 0x00000202;
	tarea_idle.iomap 	= 0xFFFF;
	tarea_idle.esp0     = 0;
	tarea_idle.ss0      = 0;

	gdt[GDT_TAREA_IDLE].base_0_15  = 
	((unsigned int) (&tarea_idle)) & 0xFFFF;

    gdt[GDT_TAREA_IDLE].base_23_16 = 
    (((unsigned int) (&tarea_idle)) >> 16) & 0xFF;

    gdt[GDT_TAREA_IDLE].base_31_24 = (
    	(unsigned int) (&tarea_idle)) >> 24 ;

	mmu_mapear_pagina(DIR_VIRTUAL_TAREA,0x27000,0x20000,0,1); // escritura nivel cero
	mmu_mapear_pagina(DIR_VIRTUAL_TAREA + 0x1000,0x27000,0x21000,0,1); // escritura nivel cero

}

//la vamos a llamar con cr3_inicial = 0x30000
void tss_iniciarTareas(){
	//en cada ciclo inicializamos un navio y una bandera
	unsigned int id_tarea = 0x0000;
	unsigned int cr3_paCadaTarea;
	int j=0;	
	int i;
	for(i = 0; i < 16; i= i+2){
		tss* tss_nueva = (tss*) mmu_proxima_pagina_fisica_libre();		
		cr3_paCadaTarea = (unsigned int) mmu_inicializar_dir_tarea(id_tarea); //le pasamos el id ; llama tres paginas
    		//inicio navio
        //lo que no tocamos
    	tss_nueva->	ptl     =0x0;
        tss_nueva->	unused0	=0x0;
        tss_nueva->	unused1	=0x0;
        tss_nueva-> esp1	=0x0;
        tss_nueva->	ss1		=0x0;
        tss_nueva->	unused2	=0x0;
        tss_nueva-> esp2	=0x0;
        tss_nueva->	ss2		=0x0;
        tss_nueva->	unused3	=0x0;
        tss_nueva-> eax		=0x0;
        tss_nueva-> ecx		=0x0;
        tss_nueva-> edx		=0x0;
        tss_nueva-> ebx		=0x0;
        tss_nueva-> ebp		=0x0;
        tss_nueva-> esi		=0x0;
        tss_nueva-> edi		=0x0;
        tss_nueva->	unused4	=0x0;
        tss_nueva->	unused5	=0x0;
        tss_nueva->	unused6	=0x0;        
        tss_nueva->	unused7	=0x0;        
        tss_nueva-> unused8	=0x0;        
        tss_nueva-> unused9	=0x0;
        tss_nueva-> ldt		=0x0;
        tss_nueva-> unused10=0x0;
        tss_nueva-> dtrap	=0x0;
        //lo que tocamos
		//tss_nueva->esp0 = (unsigned int) mmu_proxima_pagina_fisica_libre() + 0x1000; //para recorrerla topdown
		//unsigned pila_cero_tarea = tss_nueva->esp0;
		//pruebo mappeando la pila de nivel cero
        unsigned int pila_cero_fisica =  (unsigned int) mmu_proxima_pagina_fisica_libre(); //para recorrerla topdown
        //mmu_mapear_pagina(0x40003000,cr3_paCadaTarea,pila_cero_fisica,1,1);//mappeamos la primera pagina

		tss_nueva->ss0 = (GDT_IDX_DATA_0 << 3); // creo que va (GDT_IDX_DATA_0 << 3) | 3 
		tss_nueva->esp0 = pila_cero_fisica + 0x1000;
	    tss_nueva->cr3 = cr3_paCadaTarea;
	    tss_nueva->eip = DIR_VIRTUAL_TAREA;
	    tss_nueva->esp = 0x40001c00; // 0x40001C00
	    tss_nueva->ebp = 0x40001c00; //0x40001C00
	    tss_nueva->eflags = 0x202;
	    tss_nueva->es = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->cs = (GDT_IDX_CODE_3 << 3) | 3;//aca creo que va (GDT_IDX_CODE_3 << 3) | 3 ( RPL = 3 )
	    tss_nueva->ss = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->ds = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->fs = (GDT_IDX_DATA_3 << 3) | 3;// no seria << en vez de |?
	    tss_nueva->gs = (GDT_IDX_DATA_3 << 3) | 3;//chequear esto
	    tss_nueva->iomap = 0xFFFF;
	    tss_nueva->dtrap = 0x0;
		//Descripcion de Navio lista
	    //Apuntarle con el segmento en gdt
	        gdt[GDT_TAREA_1 + i].base_0_15  = ((unsigned int) tss_nueva) & 0xFFFF;
	        gdt[GDT_TAREA_1 + i].base_23_16 = (((unsigned int) tss_nueva )>> 16) & 0xFF;
	        gdt[GDT_TAREA_1 + i].base_31_24 = ((unsigned int) tss_nueva )>> 24 ;
		//lesto
	    //inicio bandera 
		/*tss_nueva = (tss*) mmu_proxima_pagina_fisica_libre();	
        //de nuevo
        tss_nueva-> ptl     =0x0;
        tss_nueva-> unused0 =0x0;
        tss_nueva-> unused1 =0x0;
        tss_nueva-> esp1    =0x0;
        tss_nueva-> ss1     =0x0;
        tss_nueva-> unused2 =0x0;
        tss_nueva-> esp2    =0x0;
        tss_nueva-> ss2     =0x0;
        tss_nueva-> unused3 =0x0;
        tss_nueva-> eax     =0x0;
        tss_nueva-> ecx     =0x0;
        tss_nueva-> edx     =0x0;
        tss_nueva-> ebx     =0x0;
        tss_nueva-> ebp     =0x0;
        tss_nueva-> esi     =0x0;
        tss_nueva-> edi     =0x0;
        tss_nueva-> unused4 =0x0;
        tss_nueva-> unused5 =0x0;
        tss_nueva-> unused6 =0x0;        
        tss_nueva-> unused7 =0x0;        
        tss_nueva-> unused8 =0x0;        
        tss_nueva-> unused9 =0x0;
        tss_nueva-> ldt     =0x0;
        tss_nueva-> unused10=0x0;
        tss_nueva-> dtrap   =0x0;
        //seteamos
		tss_nueva->esp0 = pila_cero_fisica + 0x500;
		tss_nueva->ss0 = (GDT_IDX_DATA_0 << 3);  // creo que va (GDT_IDX_DATA_0 << 3) | 3 
	    tss_nueva->cr3 = cr3_paCadaTarea;

	    unsigned int *dir_bandera= (unsigned int *)(0x10000 + id_tarea + 0x1FFC);
		tss_nueva->eip = 0x40000000 + *(dir_bandera);
	    
	    tss_nueva->esp = 0x40001FFC; // 0x40001FFC
	    tss_nueva->ebp = 0x40001FFC; // 0x40001FFC
	    tss_nueva->eflags = 0x202;
	    tss_nueva->es = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->cs = (GDT_IDX_CODE_3 << 3) | 3;//Figura 5: Dirección de la función bandera ¿?
	    tss_nueva->ss = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->ds = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->fs = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_nueva->gs = (GDT_IDX_DATA_3 << 3) | 3;//chequear esto
		tss_nueva->iomap = 0xFFFF;
		tss_nueva->dtrap = 0x0;
		//mismo contexto que su navio (bandera)
		//apunto segseltss
        gdt[GDT_TAREA_1 + (i+1)].base_0_15  = ((unsigned int) tss_nueva) & 0xFFFF;
        gdt[GDT_TAREA_1 + (i+1)].base_23_16 = (((unsigned int) tss_nueva )>> 16) & 0xFF;
        gdt[GDT_TAREA_1 + (i+1)].base_31_24 = ((unsigned int) tss_nueva )>> 24 ;
        id_tarea = id_tarea + 0x2000;
        */

        //guardo las cosas en el arreglo de tss_banderas
        tss_banderas[j]. ptl     =0x0;
        tss_banderas[j]. unused0 =0x0;
        tss_banderas[j]. unused1 =0x0;
        tss_banderas[j]. esp1    =0x0;
        tss_banderas[j]. ss1     =0x0;
        tss_banderas[j]. unused2 =0x0;
        tss_banderas[j]. esp2    =0x0;
        tss_banderas[j]. ss2     =0x0;
        tss_banderas[j]. unused3 =0x0;
        tss_banderas[j]. eax     =0x0;
        tss_banderas[j]. ecx     =0x0;
        tss_banderas[j]. edx     =0x0;
        tss_banderas[j]. ebx     =0x0;
        tss_banderas[j]. ebp     =0x0;
        tss_banderas[j]. esi     =0x0;
        tss_banderas[j]. edi     =0x0;
        tss_banderas[j]. unused4 =0x0;
        tss_banderas[j]. unused5 =0x0;
        tss_banderas[j]. unused6 =0x0;        
        tss_banderas[j]. unused7 =0x0;        
        tss_banderas[j]. unused8 =0x0;        
        tss_banderas[j]. unused9 =0x0;
        tss_banderas[j]. ldt     =0x0;
        tss_banderas[j]. unused10=0x0;
        tss_banderas[j]. dtrap   =0x0;
        //seteamos
		tss_banderas[j].esp0 = pila_cero_fisica + 0x500;
		tss_banderas[j].ss0 = (GDT_IDX_DATA_0 << 3);  // creo que va (GDT_IDX_DATA_0 << 3) | 3 
	    tss_banderas[j].cr3 = cr3_paCadaTarea;

	    unsigned int *dir_bandera= (unsigned int *)(0x10000 + id_tarea + 0x1FFC);
		tss_banderas[j].eip = 0x40000000 + *(dir_bandera);
	    
	    tss_banderas[j].esp = 0x40001FFC; // 0x40001FFC
	    tss_banderas[j].ebp = 0x40001FFC; // 0x40001FFC
	    tss_banderas[j].eflags = 0x202;
	    tss_banderas[j].es = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_banderas[j].cs = (GDT_IDX_CODE_3 << 3) | 3;//Figura 5: Dirección de la función bandera ¿?
	    tss_banderas[j].ss = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_banderas[j].ds = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_banderas[j].fs = (GDT_IDX_DATA_3 << 3) | 3;
	    tss_banderas[j].gs = (GDT_IDX_DATA_3 << 3) | 3;//chequear esto
		tss_banderas[j].iomap = 0xFFFF;
		tss_banderas[j].dtrap = 0x0;

		//apunto segseltss
        gdt[GDT_TAREA_1 + (i+1)].base_0_15  = ((unsigned int) &(tss_banderas[j])) & 0xFFFF;
        gdt[GDT_TAREA_1 + (i+1)].base_23_16 = (((unsigned int) &(tss_banderas[j]) )>> 16) & 0xFF;
        gdt[GDT_TAREA_1 + (i+1)].base_31_24 = ((unsigned int) &(tss_banderas[j]) )>> 24 ;
        id_tarea = id_tarea + 0x2000;

        //guardo la tss nueva de la bandera en el indice correspondiente de tss_banderas
        //tss_banderas[j]=*(tss_nueva);
        j++;

	}


}




