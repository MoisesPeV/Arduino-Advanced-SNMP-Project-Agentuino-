#include "Variable.h"

uint32_t locUpTime              = 0;                                           
char locContact[20]             = "Vicente";                      
char locName[20]                = "Prototipo 1";                                
char locLocation[20]            = "Dormitorio";                                
char ip[15]                         = "192.168.1.177";   
char email[30]                      = "trabajogredes24@gmail.com";       
char phonenumber[10]                = "639358137";
byte my_IP_address[4]       = {192, 168, 1, 177};                            


int32_t index1              = 1;
int32_t index2             = 2; 
int32_t index3             = 3; 
int32_t index4             = 4; 

char descr1[25] = "Sensor de Temperatura";
char descr2[20] = "Sensor Capacitivo";
char descr3[20] = "Voltimetro";
char descr4[20] = "Accesos Web";

int32_t sensortemp             = 7;  
int32_t sensorhum              = 0;  
int32_t valorled1              ; 
int32_t tempArduino            = 0;


uint32_t instanteLectura1 = 0;
uint32_t instanteLectura2 = 0;
uint32_t instanteLectura3 = 0;
uint32_t instanteLectura4 = 0;

int32_t tiempoMuestreo1 = 10;
int32_t tiempoMuestreo2 = 10;
int32_t tiempoMuestreo3 = 10;
int32_t tiempoMuestreo4 = 10;

// EVENTOS

 int32_t ind1 = 1;
 int32_t ind2 = 2;
 int32_t ind3 = 3;
 int32_t ind4 = 4;
 int32_t ind5 = 5;

 int32_t Sind1;
 int32_t Sind2;;
 int32_t Sind3;
 int32_t Sind4;
 int32_t Sind5;

char d1[20] = "mensaje evento1";
char d2[20] = "mensaje evento2";
char d3[20] ="mensaje evento3" ;
char d4[20] ="mensaje evento4";
char d5[20] = "mensaje evento5";

 int32_t uS1;
 int32_t uS2;
 int32_t uS3;
 int32_t uS4;
 int32_t uS5;

 int32_t uI1=0;
 int32_t uI2;
 int32_t uI3;
 int32_t uI4;
 int32_t uI5;

 byte ip1[4];
 byte ip2[4];
 byte ip3[4];
 byte ip4[4];
 byte ip5[4];
