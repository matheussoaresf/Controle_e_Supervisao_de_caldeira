//Definição de Bibliotecas
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "time.h" // utilizado para medição de tempo
#include "tela.h" //Inclusão do monitor que acessa/requisitar a tela do computador
//Inclusão do monitor que ler e disponibiliza valor dos dados para a threads da main 
#include "sensorfluxo.h"
#include "sensornivel.h"
#include "sensortemperatura.h"
#include "sensortemperaturaamb.h"
#include "sensortemperaturaentra.h"
#include "bufferDuplo.h"
//Inclusão do monitor que acessa o canal de comunicação do simulador
#include "socket.h"
#include "refTemperatura.h"//usar a referencia da temperatura
#include "refNivel.h"
//#include "bufduplo.h"
#include "atuador_aquecedor.h"
#include "atuador_fluxo_aquecida.h"
#include "atuador_fluxo_entrada.h"
#include "atuador_fluxo_saida.h"


//Definindo Variáveis necessárias para as medições de tempo
#define	NSEC_PER_SEC    (1000000000) 	// Numero de nanosegundos em um segundo
#define	N_AMOSTRAS	10000		// Numero de amostras (medições) coletadas
#define NUM_THREADS	6
long temp_exec[N_AMOSTRAS];		// Medicoes do tempo de execução da tarefa em microsegundos
double valores_altura[N_AMOSTRAS];
double valores_temperatura[N_AMOSTRAS];


//Thread que exibe os valores do Nível e Temperaturas na tela
#define NSEC_PER_SEC (1000000000)


void thread_le_sensor (void){ //Le Sensores periodicamente a cada 10ms
	char msg_enviada[1000];	
	struct timespec t, t_fim;
	long periodo = 10e6;

	int intervalo = 100; //Numero de vezes que o programa vai rodar até gravar no arquivo (100x10ms= 1000ms = 1s)
	int cont = 0;
	
	//variaveis dos atuadores
	double var_q, var_ni, var_na, var_nf;
	
	clock_gettime(CLOCK_MONOTONIC, &t);
	while(1){
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		sensor_put_temperatura(msg_socket("st-0"));
		sensor_put_nivel(msg_socket("sh-0"));
		sensor_put_temperatura_ambiente(msg_socket("sta0"));
		sensor_put_fluxo(msg_socket("sno0"));
		sensor_put_temperatura_entrada(msg_socket("sti0"));
		
		var_q = atuador_get_aquecedor();
		sprintf(msg_enviada, "aq-%lf", var_q);
		msg_socket(msg_enviada);
                
		var_ni = atuador_get_entrada();
		sprintf(msg_enviada, "ani%lf", var_ni);
		msg_socket(msg_enviada);
		
		var_na = atuador_get_fluxo_aquecida();
		sprintf(msg_enviada, "ana%lf", var_na);
		msg_socket(msg_enviada);

		var_nf = atuador_get_saida();
		sprintf(msg_enviada, "anf%lf", var_nf);
		msg_socket(msg_enviada);

		intervalo--;

        if(intervalo == 0 && cont < N_AMOSTRAS){
			valores_altura[cont] = sensor_get_nivel();
			valores_temperatura[cont] = sensor_get_temperatura();
        
            intervalo = 100;
            cont++;        
        }

		if(cont >= N_AMOSTRAS){
			FILE * dados_t;
	        dados_t = fopen("sensor_t.txt", "w");
	        if(dados_t ==NULL){
		        printf("Erro na abertura do arquivo do sensor de temperatura\n");
		        exit(1);
	        }
            FILE * dados_h;
	        dados_h = fopen("sensor_h.txt", "w");
	        if(dados_h ==NULL){
		        printf("Erro na abertura do arquivo do sensor de nivel\n");
		        exit(1);
	        }
			for( int i=0; i<N_AMOSTRAS; i++){
				fprintf(dados_t, "%2lf\n", valores_altura[i]);
				fprintf(dados_h, "%2lf\n", valores_temperatura[i]);
				}
	
				fclose(dados_t);
				fclose(dados_h);	

		}
		
		t.tv_nsec += periodo;

		while(t.tv_nsec>= NSEC_PER_SEC){
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_nsec ++;
		}
			
	}		
}

