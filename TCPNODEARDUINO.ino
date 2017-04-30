#include <EmonLib.h>

#include "ESP8266.h"

#define SSID        "Venizao"
#define PASSWORD    "venizao123"
#define HOST_NAME   "192.168.0.56"
#define HOST_PORT   (8090)

ESP8266 wifi(Serial1);
static uint8_t mux_id = 0;

#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
EnergyMonitor emon1;  
int pinopir = 7; 
int pinopir2 = 6; 

int retorno = 0;

int retorno2 = 0;

void(* resetFunc) (void) = 0; 

void setup(void)
{
    pinMode(pinopir, INPUT);
    pinMode(8, OUTPUT); 
    
    Serial1.begin(115200);
    Serial.begin(9600);
    
    wifi.setOprToStation();
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("CONECTADO ! IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
        if(!wifi.enableMUX()){
           resetFunc();
        }
    } else {
        resetFunc();
    }
    
    dht.begin();
    emon1.current(A1, 60);
}

String paramsArduino = "";
String luminosidade  = "";
String temperatura   = "";
String movimentacao  = "";
String movimentacao2  = "";
String voltage       = "";
uint8_t buffer[128];
int estado           = 0;
float t              = 0;

char* params;    
void loop(void)
{
    if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
   
         estado       =  analogRead(A5);  //Lê o valor fornecido pelo LDR  
         luminosidade = "{ \"luminosidade\" : \""+String(estado)+"\" , ";
         t            = dht.readTemperature();
         temperatura  = "\"temperatura\" :\""+String(t)+"\" , ";
         
         retorno      = digitalRead(pinopir);   
         movimentacao = "\"movimentacao\" : \""+String(retorno)+"\" , ";
         
         retorno2      = digitalRead(pinopir2);   
         movimentacao2 = "\"movimentacao2\" : \""+String(retorno2)+"\"";

         
       /* emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
        emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
        
         float realPower       = emon1.realPower;        //extract Real Power into variable
         float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
         float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
         float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
         float Irms            = emon1.Irms;             //extract Irms into Variable
         String voltage = "realPower:"+String(realPower)+"apparentPower:"+String(apparentPower)+"powerFActor:"+String(powerFActor)+"supplyVoltage:"+String(supplyVoltage)+"Irms:"+String(Irms);
         */
         voltage = "}";
         paramsArduino.concat(luminosidade);
         paramsArduino.concat(temperatura);
         paramsArduino.concat(movimentacao);
         paramsArduino.concat(movimentacao2);
         paramsArduino.concat(voltage);
         
         params = new char[paramsArduino.length()+1];
         strncpy(params, paramsArduino.c_str(), paramsArduino.length()+1);
         
         
        buffer[128]  = {0};
         
         uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 1000);         
         if (len > 0) {
            String str = (char*)buffer;
            Serial.println("Recebido: "+str); 
            if(str.indexOf("abre_rele_luz") != -1){
                Serial.println("LIGANDO A LUZ");
                digitalWrite(8, LOW);
            }
            if(str.indexOf("fecha_rele_luz") != -1){
                Serial.println("DESLIGANDO A LUZ");
                digitalWrite(8, HIGH); 
            }
         }
        
         if (wifi.send(mux_id, (const uint8_t*)params, strlen(params))) {
           Serial.println("Enviando: "+paramsArduino);
           //wifi.enableMUX();
         }else{
           resetFunc();
         }
         paramsArduino = "";
         
         
      
         if (!wifi.releaseTCP(mux_id)) {
          Serial.println("release tcp");
          resetFunc();
         }
     } else {
        Serial.println(" tcp not create");
        resetFunc();
        
     }

     delay(2000);
}


