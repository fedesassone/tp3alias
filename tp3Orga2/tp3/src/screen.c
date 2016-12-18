/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "colors.h"
#include "screen.h"
#include "defines.h"
#include "sched.h"

#define BANDERA_BUFFER  0x40001000

/*str para guardarme info de registros*/

struct {
  unsigned int eax;
  
  char *error;
  unsigned int error_len;
  unsigned int tareaId;
  
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  unsigned int esi;
  unsigned int edi;
  unsigned int ebp;
  unsigned int esp;
  unsigned int eip;
  unsigned int cr0;
  unsigned int cr2;
  unsigned int cr3;
  unsigned int cr4;

  unsigned int cs;
  unsigned int ds;
  unsigned int es;
  unsigned int fs;
  unsigned int gs;
  unsigned int ss;
  
  unsigned int eflags;
  
} __attribute__((__packed__)) debug_info;



void print(unsigned int dest, const char * text, unsigned int x, unsigned int y, unsigned short attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) dest;
    int i;
    for (i = 0; text[i] != 0; i++) {
        p[y][x].c = (unsigned char) text[i];
        p[y][x].a = (unsigned char) attr;
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}//imprime puntero a char

void print_hex(unsigned int dest, unsigned int numero, int size, unsigned int x, unsigned int y, unsigned short attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) dest;
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}// imprime en hexa

void print_int(unsigned int dest, unsigned int n, unsigned int x, unsigned int y, unsigned short attr){
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) dest;
    if( n > 9 ) {
      int a = n / 10;
      n -= 10 * a;
      print_int(dest,a,x-1,y,attr);
    }
    p[y][x].c = '0'+n;
    p[y][x].a = attr;
}//imprime un short

void iniciarBufferEstado(){
    //linea fondo negro y letras blancas de 80pix
    unsigned int i;
    unsigned int x = 0;
    unsigned int y = 0;
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BUFFER_ESTADO;
//primer linea 
    for(i=0; i<80; i++){
            p[y][x].c = ' ';
            p[y][x].a = (C_FG_WHITE | C_BG_BLACK);
            x++;
            if(x == VIDEO_COLS){
                x=0;
                y++;
            }
    }
//fondo gris
    x=0;
    y=1;
    for(i=0; i<1200 ; i++){
        p[y][x].c = ' ';
        p[y][x].a = (C_FG_BLACK | C_BG_LIGHT_GREY);
        x++;
        if(x == VIDEO_COLS){
            x=0;
            y++;
        }
    }
//linea celeste der
    x=50;
    y=1;
    for(i=0; i<28 ; i++){
        p[y][x].c = ' ';
        p[y][x].a = (C_FG_BLACK | C_BG_CYAN);
        x++;
        if(x == 78){
            x=50;
            y++;
        }
    }
//panel derecha, 
    x=50;
    y=2;
    for(i=0; i<364 ; i++){
        p[y][x].c = ' ';
        p[y][x].a = (C_FG_WHITE | C_BG_BLACK);
        x++;
        if(x == 78){
            x=50;
            y++;
        }
    }
//registros derecha
    x=51;
    y=2;
    const char *regs[20] ={"EAX","EBX","ECX","EDX","ESI","EDI","EBP","ESP","EIP",
    "CR0","CR2","CR3","CR4","CS","DS","ES","FS","GS","SS","EFLAGS"};
    for (i = 0; i < 19; ++i){
        print(BUFFER_ESTADO,regs[i],x,y,(C_BG_BLACK|C_FG_WHITE));
        y++;
        if(y==15){
            y=2;
            x=66;
        }
    }
    y=y+2;
    print(BUFFER_ESTADO,regs[19],x,y,(C_BG_BLACK|C_FG_WHITE));

//estados  
    x=1;
    y=16;
    for(i=0; i<624 ; i++){
        p[y][x].c = ' ';
        p[y][x].a = (C_FG_BLACK | C_BG_CYAN);
        x++;
        if(x == 79){
            x=1;
            y++;
        }
    }
}