//Aciona alarme após atingir a temperatura definida 
void thread_alarme (void){
	
	while(1){
		sensor_alarmeT(30); //Definindo a temperatura
		aloca_tela();//reserva recurso da tela
		printf("ALARME, TEMPERATURA LIMITE ATINGIDA !!!\n");
		libera_tela();//libera os recursos da tela
		sleep(1);//Notifica a cada 1 segundo
	
	}
		
}

/*
// Altera os valores da referência durante a execução 
void thread_altera_ref (void){
	while(1){
		//Criando o menu
		int aux;
    		double temp, nivel;
		
		aloca_tela();
		system("tput reset"); //limpa tela
		printf("\n\n ------------------------------------------------- ");

		printf("\n Escolha uma Opcao digitando o numero referente \n");
		printf("\n 1 - Opcao 1 - Alterar Referencia de Temperatura ");
		printf("\n 2 - Opcao 2 - Alterar Referencia de Nivel de Agua ");
		printf("\n 3 - Opcao 3 - Alterar ambas Referencias ");
		printf("\n\n Escolha uma opcao: ");
			
		aux = getc(stdin);
			
		switch(aux){
			
			case '1':
			{
			
			printf("\n\n Opcao escolhida: 1 \n");
			printf("Digite um valor REFERENCIA para a Temperatura: ");
			scanf(" ");
			scanf("%lf", &temp);
			
			while(temp > 30.0){
				printf("Digite um valor de referencia menor que 30 graus: \n");
				scanf(" ");
				scanf("%lf",&temp);
    			}
	
			break;
			}
			
			case '2':
			{
			printf("\n\n Opcao escolhida: 2 \n");
			printf("Digite um valor REFERENCIA para o Nivel de Agua: \n");
			scanf(" ");
			scanf("%lf", &nivel);

			break;
			}
			
			case '3':
			{
			printf("\n\n Opcao escolhida: 3 \n");
			printf("Digite um valor REFERENCIA para a Temperatura: \n");
			scanf(" ");
			scanf("%lf", &temp);
	
			while(temp > 30.0){
				printf("Digite um valor de referência menor que 30 graus: \n");
				scanf(" ");
				scanf("%lf",&temp);
    			}
	
			printf("Digite um valor REFERENCIA para o Nivel de Agua: \n");
			scanf(" ");
			scanf("%lf", &nivel);

			break;
			}			
		}	
		
		libera_tela();
		
		put_refTemp(temp);
		put_refNivel(nivel);
	}
}

*/


