/* Modulo do monitor responsável por ler e disponibilza os dados do sensor de nível p*/ 
#include <math.h>
#include <pthread.h>
#include <string.h>


static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; //Variável do tipo mutex e estática (acessível apenas a esse monitor), inicializada. Responsável pela exclusão mutua da seção crítica
static pthread_cond_t alarme = PTHREAD_COND_INITIALIZER; 
//Variáveis globas para amezanar os valores dos sensores, só são acessado por esse modulo do monitor sensor

static double s_nivel = 0; 

static double limite_atual = HUGE_VAL;


void sensor_put_nivel( double niv) {
	//Seção crítica
	pthread_mutex_lock( &exclusao_mutua); //Exclusão mútua dos recursos
	s_nivel = niv;//Aloca o valor de nível na variável global
	pthread_mutex_unlock( &exclusao_mutua);//Libera os recursos exclusos
}


double sensor_get_nivel(void) {
	double aux; 
	pthread_mutex_lock(&exclusao_mutua); 
	aux = s_nivel; //Armazena localmente o valor da variável global nível
	pthread_mutex_unlock(&exclusao_mutua); 
	return aux; //Retorna o valor do nível
}

//verificar 
/*double sensor_get_nivel(void) {
	double aux; 
	pthread_mutex_lock(&exclusao_mutua); 
	
	if(strncmp(s, "Ta", 1) == 0)
	aux = sens_Ta; //Armazena localmente o valor da variável global temperatura do ar
	
	else if (strncmp(s, "T", 1) == 0)
	aux = sens_T; //Armazena localmente o valor da variável global temperatura da agua no interior do recipiente
	
	else if (strncmp(s, "Ti", 1) == 0)
	aux = sens_Ti; //Armazena localmente o valor da variável global temperatura da agua que entra no recipiente
	
	else if (strncmp(s, "No", 1) == 0)
	aux = sens_No; //Armazena localmente o valor da variável global fluxo de agua de saida do recipiente
	
	else if (strncmp(s, "H", 1) == 0)
	aux = sens_H; //Armazena localmente o valor da variável global altura da coluna de agua dentro do recipiente
		
	pthread_mutex_unlock(&exclusao_mutua); 
	return aux; //Retorna o valor do nível
}
*/

/* Thread fica bloqueada até o valor do sensor chegar em limite */ 


