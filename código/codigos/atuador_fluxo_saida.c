/* Modulo do monitor responsável por ler e disponibilza os dados do sensor de nível p*/ 
#include <math.h>
#include <pthread.h>
#include <string.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; //Variável do tipo mutex e estática (acessível apenas a esse monitor), inicializada. Responsável pela exclusão mutua da seção crítica

static double comporta = 0;

void atuador_put_saida(double abertura){
    //Seção crítica
	pthread_mutex_lock( &exclusao_mutua); //Exclusão mútua dos recursos
	comporta = abertura;//Aloca o valor de temperatura na variável global
	pthread_mutex_unlock( &exclusao_mutua);//Libera os recursos exclusos
}

double atuador_get_saida(void){
	double aux;
	pthread_mutex_lock( &exclusao_mutua); //Exclusão mútua dos recursos
	aux = comporta;
	pthread_mutex_unlock( &exclusao_mutua);//Libera os recursos exclusos
	return aux;
}
