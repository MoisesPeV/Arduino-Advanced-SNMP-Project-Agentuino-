#ifndef _MIB_H_
#define _MIB_H_


const char led1set[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.4"; // read-only  (Integer)
const char led2set[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.5"; // read-only  (Integer)





const char sysName[] PROGMEM = "1.3.6.1.4.1.36582.1.1.0"; 

const char sysLocation[] PROGMEM = "1.3.6.1.4.1.36582.1.2.0";

const char ipAddress[] PROGMEM = "1.3.6.1.4.1.36582.1.3.0";

const char sysUpTime[] PROGMEM = "1.3.6.1.4.1.36582.1.4.0"; 

const char sysContact[] PROGMEM = "1.3.6.1.4.1.36582.1.5.0"; 

const char telef[] PROGMEM = "1.3.6.1.4.1.36582.1.6.0";

const char mail[] PROGMEM = "1.3.6.1.4.1.36582.1.7.0";






// VALORES LEIDOS
const char ifIndex1[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.1.1"; // read-only  (Integer)
const char ifIndex2[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.1.2"; // read-only  (Integer)
const char ifIndex3[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.1.3"; // read-only  (Integer)
const char ifIndex4[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.1.4"; // read-only  (Integer)


const char des1[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.2.1"; // read-only  (Integer)
const char des2[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.2.2"; // read-only  (Integer)
const char des3[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.2.3"; // read-only  (Integer)
const char des4[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.2.4"; // read-only  (Integer)



const char sensorTemp[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.3.1"; // read-only  (Integer)
const char sensorHum[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.3.2"; // read-only  (Integer)
const char led1[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.3.3"; // read-only  (Integer)
const char tempArd[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.3.4"; // read-only  (Integer)

const char inst1[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.4.1"; // read-only  (Integer)
const char inst2[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.4.2"; // read-only  (Integer)
const char inst3[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.4.3"; // read-only  (Integer)
const char inst4[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.4.4"; // read-only  (Integer)

const char tMuestreo1[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.5.1"; // read-only  (Integer)
const char tMuestreo2[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.5.2"; // read-only  (Integer)
const char tMuestreo3[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.5.3"; // read-only  (Integer)
const char tMuestreo4[] PROGMEM = "1.3.6.1.4.1.36582.2.1.1.5.4"; // read-only  (Integer)







//EVENTOS 
const char Index1[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.1.1"; // read-only  (Integer)
const char Index2[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.1.2"; // read-only  (Integer)
const char Index3[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.1.3"; // read-only  (Integer)
const char Index4[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.1.4"; // read-only  (Integer)
const char Index5[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.1.5"; // read-only  (Integer)


const char s1[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.2.1"; // read-only  (Integer)
const char s2[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.2.2"; // read-only  (Integer)
const char s3[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.2.3"; // read-only  (Integer)
const char s4[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.2.4"; // read-only  (Integer)
const char s5[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.2.5"; // read-only  (Integer)


const char dd1[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.3.1"; // read-only  (Integer)
const char dd2[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.3.2"; // read-only  (Integer)
const char dd3[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.3.3"; // read-only  (Integer)
const char dd4[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.3.4"; // read-only  (Integer)
const char dd5[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.3.5"; // read-only  (Integer)

const char us1[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.4.1"; // read-only  (Integer)
const char us2[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.4.2"; // read-only  (Integer)
const char us3[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.4.3"; // read-only  (Integer)
const char us4[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.4.4"; // read-only  (Integer)
const char us5[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.4.5"; // read-only  (Integer)

const char is1[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.5.1"; // read-only  (Integer)
const char is2[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.5.2"; // read-only  (Integer)
const char is3[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.5.3"; // read-only  (Integer)
const char is4[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.5.4"; // read-only  (Integer)
const char is5[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.5.5"; // read-only  (Integer)

const char IP1[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.6.1"; // read-only  (Integer)
const char IP2[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.6.2"; // read-only  (Integer)
const char IP3[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.6.3"; // read-only  (Integer)
const char IP4[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.6.4"; // read-only  (Integer)
const char IP5[] PROGMEM = "1.3.6.1.4.1.36582.3.1.1.6.5"; // read-only  (Integer)

 //VALOR INVENTADO PARA QUE LA TABLA CARGUE BIEN 

const char inventado[] PROGMEM = "1.3.6.1.4.1.36582.4.1.1.1.1"; // read-only  (Integer)

      
#endif
