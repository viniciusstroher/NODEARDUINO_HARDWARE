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

const size_t sizeBuf = 228;
char szBuf[228];
char temp1[6],temp2[6];

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
 
}

void loop()
{
    
    //conecta ao servidor
    if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
      
         //uint8_t buffer[128]  = {0};
         uint8_t buffer[25]  = {0};
         uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 500);         
      
         if (len > 0) {
            //regra shutdown
            if (strcmp ((char*)buffer,"shutdown_relays") == 0) {

                digitalWrite(10, HIGH); 
              
                digitalWrite(11, HIGH); 
               
                digitalWrite(12, HIGH);

            }
    
            if (strcmp ((char*)buffer,"up_app_relays") == 0) {
                digitalWrite(10, LOW);
                
                digitalWrite(11, LOW);
                
                digitalWrite(12, LOW);
               
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
        
             /*String paramsArduino = "{ \"luminosidade\" : \"" +String(analogRead(A5))+"\","+
                                    "  \"luminosidade2\" : \""+String(analogRead(A6))+"\","+
                                    "  \"temperatura\" : \""  +String(dht.readTemperature())+"\","+
                                    "  \"temperatura2\" : \"" +String(dht2.readTemperature())+"\","+
                                    "  \"movimentacao\" : \"" +String(digitalRead(7))+"\","+
                                    "  \"movimentacao2\" : \""+String(digitalRead(6))+"\"}";
            */      

            

            /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
            dtostrf(dht.readTemperature(), 4, 2, temp1);
            dtostrf(dht2.readTemperature(), 4, 2, temp2);
                                                  
            Serial.println(dht.readTemperature());                                                                                    
            snprintf(szBuf, sizeBuf, "{ \"luminosidade\" : \" %d \", \"luminosidade2\" : \" %d \", \"temperatura\" : \"%s\", \"temperatura2\" : \"%s\", \"movimentacao\" : \" %d \"  \"movimentacao2\" : \" %d \"}", 
            analogRead(A5),
            analogRead(A6),
            temp1,
            temp2,
            digitalRead(7),
            digitalRead(6)
            );
            
             Serial.println(szBuf);
             if (!wifi.send(mux_id, (const uint8_t*)szBuf, strlen(szBuf))) {
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