void thread_controle_temperatura (void){

	struct timespec t, t_fim;
	long periodo = 50e6;  // Período de execução da tarefa é de 50ms
	double temperatura_referencia, temperatura, temp_entrada, temp_ambiente;//grava as temperaturas do processo
	double aquecedor, fluxo_entrada, fluxo_saida, fluxo_entrada_aquecida;
	double proporcional_erro, erro, aux;
	long temp_resp; // Armazena o calculo do momento em que inicia a tarefa até o fim

	//Atualiza o tempo e faz a leitura da hora atual e coloca na variavel t
  	clock_gettime(CLOCK_MONOTONIC, &t);
	while(1){
		// espera até o inicio do próximo periodo	
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		
		//coletar a temperatura de referência e as demais temperaturas
		temperatura_referencia = get_refTemp();
		temperatura = sensor_get_temperatura();
		temp_ambiente = sensor_get_temperatura_ambiente();
		temp_entrada = sensor_get_temperatura_entrada();
		fluxo_entrada = atuador_get_entrada();
		
		
		//A temperatura do recipiente é diferente da referencia?
		if(temperatura_referencia > temperatura){//temperatura de referencia é maior que a temperatura da caldeira ( Necessário esquentar a água )
			erro = temperatura_referencia-temperatura;
			proporcional_erro = ((erro)/temperatura_referencia)*100; //Calcula o erro (%) entre a temperatura de referência e a temperatura da caldeira
			if(fluxo_entrada > 0 && temp_entrada < temperatura_referencia){//está entrando agua com temperatura menor(controle de nivel está atuando no sistema para aumentar o nível)
				atuador_put_aquecedor(proporcional_erro*10000 + fabs(fluxo_entrada*4184*(temp_entrada-temperatura)));//Aumenta o aquecedor proporcional a vazão e ao erro
				if(proporcional_erro>=0.5){ //Caso a temperatura começa a cair bruscamente(erro fique maior que 5%)
					atuador_put_fluxo_aquecida(10.0);//aciona o fluxo de água quente, ajudando a aumentar o nível 
				}else{//Caso o erro seja menor que %5
					atuador_put_fluxo_aquecida(0.0);//desliga o fluxo de água quente
					atuador_put_saida(0.0); //Desliga o fluxo de saida
				}							
			}else{
				if(proporcional_erro>=0.5){//Se o erro for maior que 5%
					atuador_put_aquecedor(1000000);//Deixa o aquecedor no máximo					
					atuador_put_fluxo_aquecida(10.0);//liga o fluxo de água quente
					atuador_put_saida(10.0);//liga a saida com a mesma vasão de entrada quente
										
				}else{//após o erro for menor que 10% 
						atuador_put_aquecedor(proporcional_erro*100000);//Aciona o aquecedor proporcional ao erro
						atuador_put_fluxo_aquecida(0.0);//Desliga o fluxo de água quente
						atuador_put_saida(0.0);
				}
			}		

		}

		if(temperatura_referencia < temperatura ){//temperatura de referencia é menor que a temperatura da caldeira ( Necessário esfriar a água )
			atuador_put_aquecedor(0.0);//desliga o aquecedor
			atuador_put_fluxo_aquecida(0.0);//desliga o fluxo de água quente
			
			if(temp_entrada<=temperatura){//temperatura da água que entra é menor que a temperatura da caldeira (Água resfria a caldeira)
				erro = temperatura - temp_entrada;
				proporcional_erro = (erro/temperatura)*100;// A vasão é proporcional ao erro da temperatura da caldeira e da temperatura da água que entra
				if(proporcional_erro>=10){//se a temperatura de referencia for muito baixa em relação a temperatura do tanque (resfriar rapidamente)
					atuador_put_entrada(100.0);//liga o fluxo de entrada de água 
					atuador_put_saida(100.0);//liga o fluxo de saída de água 
				}else{//caso contrário
					atuador_put_entrada(proporcional_erro);//vasão de entrada é proporcional ao erro da temperatura de entrada e a referencia
					atuador_put_saida(proporcional_erro);
				}
			}else{//Quando a Temperatura da água que entra for maior que a temperatura da caldeira
				atuador_put_entrada(0.0);//Desliga o fluxo de entrada de água
				atuador_put_saida(0.0);//Desliga fluxo de saída de água 
			}
			erro = temperatura - temperatura_referencia;
			
		}
		
		if(erro>= 0 && erro<=0.001){ // estabilizar em relação a temperatura ambiente
			atuador_put_entrada(0.0);
			atuador_put_saida(0.0);
			atuador_put_fluxo_aquecida(0.0);
			if(temp_ambiente <= temperatura){
				proporcional_erro = (temperatura - temp_ambiente)/0.001;
				atuador_put_aquecedor(proporcional_erro);
			}
		}
			
			//Lê a hora atual do relógio
    		clock_gettime(CLOCK_MONOTONIC, &t_fim);

    		// calcula o tempo de resposta desda leitura dos sensores até mandar a msg via socket micro
    		temp_resp = 1e6 * (t_fim.tv_sec - t.tv_sec) + (t_fim.tv_nsec - t.tv_nsec) / 1e3;

    		bufduplo_insereLeitura(temp_resp);

    		// calcula o inicio do próximo periodo(define qual o próximo ponto no relógio)
    		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {// estrutura aponta para o periodo no relogio monotomico
      			t.tv_nsec -= NSEC_PER_SEC;
      			t.tv_sec++;
    		}	
	}

}




void thread_grava_temp_resp(void){
    FILE* dados_f;
	dados_f = fopen("dados_tempo_exec.txt", "w");
	if(dados_f == NULL){
		printf("Erro, nao foi possivel abrir o arquivo\n");
		exit(1);
	}

	int amostras = 1;
	int tambuffer = tamBuf();

	while(amostras++ <= N_AMOSTRAS / tambuffer) {
    //Espera até o buffer encher para descarregar no arquivo
    long * buf = bufduplo_esperaBufferCheio();

    int n2 = tamBuf();
    int tam = 0;

    while (tam < n2)
      fprintf(dados_f, "%ld\n", buf[tam++]);

    fflush(dados_f);

    aloca_tela();
    printf("Gravando arquivo...\n");
    libera_tela();
  }

	fclose(dados_f);	
}

