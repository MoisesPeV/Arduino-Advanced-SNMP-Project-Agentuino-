#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <Arduino.h>

// GRUPO DE INFORMACION


extern uint32_t locUpTime;                                                      // read-only (static)
extern char locContact[20];                                                     // should be stored/read from EEPROM - read/write (not done for simplicity)
extern char locName[20];                                                        // should be stored/read from EEPROM - read/write (not done for simplicity)
extern char locLocation[20];                                                    // should be stored/read from EEPROM - read/write (not done for simplicity)
extern char email[30];
extern char phonenumber[10];
extern char ip[15];
extern byte my_IP_address[4];// arduino IP address


// GRUPO SENSORES LEIDOS

extern int32_t index1;
extern int32_t index2;
extern int32_t index3;
extern int32_t index4;

extern char descr1[25];
extern char descr2[20];
extern char descr3[20];
extern char descr4[20];

extern uint32_t instanteLectura1;
extern uint32_t instanteLectura2;
extern uint32_t instanteLectura3;
extern uint32_t instanteLectura4;

extern int32_t sensortemp;  
extern int32_t sensorhum;
extern int32_t valorled1;
extern int32_t tempArduino;

extern int32_t tiempoMuestreo1;
extern int32_t tiempoMuestreo2;
extern int32_t tiempoMuestreo3;
extern int32_t tiempoMuestreo4;


//GRUPO EVENTOS


extern int32_t ind1;
extern int32_t ind2;
extern int32_t ind3;
extern int32_t ind4;
extern int32_t ind5;

extern int32_t Sind1;
extern int32_t Sind2;
extern int32_t Sind3;
extern int32_t Sind4;
extern int32_t Sind5;

extern char d1[20];
extern char d2[20];
extern char d3[20];
extern char d4[20];
extern char d5[20];


extern int32_t uS1;
extern int32_t uS2;
extern int32_t uS3;
extern int32_t uS4;
extern int32_t uS5;

extern int32_t uI1;
extern int32_t uI2;
extern int32_t uI3;
extern int32_t uI4;
extern int32_t uI5;

extern byte ip1[4];
extern byte ip2[4];
extern byte ip3[4];
extern byte ip4[4];
extern byte ip5[4];




#endif
