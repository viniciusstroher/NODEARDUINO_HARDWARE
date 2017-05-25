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

//função de reset usada se der problema no esp8266 wifi
void(* resetFunc) (void) = 0; 

void setup(void)
{
    //set pino pir
    pinMode(7, INPUT);
    pinMode(6, INPUT);
    
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
            //regra shutdown
            if (strcmp ((char*)buffer,"shutdown_relays") == 0) {

                digitalWrite(10, HIGH); 
                delay(100);
                digitalWrite(11, HIGH); 
                delay(100);
                digitalWrite(12, HIGH);
                delay(100); 
            }
    
            if (strcmp ((char*)buffer,"up_app_relays") == 0) {
                digitalWrite(10, LOW);
                delay(100); 
                digitalWrite(11, LOW);
                delay(100); 
                digitalWrite(12, LOW);
                delay(100); 
            }

            if (strcmp ((char*)buffer,"abre_rele_luz1") == 0) {
                digitalWrite(10, LOW);
            }

            if (strcmp ((char*)buffer,"fecha_rele_luz1") == 0) {
                digitalWrite(10, HIGH); 
            }

            if (strcmp ((char*)buffer,"abre_rele_luz2") == 0) {
                digitalWrite(11, LOW);
            }

            if (strcmp ((char*)buffer,"fecha_rele_luz2") == 0) {
                digitalWrite(11, HIGH); 
            }

            if (strcmp ((char*)buffer,"abre_rele_luz3") == 0) {
                digitalWrite(12, LOW);
            }

            if (strcmp ((char*)buffer,"fecha_rele_luz3") == 0) {
                digitalWrite(12, HIGH); 
            }
           
         }else{

             String paramsArduino = "{ \"luminosidade\" : \"" +String(analogRead(A5))+"\","+
                                    "  \"luminosidade2\" : \""+String(analogRead(A6))+"\","+
                                    "  \"temperatura\" : \""  +String(dht.readTemperature())+"\","+
                                    "  \"temperatura2\" : \"" +String(dht2.readTemperature())+"\","+
                                    "  \"movimentacao\" : \"" +String(digitalRead(7))+"\","+
                                    "  \"movimentacao2\" : \""+String(digitalRead(6))+"\"}";
             
             const char* params = paramsArduino.c_str(); 
             
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

}