void cargarBufferEstado(){
    unsigned int i;
    ca (*s)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BUFFER_ESTADO;
    ca (*d)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
    unsigned int x = 0;
    unsigned int y = 0;
    for(i=0; i<2000;i++){
        d[y][x].c = s[y][x].c;
        d[y][x].a = s[y][x].a;
        x++;
        if(x == VIDEO_COLS){
            x=0;
            y++;
        }
    }
    const char* a = " // Pabellon de Aragon -1 // F. Sassone - G. Teren";
    print(VIDEO,a,0,0,(C_FG_WHITE | C_BG_BLUE));    
}

void iniciarBufferMapa(){
    int i;
    ca (*mapa)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BUFFER_MAPA;

///////imprimoTierra
    int x = 0;
    int y = 0;
    for(i=0; i<256; i++){
        mapa[y][x].c = ' '; //character
        mapa[y][x].a =  (C_BG_GREEN); //atributo; fondo green, letras white
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
    
    int MarInicioX = 16;
    int MarInicioY = 3;

///////lleno el mar
    for(i=0; i<1664;i++){

        mapa[y][x].c =  ' '; //character
        mapa[y][x].a =  C_BG_CYAN; //atributo; fondo blue, letras white
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }

//////pongo pags 1 y 2 de tareas en mar
    int j;
    for (j=0;j<8;j++)
    {
    	 x = damePosX(MarInicioX,MarInicioY,scheduler.paginasTareas[j].p1);
    	 y = damePosY(MarInicioX,MarInicioY,scheduler.paginasTareas[j].p1);
         print_int(BUFFER_MAPA,j+1,x,y,C_FG_WHITE | C_BG_RED); 
     
         x = damePosX(MarInicioX,MarInicioY,scheduler.paginasTareas[j].p2);
    	 y = damePosY(MarInicioX,MarInicioY,scheduler.paginasTareas[j].p2);
     
         print_int(BUFFER_MAPA,j+1,x,y,C_FG_WHITE | C_BG_RED);     
         
    }

///////pongo anclas de tareas en tierra
        //estan todas en 0, no es nec
        print_int(BUFFER_MAPA,8,0,0,C_FG_WHITE | C_BG_RED);     
}

int damePosX(int x,int y,unsigned int pagina )
{
		int i;
	    int pos = (pagina - 0x10000)/0x1000;
        for(i=0; i<pos;i++)
        {
        	x++;
	        if (x == VIDEO_COLS) {
	            x = 0;
	            y++;
	        }
    	}
    	return x;
    
}

int damePosY(int x,int y,unsigned int pagina )
{
		int i;
	    int pos = (pagina - 0x10000)/0x1000;
        for(i=0; i<pos;i++)
        {
        	x++;
	        if (x == VIDEO_COLS) {
	            x = 0;
	            y++;
	        }
    	}
    	return y;
    
}

void cargarBufferMapa(){
    int i;
    int x=0;
    int y=0;
    ca(*d)[VIDEO_COLS] = (ca(*)[VIDEO_COLS]) VIDEO_SCREEN;
    ca(*s)[VIDEO_COLS] = (ca(*)[VIDEO_COLS]) BUFFER_MAPA;
    for(i=0;i<2000;i++){
        d[y][x].c = s[y][x].c;
        d[y][x].a = s[y][x].a;
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void actualizarBufferEstado_Bandera_i(char id_Bandera){
    int i;
    ca (*d)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BUFFER_ESTADO;
    ca (*s)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BANDERA_BUFFER;
    //inicializo vars
    int x = 2+(id_Bandera* 12);
    int y;
    int j;

    int cont=0;
    if(id_Bandera<4)y=3;
    if(id_Bandera>3)y=10;
    char texto[7] = {'N','A','V','I','O',' ', id_Bandera+1};
    for(j=0; j<7;j++){
        d[y-1][x+3].c = texto[j];
        d[y-1][x+3].a = (C_FG_WHITE | C_BG_LIGHT_GREY);
        j++;
    }
    //actualizo buffer estado
    for(i=0;i<50;i++){
        d[y][x].c = s[y][x].c;
        d[y][x].a = s[y][x].a;
        x++;
        cont++;
        if(cont==5){
            x = 2+(id_Bandera* 12);
            cont = 0;
            y++;
        }
    } //fijarse donde se llama

}

void actualizarBufferEstado_UltimoProblema(){
    unsigned int i;
    unsigned int x = 50;
    unsigned int y = 1;

    char *error = debug_info.error;
    ca (*d)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BUFFER_ESTADO;
    //error
    print(BUFFER_ESTADO, error,x,y,(C_BG_CYAN | C_FG_BLACK ));
    
    x=71;
    char texto[7] = {'N','A','V','I','O',' ', ((char)debug_info.tareaId) + 1};    
    //navio i
    for (i = 0; i < 7; ++i){
        d[y][x].c = texto[i];
        d[y][x].a = (C_BG_CYAN | C_FG_BLACK );
        x++;         
    }
    //registros der
    x=55;
    y=2;
    unsigned int nums[20] ={debug_info.eax,debug_info.ebx, debug_info.ecx, debug_info.edx, 
        debug_info.esi, debug_info.edi, debug_info.ebp, debug_info.esp, debug_info.eip, 
        debug_info.cr0, debug_info.cr2, debug_info.cr3, debug_info.cr4, 
        debug_info.cs, debug_info.ds, debug_info.es, debug_info.fs, 
        debug_info.gs, debug_info.ss, debug_info.eflags} ;
    for (i = 0; i < 19; ++i){
        print_hex(BUFFER_ESTADO,nums[i],8,x,y,(C_BG_BLACK|C_FG_WHITE));
        y++;
        if(y==15){
            y=2;
            x=69;
        }
    }
    y=y+3;
    print_hex(BUFFER_ESTADO,nums[19],8,x,y,(C_BG_BLACK|C_FG_WHITE));
}

void actualizarBufferEstado_Paginas(){
    int i;
    unsigned int y = 16;
    for (i = 0; i < 8; ++i){
        //numero
        print_int(BUFFER_ESTADO,i+1, 1, y, (C_FG_BLACK|C_BG_CYAN));
        //caract
        print(BUFFER_ESTADO, "P1:",3 ,y,(C_BG_CYAN | C_FG_BLACK ));    
        print(BUFFER_ESTADO, "P2:",15,y,(C_BG_CYAN | C_FG_BLACK ));    
        print(BUFFER_ESTADO, "P3:",29,y,(C_BG_CYAN | C_FG_BLACK ));    
        //pags
        print_hex(BUFFER_ESTADO,scheduler.paginasTareas[i].p1 ,8,6 ,y,(C_BG_BLACK|C_FG_WHITE));    
        print_hex(BUFFER_ESTADO,scheduler.paginasTareas[i].p2 ,8,18,y,(C_BG_BLACK|C_FG_WHITE));    
        print_hex(BUFFER_ESTADO,scheduler.paginasTareas[i].p3 ,8,32,y,(C_BG_BLACK|C_FG_WHITE));    

        y++;
    }
    
}

void matarEnBuffer(){
    ca (*d)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) BUFFER_ESTADO;
    int i;
    int x=2;
    int y=16+scheduler.tarea_actual;
    //pone en rojo
    for (i = 2; i < 79; ++i)
    {
        d[y][x].a = (C_FG_WHITE | C_BG_RED);
        x++;
    }
    //imprime mensaje
    x=50;
    print(BUFFER_ESTADO,debug_info.error,55,y,(C_FG_WHITE | C_BG_RED));
}








