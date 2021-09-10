
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using namespace std;


typedef struct Balas {
	char actual[1];
	char stack[10];
	int x;
	int y;
	int max;
	Balas* Siguiente;

};
typedef Balas *PtrBalas;


//Globales
//**********************************************************

#define FPS 40.0
const int numHileras = 50;

//Elementos de allegro que se utilizarán para el juego
//**********************************************************
ALLEGRO_DISPLAY* pantalla;
ALLEGRO_FONT* fuente;

//LAS SIGUIENTES DOS FUNCIONES SE USAN PARA GENERAR NUMEROS RANDOM "VERDADEROS"
//EN PERIODOS DE TIEMPO MUY CORTOS
long g_seed = 1;
inline int fastrand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}
char GenerarRandom()
{
	char c = 33 + rand() % (126 - 33);
	return c;
}
int random(int n) {
	int c = fastrand() % n;
	return c;
}
	




void inicializarstack(char stack[6]) {
	for (int i = 0; i < 6; i++) {
		stack[i] = ' ';
	}
}
void inicializarLista(PtrBalas& Lista) {
	Lista = nullptr;
}
void AgregarHilera(PtrBalas& Lista, PtrBalas& Hilera) {
	PtrBalas Aux;
	Aux = Lista;
	if (Aux != nullptr) {
		while (Aux->Siguiente != nullptr) {
			Aux = Aux->Siguiente;
		}
		Aux->Siguiente = Hilera;
		Aux->Siguiente->Siguiente = nullptr;

	}
	else {
		Lista = Hilera;
	}
}
PtrBalas CrearHilera(int x, int y) {
	PtrBalas Hilera = new(Balas);
	Hilera->x = x;
	Hilera->y = y;
	Hilera->Siguiente = nullptr;
	Hilera->max = random(10);
	return Hilera;
}


/*
CADA LINEA DE CARACTERES QUE VA CAYENDO SE GUARDA EN UN STACK, QUE ES UN ARREGLO DE CHAR
COMO SE VAN GENERANDO CARACTERES NUEVOS ENTONCES LOS CARACTERES EN EL STACK SE VAN DESPLAZANDO
A MODO DE UNA PILA FIFO, PERO COMO ES UN ARREGLO ENTONCES SE IMPLEMENTA MAS FACIL DE LA SIGUIENTE MANERA
*/
void desplazar(char stack[10]) {
	stack[10] = stack[9];
	stack[9] = stack[8];
	stack[8] = stack[7];
	stack[7] = stack[6];
	stack[6] = stack[5];
	stack[5] = stack[4];
	stack[4] = stack[3];
	stack[3] = stack[2];
	stack[2] = stack[1];
	stack[1] = stack[0];
}

/*
DIBUJAR LO QUE HACE ES EN BASE A UN STACK DEFINIDO Y UN CONJUNTO DE COORDENADAS IMPRIME EN EL DISPLAY
LA SECUENCIA DE CARACTERES ASCII, Y A SU VEZ LLAMA A LA FUNCION DESPLAZAR PARA SACAR DEL STACK EL CARACTER MAS VIEJO Y
AÑADIR AL INICIO EL MAS NUEVO
*/
char actual[1];
void dibujar(char caracter[1], char stack[10], int x, int y, int max) {
	int color = 255;
	for (int i = 0; i < max; i++) {
		if (i == 0) {
			al_draw_text(fuente, al_map_rgb(255,255,255), x, y-20, ALLEGRO_ALIGN_CENTRE, actual);
		}
		else {
			al_draw_text(fuente, al_map_rgb(0, color, 0), x, y - 20, ALLEGRO_ALIGN_CENTRE, actual);
			color -= 50;
			if (color < 50) {
				color = 25;
			}
		}
		
		actual[0] = stack[i];
		
		y -= 20;

	}

	al_flip_display();

	desplazar(stack);
	stack[0] = caracter[0];
}




