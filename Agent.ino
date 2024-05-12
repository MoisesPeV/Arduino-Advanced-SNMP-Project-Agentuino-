

#include "Streaming.h"         
#include <Ethernet.h>         
#include "Agentuino.h"
#include "MIB.h"
#include "Variable.h"
#include <CapacitiveSensor.h>
#include <string.h>

const int sensorPin = A0;



int numAccesos;

int numAccesosAnteriorMuestreo;



char* strings[4]; // Array de punteros a caracteres (strings)
int num_strings = 0; // Variable para mantener el número actual de strings en la lista
String readString;
// Crear un servidor en el puerto 80
EthernetServer server(80);
IPAddress address;
CapacitiveSensor capSensor = CapacitiveSensor(4,2);
//VARIABLES PARA MEDIR LOS RETRASOS
static int cont1;
static int cont2;
static int cont3;
static int contador4;

int x=100;


static int muestreo1Ant=10;
static int muestreo2Ant=10;
static int muestreo3Ant=10;
static int muestreo4Ant=10;


static int activada=0;
static int activada2=0;
static int activada3=0;
static int activada4=0;
static int activada5=0;




//VARIABLE PARA LA FUNCION DE EVENTOS
static int32_t* sensores[4];


//CONFIGURACIÓN IP Y MAC DEL ARDUINO
static byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF};
static byte ipArduino[] = {192, 168, 1, 177};
static byte gateway[] = {192, 168, 1, 1};
static byte subnet[] = {255, 255, 255, 0};



void setup() {
    Serial.begin(9600);
    
    Serial.println("Arduino Iniciado, logs preparados para ser servidos :)");
    
    pinMode(9,OUTPUT);
    pinMode(12,OUTPUT);
    pinMode(22,OUTPUT);
    digitalWrite(9,HIGH);


    addString("Trap generado por: Sensor de Temperatura");
    addString("Trap generado por: Sensor Capacitivo");
    addString("Trap generado por: Voltimetro");
    addString("Trap generado por: Numero de Accesos");


   sensores[0] = &sensortemp;
   sensores[1] = &sensorhum;
   sensores[2] = &valorled1;
   sensores[3] = &tempArduino;
    
    uS1=1000;
    digitalWrite(22,HIGH);

    server.begin();
    
    analogWrite(A3,1024);

     tiempoMuestreo4=10;
    
  
    Ethernet.begin(mac,ipArduino,gateway,subnet);

    IPAddress address = Ethernet.localIP();
    for (uint8_t i=0;i<=4;i++) {
        my_IP_address[i] = address[i];
        if(i!=4){
        Serial.print(my_IP_address[i]);
        }
        Serial.print(".");
    }
    Serial.println("");

   

    cont1=1000;
    delay(100);
    cont2=1000;
    delay(100);
    cont3=1000;
    delay(100);
    contador4=1000;
    delay(100);
    
    
    delay(100);



    
    api_status = Agentuino.begin();

   
    
    if (api_status == SNMP_API_STAT_SUCCESS) {

        Agentuino.onPduReceive(pduReceived);

        delay(10);

      

        return;
    }

    delay(10);

    

    
}

void loop() {

//SERVIDOR WEB

EthernetClient client = server.available();

 if (client) {
    // Si hay un cliente conectado
    if (client.connected()) {

       numAccesos++;
      // Crear la respuesta HTTP
     

           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Proyecto Gestion de Redes 2024</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Proyecto Gestion de Redes 2024</H1>");
           client.println("<hr />");
           client.println("");  
           client.println("<H2>Ardunio Gestionado</H2>");
           client.println("");  
           client.print("<H3>Numero de visitas: </H3>");
           client.println(numAccesos);
           
           
           client.println("");     
           client.println(""); 
           client.println("<p>Creado por Vicente Zamora. Contacto: trabajogredes2024@gmail.com</p>");  
           client.println(""); 
           client.println("</BODY>");
           client.println("</HTML>");
     
      // Esperar un poco antes de cerrar la conexión
      delay(10);
      
      // Cerrar la conexión
      client.stop();
    }
  }


 

     
    // Led encendidos o apagados

    // if(valorled1==1){
       
      
     //}else{
    //    digitalWrite(9,LOW);
    // }

     
     
   
    //Serial.println(tempArduino);
     
     
    // LECTURA DE PARÁMETROS
   
     Agentuino.listen();


   // FUNCIONES DE MUESTREO/LECTURA DE VARIABLES

    actualiza1();
    actualiza2();
    actualiza3();
    actualiza4();



    //COMPROBAMOS LOS POSIBLES TRAPS
    evento1();
    evento2();
    evento3();
    evento4();
    evento5();
    
    //Serial.println(ip1[0]);
    //delay(100);

   // COMPROBAMOS EVENTOS
     
   
     //float cable = (analogRead(1))*10;


       //Serial.println(cable);
    // FUNCION TRAPS SNMP
    
    //if ((cableant<10000)&& cable == 10230) {
      //  Serial.println("TRAP ENVIADO!");
        
        
        
      //  Agentuino.Trap("Cable desconectado!", RemoteIP, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
        
       // delay(1000);
       // locUpTime = locUpTime + 100;
       
    //}

 // VARIABLE SYSUPTIME, LA ACTUALIZAMOS!
    if (millis() - prevMillis > 1000) {

       
        prevMillis += 1000;

        
        locUpTime += 100 ;


       // CONTADOR DE LOS SENSORES!
        cont1=cont1-100;
        cont2=cont2-100;
        cont3=cont3-100;
        contador4=contador4-100;
    }

     //delay(tiempoMuestreo);
    
   
  
    
     //Serial.println(sensortemp);
     //Serial.println(locUpTime);

     
     //cableant = cable;



     


    
}