void thread_mostra_status (void){
	double temperatura, nivel, fluxo, temp_amb, temp_entrada;
	int i = 0;
	while(1){
		temperatura = sensor_get_temperatura();
		nivel = sensor_get_nivel();
		
		fluxo = sensor_get_fluxo();
		temp_amb = sensor_get_temperatura_ambiente();
		temp_entrada = sensor_get_temperatura_entrada();
		
		aloca_tela();//Permite acesso exclusivo dos recursos para a tela do computador
		system("tput reset"); //limpa tela
		printf("---------------------------------------\n");
		printf("Temperatura (T)--> %.4lf\n", temperatura);
		printf("Nivel (H)--> %.4lf\n", nivel);
		
		printf("Fluxo (No)--> %.4lf\n", fluxo);
		printf("Temperatura Ambiente (Ta)--> %.4lf\n", temp_amb);
		printf("Temperatura Entrada (Ti)--> %.4lf\n", temp_entrada);
		
		printf("---------------------------------------\n");
		libera_tela();//Libera os recursos 

		sleep(1); //Executada a cada 1 segundo
	}

}



// Controlar nivel da caldeira 
void thread_controle_nivel(){
struct timespec t;
	long int periodo = 70e6; 	// 70ms
	
	char tam[1000];
	
	//Nivel de referencia
	double href = get_refNivel();
	double temp_ref = get_refTemp();
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);

	// Tarefa periodica iniciará em 1 segundo
	t.tv_sec++;

	while(1) {
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

                // Realiza seu trabalho
                //variaveis de comporta de entrada
                double atu_q, atu_ni, atu_na, atu_nf;
                
                //verificar se o nivel está baixo -> Hmin = 0,1m e Hmax = 3,0m
                if(sensor_get_nivel() < href){
                	//Verificar a temperatura
                	if(temp_ref < sensor_get_temperatura()){ //temperatura alta
                		atu_q = 0;
                		atu_ni = 100;
                		atu_na = 0;
                		atu_nf = 0;
			} else if(temp_ref > sensor_get_temperatura()){ //temperatura baixa
				atu_q = 0;
                		atu_ni = 100;
                		atu_na = 10;
                		atu_nf = 100000;
			} else if(temp_ref == sensor_get_temperatura()){ //temperatura ideal
				atu_q = 100;
                		atu_ni = 100;
                		atu_na = 0;
                		atu_nf = 0;
			}
                }
                
                //verificar se o nivel está alto -> Hmin = 0,1m e Hmax = 3,0m
                if(sensor_get_nivel() > href){
                	//Verificar a temperatura
                	if(temp_ref < sensor_get_temperatura()){ //temperatura alta
                		atu_q = 0;
                		atu_ni = 0;
                		atu_na = 0;
                		atu_nf = 100;
			} else if(temp_ref > sensor_get_temperatura()){ //temperatura baixa
				atu_q = 100000;
                		atu_ni = 0;
                		atu_na = 10;
                		atu_nf = 100;
			} else if(temp_ref == sensor_get_temperatura()){ //temperatura ideal
				atu_q = 100;
                		atu_ni = 0;
                		atu_na = 0;
                		atu_nf = 100;
			}
                }
                
		//verificação mais precisa para manter o nivel
                if((0.001) >= (sensor_get_nivel() - href) && (-0.001) <= (sensor_get_nivel() - href)){
                	//Verificar a temperatura
                	if(temp_ref < sensor_get_temperatura()){ //temperatura alta
                		atu_q = 0;
                		atu_ni = 0;
                		atu_na = 0;
                		atu_nf = 0;
			} else if(temp_ref > sensor_get_temperatura()){ //temperatura baixa
				atu_q = 0;
                		atu_ni = 0;
                		atu_na = 1;
                		atu_nf = 0;
			} else if(temp_ref == sensor_get_temperatura()){ //temperatura ideal
				atu_q = 0;
                		atu_ni = 0;
                		atu_na = 0;
                		atu_nf = 0;
			}
                }
		// enviar os valores para os atuadores
		
		// atuador do aquecedor
		//atuador_put_aquecedor(atu_q);
                
		// atuador de entrada de agua ambiente
		atuador_put_entrada(atu_ni);
		
		// atuador de entrada de agua aquecida 80 graus
		//atuador_put_fluxo_aquecida(atu_na);

		// atuador de saída de agua do esgoto controlado
		atuador_put_saida(atu_nf);
		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
}


