#include <EmonLib.h>

#include "ESP8266.h"

//configurações do wifi
#define SSID        "COBRE"
#define PASSWORD    "robotica"
#define HOST_NAME   "192.168.25.2"
#define HOST_PORT   (8090)
//configurações do wifi

ESP8266 wifi(Serial1);
static uint8_t mux_id = 0;

//DECLARACAO DOS DHT
#include "DHT.h"
#define DHTPIN 2
#define DHTPIN2 3
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
//DECLARACAO DOS DHT

EnergyMonitor emon1;  

int pinopir  = 7; 
int pinopir2 = 6; 

int retorno  = 0;
int retorno2 = 0;


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
        Serial.print("CONECTADO ! IP: ");       
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

//variaveis do escopo
String paramsArduino = "";

String luminosidade  = "";
String luminosidade2 = "";

String temperatura   = "";
String temperatura2  = "";

String movimentacao  = "";
String movimentacao2  = "";

uint8_t buffer[128];

int estado            = 0;
int estado2           = 0;

float t               = 0;
float t2              = 0;

char* params;
//variaveis do escopo

void loop(void)
{
    //conecta ao servidor
    if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
        
        buffer[128]  = {0};
         //recebe dados do servidor
         uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 1000);         
         if (len > 0) {
            //executa reles se vier comando do servidor
            String str = (char*)buffer;
            Serial.println("Recebido: "+str); 
           
            if(str.indexOf("abre_rele_luz1") != -1){
                Serial.println("LIGANDO A LUZ");
                digitalWrite(10, LOW);
            }
            if(str.indexOf("fecha_rele_luz1") != -1){
                Serial.println("DESLIGANDO A LUZ");
                digitalWrite(10, HIGH); 
            }

            if(str.indexOf("abre_rele_luz2") != -1){
                Serial.println("LIGANDO A LUZ 2");
                digitalWrite(11, LOW);
            }
            if(str.indexOf("fecha_rele_luz2") != -1){
                Serial.println("DESLIGANDO A LUZ 2");
                digitalWrite(11, HIGH); 
            }


            if(str.indexOf("abre_rele_luz3") != -1){
                Serial.println("LIGANDO A LUZ 3");
                digitalWrite(12, LOW);
            }
            if(str.indexOf("fecha_rele_luz3") != -1){
                Serial.println("DESLIGANDO A LUZ 3");
                digitalWrite(12, HIGH); 
            }
  
            if(str.indexOf("liga_ar_condicionado") != -1){
                Serial.println("Ligando Ar");
                
            }

             if(str.indexOf("liga_projetor") != -1){
                Serial.println("Ligando Projetor");
                
            }
            
            //executa reles se vier comando do servidor
            
         }else{

             paramsArduino = "";
             //pega dados dos sensores
             estado       =  analogRead(A5);  //Lê o valor fornecido pelo LDR  
             luminosidade = "{ \"luminosidade\" : \""+String(estado)+"\" , ";

             estado2        =  analogRead(A6);  //Lê o valor fornecido pelo LDR  
             luminosidade2 = " \"luminosidade2\" : \""+String(estado2)+"\" , ";
             
             t            = dht.readTemperature();
             temperatura  = "\"temperatura\" :\""+String(t)+"\" , ";

             t2            = dht2.readTemperature();
             temperatura   = "\"temperatura2\" :\""+String(t2)+"\" , ";
             
             retorno      = digitalRead(pinopir);   
             movimentacao = "\"movimentacao\" : \""+String(retorno)+"\" , ";
             
             retorno2      = digitalRead(pinopir2);   
             movimentacao2 = "\"movimentacao2\" : \""+String(retorno2)+"\" }";
            //pega dados dos sensores
             
             //cria dados para envio do servidor
             paramsArduino.concat(luminosidade);
             paramsArduino.concat(luminosidade2);
             paramsArduino.concat(temperatura);
             paramsArduino.concat(movimentacao);
             paramsArduino.concat(movimentacao2);
             
             
             params = new char[paramsArduino.length()+1];
             strncpy(params, paramsArduino.c_str(), paramsArduino.length()+1);
             //cria dados para envio do servidor

             //envia dados ao servidor
             if (wifi.send(mux_id, (const uint8_t*)params, strlen(params))) {
               Serial.println("Enviando: "+paramsArduino);
               
             }else{
               resetFunc();
             }
         }
        
         //desconecta do servidor
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