void actualiza1(void)
{    
    
   
    if(tiempoMuestreo1!=muestreo1Ant){
      muestreo1Ant=tiempoMuestreo1;
      cont1 = tiempoMuestreo1*100;
    }

    

    if(cont1==0){

      //SE LEEE EL VALOR Y SE ACTUALIZA EL TIEMPO

      int sensorVal = analogRead(5);
      float voltage = sensorVal * (5.0 / 1023.0); // Convertir el valor a voltaje
      sensortemp = ((voltage - 0.5) * 100.0)-12; // Convertir voltaje a temperatura en grados Celsius
      
      instanteLectura1 = locUpTime;
      cont1=tiempoMuestreo1*100;
      

      
    }

  

    
}



void actualiza2(void)
{
    if(tiempoMuestreo2!=muestreo2Ant){
      muestreo2Ant=tiempoMuestreo2;
      cont2 = tiempoMuestreo2*100;
    }

   

    if(cont2==0){

      //SE LEEE EL VALOR Y SE ACTUALIZA EL TIEMPO
      sensorhum= capSensor.capacitiveSensor(30)/100;
      //Serial.println(sensorhum);
      instanteLectura2 = locUpTime;
      cont2=tiempoMuestreo2*100;
      

      
    }

  

    
}


void actualiza3(void)
{
    if(tiempoMuestreo3!=muestreo3Ant){
      muestreo3Ant=tiempoMuestreo3;
      cont3 = tiempoMuestreo3*100;
    }

     

    if(cont3==0){

      //SE LEEE EL VALOR Y SE ACTUALIZA EL TIEMPO

  
      valorled1 = analogRead(2)*5.0/1023.0;
     
      instanteLectura3 = locUpTime;
      cont3=tiempoMuestreo3*100;
      

      
    }

  

    
}


void actualiza4(void)
{   
    if(tiempoMuestreo4!=muestreo4Ant){
      muestreo4Ant=tiempoMuestreo4;
      contador4 = tiempoMuestreo4*100;
      
    }

    


    if((contador4==0)){



       
      //SE LEEE EL VALOR Y SE ACTUALIZA EL TIEMPO
      numAccesosAnteriorMuestreo= tempArduino; //Almaceno el valor del muestreo anterior
      tempArduino = numAccesos;
      instanteLectura4 = locUpTime;
      contador4=tiempoMuestreo4*100;
      

      
    }

  

    
}






void evento1(void)
{   
    if(Sind1!=0){


       if(Sind1==4){
           if(((tempArduino-numAccesosAnteriorMuestreo)>uS1)||((tempArduino-numAccesosAnteriorMuestreo)<uI1)){
           if(activada==0){

          char str1[10]= "";
          char strcad[25]= "";
         sprintf(str1, "%d", *sensores[Sind1-1]);
          
         strcat(strcad, d1); // Concatenar cadena2 a cadena1
         strcat(strcad, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad, str1); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad , ip1, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada=1;
         
           }}else{
             activada = 0;
           
           }
        
       }else{
       //Serial.print(*sensores[Sind1-1]);
       if((*sensores[Sind1-1]>uS1)||(*sensores[Sind1-1]<uI1)){
         if(activada==0){

          char str1[10]= "";
          char strcad[25]= "";
         sprintf(str1, "%d", *sensores[Sind1-1]);
          
         strcat(strcad, d1); // Concatenar cadena2 a cadena1
         strcat(strcad, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad, str1); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad , ip1, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada=1;

       // GENERADOR DE LOGS
         unsigned long seconds = locUpTime / 100;
          unsigned long minutes = seconds / 60;
          unsigned long hours = minutes / 60;
         seconds %= 60;
          minutes %= 60;
         Serial.println(strings[Sind1-1]);
         Serial.print("En el instante: ");
         Serial.print(hours);
         Serial.print("horas, ");
         Serial.print(minutes);
         Serial.print("minutos, ");
         Serial.print(seconds);
         Serial.println("segundos");
         Serial.println(" ");
         
         
          //delay(1000);
         // locUpTime = locUpTime + 100;
         }
       
        
       }else{
        activada=0;
        }
       }
    }
}



