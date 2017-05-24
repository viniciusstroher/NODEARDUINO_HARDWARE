#include <MemoryFree.h>

#include "ESP8266.h"
//configurações do wifi
//#define   SSID        "COBRE"
//#define   PASSWORD    "robotica"
#define SSID        "Venizao"
#define PASSWORD    "venizao123"

#define HOST_NAME   "192.168.0.20"
#define HOST_PORT   (8090)
//configurações do wifi

ESP8266 wifi(Serial1);
static uint8_t mux_id = 0;

//DECLARACAO DOS DHT
#include "DHT.h"
#define DHTPIN  2
#define DHTPIN2 3
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
//DECLARACAO DOS DHT

int pinopir  = 7; 
int pinopir2 = 6; 

//função de reset usada se der problema no esp8266 wifi
void(* resetFunc) (void) = 0; 

void setup(void)
{
    //set pino pir
    pinMode(pinopir, INPUT);
    pinMode(pinopir2, INPUT);
    
    //set pino rele
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT); 
    pinMode(12, OUTPUT); 
 
    //se baudrate do wifi
    Serial1.begin(115200);
    Serial.begin(9600);
    
    //set wifi para modo estaçãp
    wifi.setOprToStation();
    
    //conecta wifi
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.println(wifi.getLocalIP().c_str());
        if(!wifi.enableMUX()){
           resetFunc();
        }
    } else {
        resetFunc();
    }
    dht.begin();
    dht2.begin();
    delay(1000);
}

void loop()
{
    
    //conecta ao servidor
    if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
      
         uint8_t buffer[128]  = {0};
         uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 500);         
      
         if (len > 0) {
             const char* str  = (char*)buffer;
        
            //regra shutdown
            if (strcmp (str,"shutdown_relays") == 0) {

                digitalWrite(10, HIGH); 
                delay(200);
                digitalWrite(11, HIGH); 
                delay(200);
                digitalWrite(12, HIGH);
                delay(200); 
            }
    
            if (strcmp (str,"up_app_relays") == 0) {
                digitalWrite(10, LOW);
                delay(200); 
                digitalWrite(11, LOW);
                delay(200); 
                digitalWrite(12, LOW);
                delay(200); 
            }

            if (strcmp (str,"abre_rele_luz1") == 0) {
                digitalWrite(10, LOW);
            }

            if (strcmp (str,"fecha_rele_luz1") == 0) {
                digitalWrite(10, HIGH); 
            }

            if (strcmp (str,"abre_rele_luz2") == 0) {
                digitalWrite(11, LOW);
            }

            if (strcmp (str,"fecha_rele_luz2") == 0) {
                digitalWrite(11, HIGH); 
            }

            if (strcmp (str,"abre_rele_luz3") == 0) {
                digitalWrite(12, LOW);
            }

            if (strcmp (str,"fecha_rele_luz3") == 0) {
                digitalWrite(12, HIGH); 
            }
           
         }else{

              String paramsArduino = "{ \"luminosidade\" : \"" +String(analogRead(A5))+"\","+
                                    "  \"luminosidade2\" : \""+String(analogRead(A6))+"\","+
                                    "  \"temperatura\" : \""+String(dht.readTemperature())+"\","+
                                    "  \"temperatura2\" : \""+String(dht2.readTemperature())+"\","+
                                    "  \"movimentacao\" : \""+String(digitalRead(pinopir))+"\","+
                                    "  \"movimentacao2\" : \""+String(digitalRead(pinopir2))+"\"}";

             const char* params = paramsArduino.c_str(); 
             paramsArduino = "";
             if (!wifi.send(mux_id, (const uint8_t*)params, strlen(params))) {
                resetFunc();             
             }
             
         }
        
         //desconecta do servidor
         if (!wifi.releaseTCP(mux_id)) {
          resetFunc();
         }
         
     } else {
        resetFunc();
     }
     freeMemory();
     delay(1000);
}

