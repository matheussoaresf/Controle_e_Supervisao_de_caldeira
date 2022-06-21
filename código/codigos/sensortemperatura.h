/*Cabeçalho do modulo monitor dos sensores*/
#ifndef SENSORTEMPERATURA_H
#define SENSORTEMPERATURA_H

void sensor_put_temperatura( double temp);
double sensor_get_temperatura(void);

void sensor_alarmeT(double limite);

#endif