void evento2(void)
{   
    if(Sind2!=0){


      if(Sind2==4){
           if(((tempArduino-numAccesosAnteriorMuestreo)>uS2)||((tempArduino-numAccesosAnteriorMuestreo)<uI2)){
           if(activada2==0){

          char str2[10]= "";
          char strcad2[25]= "";
         sprintf(str2, "%d", *sensores[Sind2-1]);
          
         strcat(strcad2, d2); // Concatenar cadena2 a cadena1
         strcat(strcad2, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad2, str2); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad2 , ip2, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada2=1;
         
           }}else{
             activada2 = 0;
           
           }
        
       }else{

       //Serial.print(*sensores[Sind2-1]);
       if((*sensores[Sind2-1]>uS2)||(*sensores[Sind2-1]<uI2)){
         if(activada2==0){

        char str2[10]= "";
          char strcad2[25]= "";
         sprintf(str2, "%d", *sensores[Sind2-1]);
          
         strcat(strcad2, d2); // Concatenar cadena2 a cadena1
         strcat(strcad2, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad2, str2); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad2, ip2, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada2=1;

         


         // GENERADOR DE LOGS
         unsigned long seconds = locUpTime / 100;
          unsigned long minutes = seconds / 60;
          unsigned long hours = minutes / 60;
         seconds %= 60;
          minutes %= 60;
         Serial.println(strings[Sind2-1]);
         Serial.print("En el instante: ");
         Serial.print(hours);
         Serial.print("horas, ");
         Serial.print(minutes);
         Serial.print("minutos, ");
         Serial.print(seconds);
         Serial.print("segundos");
         Serial.println(" ");

         
          //delay(1000);
         // locUpTime = locUpTime + 100;
         }
       
        
       }else{
        activada2=0;
        }

    }
}
}



void evento3(void)
{   
    if(Sind3!=0){
        if(Sind3==4){
           if(((tempArduino-numAccesosAnteriorMuestreo)>uS3)||((tempArduino-numAccesosAnteriorMuestreo)<uI3)){
           if(activada3==0){

          char str3[10]= "";
          char strcad3[25]= "";
         sprintf(str3, "%d", *sensores[Sind3-1]);
          
         strcat(strcad3, d3);// Concatenar cadena2 a cadena1
         strcat(strcad3, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad3, str3); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad3 , ip3, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada3=1;

         
         // GENERADOR DE LOGS
         unsigned long seconds = locUpTime / 100;
          unsigned long minutes = seconds / 60;
          unsigned long hours = minutes / 60;
         seconds %= 60;
          minutes %= 60;
         Serial.println(strings[Sind3-1]);
         Serial.print("En el instante: ");
         Serial.print(hours);
         Serial.print("horas, ");
         Serial.print(minutes);
         Serial.print("minutos, ");
         Serial.print(seconds);
         Serial.print("segundos");
         Serial.println(" ");
          //delay(1000);
         // locUpTime = locUpTime + 100;
         
         
           }}else{
             activada3 = 0;
           
           }
        
       }else{
       //Serial.print(*sensores[Sind3-1]);
       if((*sensores[Sind3-1]>uS3)||(*sensores[Sind3-1]<uI3)){
         if(activada3==0){
          

        char str3[10]= "";
          char strcad3[25]="";
         sprintf(str3, "%d", *sensores[Sind3-1]);
          
         strcat(strcad3, d3); // Concatenar cadena2 a cadena1
         strcat(strcad3, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad3, str3); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad3, ip3, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada3=1;




         // GENERADOR DE LOGS
         unsigned long seconds = locUpTime / 100;
          unsigned long minutes = seconds / 60;
          unsigned long hours = minutes / 60;
         seconds %= 60;
          minutes %= 60;
         Serial.println(strings[Sind3-1]);
         Serial.print("En el instante: ");
         Serial.print(hours);
         Serial.print("horas, ");
         Serial.print(minutes);
         Serial.print("minutos, ");
         Serial.print(seconds);
         Serial.print("segundos");
         Serial.println(" ");
          //delay(1000);
         // locUpTime = locUpTime + 100;
         }
       
        
       }else{
        activada3=0;
        }

    }
}
}


