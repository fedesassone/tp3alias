/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__
  
typedef struct pn_s {
  unsigned int p1;
  unsigned int p2;
  unsigned int p3;
  
} __attribute__((__packed__)) paginas;  

typedef struct str_sched_tarea
{
	unsigned short tss_selector;
	unsigned int id;
	unsigned int viva;
}__attribute__((__packed__)) sched_tarea;

typedef struct str_sched_t
{
	unsigned char tarea_actual;
	unsigned char bandera_actual;
	unsigned char banderasPorCiclar;
	unsigned char banderasVivas;
	sched_tarea tareas[8];
	sched_tarea banderas[8];
	paginas paginasTareas[8];
}__attribute__((__packed__)) sched_t;

extern sched_t scheduler;

unsigned int tarea_actual();

unsigned short atender_int66();
void sched_inicializar();
unsigned short sched_proximo_indice();
unsigned short sched_proxima_bandera(); 
unsigned short atender_reloj();
void llamada (unsigned int eax,unsigned int ebx, unsigned int ecx);
unsigned short matar_tarea();
int dameIndTareaEnPos(unsigned int direccion);
int cuantasMeApuntan(unsigned int direccion);
#endif	/* !__SCHED_H__ */
