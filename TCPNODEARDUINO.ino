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

int pinopir = 7; 
int retorno = 0;

void(* resetFunc) (void) = 0; 

void setup(void)
{
    pinMode(pinopir, INPUT);
    pinMode(8, OUTPUT); 
    
    Serial1.begin(115200);
    Serial.begin(9600);
    
    //wifi.setOprToStation();
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("CONECTADO ! IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
        wifi.enableMUX();
    } else {
        Serial.println("Reset arduino - join ap failure");
        resetFunc();
    }
    delay(1000);
    dht.begin();
}

String paramsArduino = "";
String luminosidade  = "";
String temperatura   = "";
String movimentacao  = "";
int estado           = 0;
float t              = 0;
uint8_t buffer[128]  = {0};
         
void loop(void)
{

    if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
   
         estado       =  analogRead(A5);  //Lê o valor fornecido pelo LDR  
         luminosidade = "#luminosidade:"+String(estado)+"";
         t            = dht.readTemperature();
         temperatura  = "#temperatura:"+String(t)+"";
         retorno      = digitalRead(pinopir);   
         movimentacao = "#movimentacao:"+String(retorno);
      
         paramsArduino.concat(luminosidade);
         paramsArduino.concat(temperatura);
         paramsArduino.concat(movimentacao);
         
         char* params = new char[paramsArduino.length()+1];
         strncpy(params, paramsArduino.c_str(), paramsArduino.length()+1);
        
         if (wifi.send(mux_id, (const uint8_t*)params, strlen(params))) {
           Serial.println("Enviando: "+paramsArduino);
         }
         paramsArduino = "";
         
         uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 100);         
         if (len > 0) {
            String str = (char*)buffer; 
            if(str.equals("liga_luz")){
                Serial.println("LIGANDO A LUZ");
                digitalWrite(8, LOW);
            }
            if(str.equals("desliga_luz")){
                Serial.println("DESLIGANDO A LUZ");
                digitalWrite(8, HIGH); 
            }
         }
      
         if (!wifi.releaseTCP(mux_id)) {
          Serial.println("Reset arduino - release tcp");
          resetFunc();
         }
     } else {
        Serial.println("Reset arduino - tcp not create");
        resetFunc();
     }
}