void main( int argc, char *argv[]) {
	
	cria_socket(argv[1], atoi(argv[2]) ); //cria o canal de comunicação entre o simulador
	
	//estruturas e váriaveis para marcar o tempo no relógio
	struct timespec t, t_inicio, t_fim;
	int amostra = 0;		// Amostra corrente
	int periodo = 100000000; 	// 100ms
	int ord_prio[NUM_THREADS] = {1, 59, 1, 99, 99, 1};
	pthread_t threads[NUM_THREADS];
	pthread_attr_t pthread_custom_attr[NUM_THREADS];
	struct sched_param priority_param[NUM_THREADS]; 
   
    	

	int aux;
    	double temp, nivel;
		
	aloca_tela();
	system("tput reset"); //limpa tela
	printf("\n\n ------------------------------------------------- ");
	printf("\n Escolha uma Opcao digitando o numero referente \n");
	printf("\n 1 - Opcao 1 - Alterar Referencia de Temperatura ");
	printf("\n 2 - Opcao 2 - Alterar Referencia de Nivel de Agua ");
	printf("\n 3 - Opcao 3 - Alterar ambas Referencias ");
	printf("\n\n Escolha uma opcao: ");
			
	aux = getc(stdin);
			
	switch(aux){
			
		case '1':
		{
			
		printf("\n\n Opcao escolhida: 1 \n");
		printf("Digite um valor REFERENCIA para a Temperatura: ");
		scanf(" ");
		scanf("%lf", &temp);
			
		while(temp > 30.0){
			printf("Digite um valor de referencia menor que 30 graus: \n");
			scanf(" ");
			scanf("%lf",&temp);
    		}
	
		break;
		}
			
		case '2':
		{
		printf("\n\n Opcao escolhida: 2 \n");
		printf("Digite um valor REFERENCIA para o Nivel de Agua: \n");
		scanf(" ");
		scanf("%lf", &nivel);
		break;
		}
			
		case '3':
		{
		printf("\n\n Opcao escolhida: 3 \n");
		printf("Digite um valor REFERENCIA para a Temperatura: \n");
		scanf(" ");
		scanf("%lf", &temp);
	
		while(temp > 30.0){
		printf("Digite um valor de referência menor que 30 graus: \n");
		scanf(" ");
		scanf("%lf",&temp);
  		}
	
		printf("Digite um valor REFERENCIA para o Nivel de Agua: \n");
		scanf(" ");
		scanf("%lf", &nivel);
		break;
		
		}			
	}	
	libera_tela();
	
	
	put_refTemp(temp);
	put_refNivel(nivel);


	//Configura escalonador do sistema
	for(int i=0;i<NUM_THREADS;i++){
		pthread_attr_init(&pthread_custom_attr[i]);
		pthread_attr_setscope(&pthread_custom_attr[i], PTHREAD_SCOPE_SYSTEM);
		pthread_attr_setinheritsched(&pthread_custom_attr[i], PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&pthread_custom_attr[i], SCHED_FIFO);
		priority_param[i].sched_priority = ord_prio[i];
		if (pthread_attr_setschedparam(&pthread_custom_attr[i], &priority_param[i])!=0)
	  		fprintf(stderr,"pthread_attr_setschedparam failed\n");
	}

	pthread_create(&threads[0], &pthread_custom_attr[0], (void *) thread_mostra_status, NULL);
	pthread_create(&threads[1], &pthread_custom_attr[1], (void *) thread_le_sensor, NULL);
	pthread_create(&threads[2], &pthread_custom_attr[2], (void *) thread_alarme, NULL);
	pthread_create(&threads[3], &pthread_custom_attr[3], (void *) thread_controle_temperatura, NULL);
	pthread_create(&threads[4], &pthread_custom_attr[4], (void *) thread_controle_nivel, NULL);
	pthread_create(&threads[5], &pthread_custom_attr[5], (void *) thread_grava_temp_resp, NULL);

	for(int i=0;i<NUM_THREADS;i++){
		pthread_join( threads[i], NULL);

	};

}
