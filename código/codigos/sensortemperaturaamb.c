/* Modulo do monitor responsável por ler e disponibilza os dados dos sensores p*/ 
#include <math.h>
#include <pthread.h>
#include <string.h>


static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; //Variável do tipo mutex e estática (acessível apenas a esse monitor), inicializada. Responsável pela exclusão mutua da seção crítica
//Variáveis globas para amezanar os valores dos sensores, só são acessado por esse modulo do monitor sensor
static double s_temp_ambiente = 0;


/* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
void sensor_put_temperatura_ambiente( double temp_amb) {
	//Seção crítica
	pthread_mutex_lock( &exclusao_mutua); //Exclusão mútua dos recursos
	s_temp_ambiente = temp_amb;//Aloca o valor de temperatura na variável global
	pthread_mutex_unlock( &exclusao_mutua);//Libera os recursos exclusos

}

/* Chamado por qualquer thread que precisa do valor lido do sensor */ 
double sensor_get_temperatura_ambiente(void) {
	double aux; 
	pthread_mutex_lock(&exclusao_mutua); 
	aux = s_temp_ambiente; //Armazena localmente o valor da variável global temperatura
	pthread_mutex_unlock(&exclusao_mutua); 
	return aux;//Retorna o valor da temperatura
}




