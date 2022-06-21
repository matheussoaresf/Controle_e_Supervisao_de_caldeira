/* Monitor buffer duplo, no arquivo bufduplo.c */

#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TAMBUF 200 

//vetor com o tamanho de Buffer definido acima
static long buffer_0[TAMBUF]; 
static long buffer_1[TAMBUF];

//Variáveis auxiliares
static int emuso = 0; 
static int prox_insercao = 0; 
static int gravar = -1;

//Variavel de exclusão mútua
static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER;
//Váriavel que verifica se o vetor está cheio
static pthread_cond_t buffer_cheio = PTHREAD_COND_INITIALIZER;


void bufduplo_insereLeitura( long leitura) {
	pthread_mutex_lock( &exclusao_mutua); // Faz um Lock no Mutex
	if( emuso == 0) //Caso em uso estja em 0 logo o buffer utulizado é o 0
		buffer_0[prox_insercao] = leitura; 
	else // se não é utilizado o buffer 1
		buffer_1[prox_insercao] = leitura; 
	++prox_insercao;
    //verifica se o buffer está cheiuo
	if( prox_insercao == TAMBUF){ 
		gravar = emuso; 
		emuso = (emuso + 1)% 2; 
		prox_insercao = 0; 
		pthread_cond_signal( &buffer_cheio); 
	}  
	pthread_mutex_unlock( &exclusao_mutua);
}

long * bufduplo_esperaBufferCheio( void) {
	long *buffer; 
	pthread_mutex_lock( &exclusao_mutua); 
	while( gravar == -1) 
		pthread_cond_wait( &buffer_cheio, &exclusao_mutua);
	if(gravar==0) 
		buffer = buffer_0; 
	else 
		buffer = buffer_1; 
	gravar = -1; 
	pthread_mutex_unlock( &exclusao_mutua); 
	return buffer; 
}
int tamBuf(void){
	
	return (int)TAMBUF;
}