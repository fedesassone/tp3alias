/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Bool */
/* -------------------------------------------------------------------------- */
#define TRUE                    0x00000001
#define FALSE                   0x00000000



/* Misc */
/* -------------------------------------------------------------------------- */
#define CANT_TAREAS             8

#define TAMANO_PAGINA           0x00001000

#define TASK_SIZE               2 * 4096

#define PAGE_DIRECTORY_KERNEL   0x27000
#define PAGE_TABLE_KERNEL		0x28000
#define PAGE_SIZE				0x1000
#define INICIO_PAGINAS_LIBRES	0x30000
#define DIR_VIRTUAL_TAREA		0x40000000
#define PAGINA_MAPEADA			0x8001000





/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_IDX_NULL_DESC           0x12 
#define GDT_IDX_DATA_0				0x13
#define GDT_IDX_DATA_3				0x14
#define GDT_IDX_CODE_0				0X15
#define GDT_IDX_CODE_3				0X16
#define GDT_IDX_PANTALLA_KERNEL		0x17
#define GDT_TAREA_IDLE				0X18
#define GDT_TAREA_INICIAL			0X19
#define GDT_TAREA_1					0x1A
#define GDT_TAREA_1_BANDERA			0x1B   
#define GDT_TAREA_2					0x1C
#define GDT_TAREA_2_BANDERA			0x1D 
#define GDT_TAREA_3					0x1E
#define GDT_TAREA_3_BANDERA			0x1F 
#define GDT_TAREA_4					0x20
#define GDT_TAREA_4_BANDERA			0x21 
#define GDT_TAREA_5					0x22
#define GDT_TAREA_5_BANDERA			0x23 
#define GDT_TAREA_6					0x24
#define GDT_TAREA_6_BANDERA			0x25 
#define GDT_TAREA_7					0x26
#define GDT_TAREA_7_BANDERA			0x27 
#define GDT_TAREA_8					0x28
#define GDT_TAREA_8_BANDERA			0x29  

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */
#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */
#define VIDEO                   0x000B8000 /* direccion fisica del buffer de video */

/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */
#define TASK_CODE               0x40000000 /* direccion virtual del codigo */

#define TASK_IDLE_CODE          0x40000000 /* direccion virtual del codigo de la tarea idle */
#define TASK_IDLE_STACK         0x003D0000 /* direccion virtual de la pila de la tarea idle */
#define TASK_IDLE_STACK_RING_0  0x0002B000 /* direccion fisica de la pila de la tarea idle */

#define TASK_ANCLA              0x40002000
#define TASK_ANCLA_FIS          0x00000000

#define AREA_TIERRA_INICIO      0x00000000  /* 0.0 MB     */
#define AREA_TIERRA_FIN         0x000FFFFF  /* 1.0 MB - 1 */
#define AREA_MAR_INICIO         0x00100000  /* 1.0 MB     */
#define AREA_MAR_FIN            0x0077FFFF  /* 7.5 MB - 1 */

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */
#define TASK_1_CODE_SRC_ADDR    0x00010000
#define TASK_2_CODE_SRC_ADDR    0x00012000
#define TAKS_3_CODE_SRC_ADDR    0x00014000
#define TASK_4_CODE_SRC_ADDR    0x00016000
#define TASK_5_CODE_SRC_ADDR    0x00018000
#define TASK_6_CODE_SRC_ADDR    0x0001A000
#define TASK_7_CODE_SRC_ADDR    0x0001C000
#define TASK_8_CODE_SRC_ADDR    0x0001E000

#define TASK_IDLE_CODE_SRC_ADDR 0x00020000

#endif  /* !__DEFINES_H__ */
