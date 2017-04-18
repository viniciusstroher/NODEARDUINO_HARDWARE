/**
 * @example TCPClientMultiple.ino
 * @brief The TCPClientMultiple demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.02
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "ESP8266.h"

#define SSID        "Venizao"
#define PASSWORD    "venizao123"
#define HOST_NAME   "192.168.0.56"
#define HOST_PORT   (8090)

ESP8266 wifi(Serial1);

#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


int pinopir = 7; 
int retorno = 0;

void(* resetFunc) (void) = 0; 

void setup(void)
{
    Serial1.begin(115200);
    Serial.begin(9600);
    Serial.print("setup begin\r\n");

    Serial.print("FW Version: ");
    Serial.println(wifi.getVersion().c_str());
    
    
    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }

    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
    } else {
        Serial.print("Join AP failure\r\n");
        resetFunc();
    }
    
    if (wifi.enableMUX()) {
        Serial.print("multiple ok\r\n");
    } else {
        Serial.print("multiple err\r\n");
    }
    pinMode(pinopir, INPUT);
    pinMode(8, OUTPUT); 
    Serial.print("setup end\r\n");
}

void loop(void)
{
    uint8_t buffer[128] = {0};
    static uint8_t mux_id = 0;
    
    if (wifi.createTCP(mux_id, HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ");
        Serial.print(mux_id);
        Serial.println(" ok");


        int estado = analogRead(A5);  //Lê o valor fornecido pelo LDR  
   
        String paramsArduino = "";
        String luminosidade = "#luminosidade:"+String(estado)+"";
    
        dht.begin();
        float t = dht.readTemperature();
               
        String temperatura = "#temperatura:"+String(t)+"";
         
        retorno = digitalRead(pinopir);   
        String movimentacao = "#movimentacao:"+String(retorno);
      
        paramsArduino.concat(luminosidade);
        paramsArduino.concat(temperatura);
        paramsArduino.concat(movimentacao);
        
        char* params = new char[paramsArduino.length()+1];
        strncpy(params, paramsArduino.c_str(), paramsArduino.length()+1);
    
        if (wifi.send(mux_id, (const uint8_t*)params, strlen(params))) {
            Serial.println("send ok");
        } else {
            Serial.println("send err"); 
           
        }
        
        uint32_t len = wifi.recv(mux_id, buffer, sizeof(buffer), 10000);
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
     
        if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" err");
            
        }
        
        delay(1000);
    } else {
        Serial.print("create tcp ");
        Serial.print(mux_id);
        Serial.println(" err");
        resetFunc();
    }

}


