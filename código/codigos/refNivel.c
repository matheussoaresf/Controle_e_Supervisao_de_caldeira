#include <math.h>
#include <pthread.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER;
static double ref_ent = 0;

/*Chama a thread que le o sensor e fornece o valor lido */
void put_refNivel(double ref) {
  pthread_mutex_lock(&exclusao_mutua);
  ref_ent = ref;
  pthread_mutex_unlock(&exclusao_mutua);
}

/*Chama qualquer thread que precisa do valor lido do sensor */ 
double get_refNivel(void) {
  double aux;

  pthread_mutex_lock(&exclusao_mutua);
  aux = ref_ent;
  pthread_mutex_unlock(&exclusao_mutua);

  return aux;
}