int main() {

	//inicializar lista de tipo Balas
	PtrBalas Lista;
	inicializarLista(Lista);
	PtrBalas Hilera;

	//lista de coordenadas en x de las hileras 
	//crear hileras y encolar
	for (int i = 0; i < numHileras; i++) {
		int x = -1;
		x = random(800);
		
		Hilera = CrearHilera(x, random(300));
		AgregarHilera(Lista, Hilera);
	}

	

	// allegro init

	if (!al_init()) {
		fprintf(stderr, "No se puede iniciar allegro!\n");
		return -1;
	}

	//Esta línea de código permite que la ventana tenga la capacidad de cambiar de tamaño
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);

	pantalla = al_create_display(720, 480);
	al_set_window_position(pantalla, 200, 200);
	al_set_window_title(pantalla, "MATRIX");
	if (!pantalla) {
		fprintf(stderr, "No se puede crear la pantalla!\n");
		return -1;
	}
	//Líneas para obtener las funcionalidades del uso de las fuentes
	//*******************
	al_init_font_addon();
	al_init_ttf_addon();
	//*******************
	//Línea para obtener las funcionalidades de las imágenes
	//*******************
	al_init_image_addon();
	//*******************


	//Líneas para obtener las funcionalidades de los audios
	//*******************
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);
	//*******************

	fuente = al_load_font("lekton.ttf", 14, NULL);

	//Timers que se necesitarán para el juego
	//**********************************************************
	ALLEGRO_TIMER* primerTimer = al_create_timer(5 / FPS);


	//**********************************************************

	//Se crea una cola de eventos
	ALLEGRO_EVENT_QUEUE* colaEventos = al_create_event_queue();

	//Registro de los eventos
	//**********************************************************
	al_register_event_source(colaEventos, al_get_timer_event_source(primerTimer));
	//al_register_event_source(colaEventos, al_get_keyboard_event_source());
	//**********************************************************

	//Inicialización de los timer
	//**********************************************************
	al_start_timer(primerTimer);

	//**********************************************************
	bool hecho = true;
	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	//al_get_keyboard_state(&estadoTeclado);
	char caracter[1];
	char caracter2[1];
	int y = 10;
	int ySenal = 10;
	ALLEGRO_SAMPLE* soundEffect = al_load_sample("soundEffect.wav");

	while (hecho) {

		ALLEGRO_EVENT eventos;

		al_wait_for_event(colaEventos, &eventos);
		//al_clear_to_color(transparente);

		/*if (eventos.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (eventos.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				hecho = false;
			}
		}*/

		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			if (eventos.timer.source == primerTimer) {
				PtrBalas Aux = Lista;
				al_clear_to_color(al_map_rgb(0, 0, 0));
				//al_play_sample(soundEffect, 1.0, 0.0, 0.50, ALLEGRO_PLAYMODE_LOOP, 0);
				for (int i = 0; i < numHileras; i++) {
					
					caracter[0] = GenerarRandom();
					dibujar(caracter, Aux->stack, Aux->x, Aux->y, Aux->max);
					Aux->y = Aux->y + 20;

					Aux = Aux->Siguiente;

				}
				
				//AQUI VA LO QUE PASARIA EN EL EVENTO DADO POR EL PRIMER TIMER
				ySenal += 10;
				
			}

			//si l hilera llegó al final
			if (ySenal >= 350) {
				Lista = nullptr;
				for (int i = 0; i < numHileras; i++) {
					int x = -1;
					x = random(800);

					Hilera = CrearHilera(x, 10);
					AgregarHilera(Lista, Hilera);
				}

				PtrBalas Aux = Lista;
				for (int i = 0; i < numHileras; i++) {
					Aux->y = random(300);
					Aux = Aux->Siguiente;
				}
				ySenal = 10;
			
			}
				
		}




		al_flip_display();

	}

	al_destroy_display(pantalla);
	al_destroy_event_queue(colaEventos);
	al_destroy_timer(primerTimer);
	al_destroy_font(fuente);
	al_destroy_sample(soundEffect);

}