void evento4(void)
{   
    if(Sind4!=0){
            if(Sind4==4){
           if(((tempArduino-numAccesosAnteriorMuestreo)>uS4)||((tempArduino-numAccesosAnteriorMuestreo)<uI4)){
           if(activada4==0){

          char str4[10]= "";
          char strcad4[25]= "";
         sprintf(str4, "%d", *sensores[Sind4-1]);
          
         strcat(strcad4, d4); // Concatenar cadena2 a cadena1
         strcat(strcad4, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad4, str4); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad4 , ip4, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada4=1;
         
           }}else{
             activada4 = 0;
           
           }
        
       }else{
       //Serial.print(*sensores[Sind4-1]);
       if((*sensores[Sind4-1]>uS4)||(*sensores[Sind4-1]<uI4)){
         if(activada4==0){

         char str4[10]= "";
          char strcad4[25]="";
         sprintf(str4, "%d", *sensores[Sind4-1]);
          
         strcat(strcad4, d4); // Concatenar cadena2 a cadena1
         strcat(strcad4, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad4, str4); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad4 , ip4, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada4=1;


         // GENERADOR DE LOGS
         unsigned long seconds = locUpTime / 100;
          unsigned long minutes = seconds / 60;
          unsigned long hours = minutes / 60;
         seconds %= 60;
          minutes %= 60;
         Serial.println(strings[Sind4-1]);
         Serial.print("En el instante: ");
         Serial.print(hours);
         Serial.print("horas, ");
         Serial.print(minutes);
         Serial.print("minutos, ");
         Serial.print(seconds);
         Serial.print("segundos");
         Serial.println(" ");
          //delay(1000);
         // locUpTime = locUpTime + 100;
         }
       
        
       }else{
        activada4=0;
        }

    }
}
}

void evento5(void)
{   
    if(Sind5!=0){

       if(Sind5==4){
           if(((tempArduino-numAccesosAnteriorMuestreo)>uS5)||((tempArduino-numAccesosAnteriorMuestreo)<uI5)){
           if(activada5==0){

          char str5[10]= "";
          char strcad5[25]= "";
         sprintf(str5, "%d", *sensores[Sind5-1]);
          
         strcat(strcad5, d5); // Concatenar cadena2 a cadena1
         strcat(strcad5, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad5, str5); // Concatenar cadena2 a cadena1
         
         Agentuino.Trap(strcad5 , ip5, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada5=1;
         
           }}else{
             activada5 = 0;
           
           }
        
       }else{
    

       //Serial.print(*sensores[Sind5-1]);
       if((*sensores[Sind5-1]>uS5)||(*sensores[Sind5-1]<uI5)){
         if(activada5==0){
         char str5[10]= "";
          char strcad5[25]="";
         sprintf(str5, "%d", *sensores[Sind5-1]);
          
         strcat(strcad5, d5); // Concatenar cadena2 a cadena1
         strcat(strcad5, " Valor: "); // Concatenar cadena2 a cadena1
         strcat(strcad5, str5); // Concatenar cadena2 a cadena1
         Agentuino.Trap(strcad5 , ip5, locUpTime, "1.3.6.1.4.1.36582.1.1.1.4.1","1.3.6.1.4.1.36582.1.1.1.4.1");
         activada5=1;


         // GENERADOR DE LOGS
         unsigned long seconds = locUpTime / 100;
          unsigned long minutes = seconds / 60;
          unsigned long hours = minutes / 60;
         seconds %= 60;
          minutes %= 60;
         Serial.println(strings[Sind5-1]);
         Serial.print("En el instante: ");
         Serial.print(hours);
         Serial.print("horas, ");
         Serial.print(minutes);
         Serial.print("minutos, ");
         Serial.print(seconds);
         Serial.print("segundos");
         Serial.println(" ");
          //delay(1000);
         // locUpTime = locUpTime + 100;
         }
       
        
       }else{
        activada5=0;
        }

    }
}
}

void addString(const char* str) {
    if (num_strings < 10) {
        // Asignar memoria para el nuevo string y copiar el contenido
        strings[num_strings++] = strdup(str);
    } 
}
