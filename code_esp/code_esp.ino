
#include <PubSubClient.h>
#include <ESP8266WiFi.h> //ESP8266
#include "http_server.hpp"
#include "ds18b20_config.hpp"
#include "wifi_config.hpp"


#define BAUDE_RATE 9600

//credenciais ao broker no node-red
#define MQTT_ID "LBI"          // identificador para conexao mqtt
#define MQTT_USERNAME "teste"          // nome do broker 
#define MQTT_PASSWORD ""               // se houver senha cadastrada no broker
#define MQTT_PORT 1887                 // porta especifica para comunicacao do broker mqtt
#define MQTT_ENDERECO_IP "10.5.39.18"  // endereco de ip onde estiver rodando o node-red e thingsboard

//tópicos necessários para envio de dados via mqtt
// #define TOPICO_PUB_TEMPERATURA "device/temperatura"
// #define TOPICO_PUB_UMIDADE "device/umidade"
// #define TOPICO_PUB_PRESSAO "device/pressao" 
#define TOPICO_SUBS_NODE "datanode"
#define TOPICO_SUBS_TB "datatago"
#define TOPICO_SUBS_LED "LEDPLACA"


// endereço do thingsboard
char thingsboardServer[] = MQTT_ENDERECO_IP; 

WiFiClient wifiClient; //objeto para conexao ao thingsboard (servidor de arquivos)
WiFiClient nodeClient; //objeto para conexao ao node-red (servidor de automação)

//Objetos para conexao ao Thingsboard e Node-red
PubSubClient client(wifiClient);
PubSubClient mqtt_node(nodeClient);

// unsigned long lastSend;
// bool ledteste = false;

void setup()
{
    Serial.begin(BAUDE_RATE);
    pinMode(BUILTIN_LED, OUTPUT); 
    digitalWrite(LED_BUILTIN, 0); 
    SENSOR::iniciarSensor();
     delay(10);
    WIFICONFIG::conectarWifi();    
    client.setServer(thingsboardServer, 1883);
    mqtt_node.setServer(MQTT_ENDERECO_IP , MQTT_PORT);
    mqtt_node.setCallback(callback); // cadastro de tópicos para checagem. Ver funcao callback
    //lastSend = 0;    

    HTTPSERVER::configurarHttpServer(); // configura a pagina de OTA

    //ESP.wdtEnable(5000);
}

void loop()
{
    /*---------------------------------------------------------------*/
    /*checa e conecta ao Thingsboard e Node-red */
    //ESP.wdtFeed();
    if ((!client.connected()))
    {
      reconnect();
    }

    if ((!mqtt_node.connected()))
    {
      reconnect();
    }

    /*---------------------------------------------------------------*/
    
    client.loop();    // conexao do thingsboard
    mqtt_node.loop(); // conexao ao node-red

    HTTPSERVER::checarHttpServer(); //mantem o recurso OTA ativo na rede wifi em que estiver conectado

}

void getAndSendTemperatureAndHumidityData() // função para envio de dados ao Thingsboard
{
    String payload = SENSOR::formatarPayload();

    // Send payload
    char attributes[100];
    payload.toCharArray(attributes, 100);
    client.publish("v1/devices/me/telemetry", attributes); //regra para envio de payload para thingsboard
    Serial.println(attributes);
}

/* Funcao: envia os valores para o dashboard node-red*/
void send_data_nodered(void)
{
    // barramento.requestTemperatures();
    // float tempC = barramento.getTempC(sensor);
    //float tempC = 3.4;

    // envia os valores aquisitados através dos tópicos cadastrados no node-red
    //mqtt_node.publish(TOPICO_PUB_TEMPERATURA, String(tempC).c_str(), true);
}

void reconnect()
{
    // Loop until we're reconnected
     WIFICONFIG::conectarWifi();

    while ((!client.connected())||(!mqtt_node.connected()))
    {        
        //WIFICONFIG::conectarWifi();      
        
        Serial.print("Connecting to Thingsboard node ...");
        // Attempt to connect (clientId, username, password)

        if ((client.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD))&&(mqtt_node.connect(MQTT_ID, MQTT_USERNAME, MQTT_PASSWORD)))
        {
            Serial.println("[DONE]");
            digitalWrite(LED_BUILTIN, 1);
            //getAndSendTemperatureAndHumidityData(); // envia um payload ao broker assim que dispositivo é ligado
            //mqtt_node.subscribe(TOPICO_SUBS_LED); // topico de estado do led
            mqtt_node.subscribe(TOPICO_SUBS_TB); // topico que grava os dados em arquivo local do broker
            mqtt_node.subscribe(TOPICO_SUBS_NODE); // topico para envio de dados para o dashboard 
        }        

        else
        {
            SENSOR::aquisitarTemperaturaOffline();
            //Serial.print("[FAILED] [ rc = ");
            //Serial.print(client.state());
            //Serial.println(" : retrying in 5 seconds]");
            // Wait 5 seconds before retrying
            digitalWrite(LED_BUILTIN, 0);
            delay(100);
             HTTPSERVER::checarHttpServer(); //quando desconectado do broker, permite acesso ao OTA
        }
    }
}

/* Funcao: checa os tópicos enviados para o esp8266 para interação com o broker */
 void callback(String topic, byte* payload, unsigned int length) 
  {

    String messageTemp; 
    
    for (int i = 0; i < length; i++) 
      {       
        messageTemp += (char)payload[i];
      }  
  
    //Serial.print(messageTemp);
    //Serial.println();
    //Serial.print(topic);
    //Serial.println();

  /*topico que checa se o botao de teste do dashboard do led interno foi pressionado*/
    if(topic==TOPICO_SUBS_LED) 
        {  
          // if(messageTemp == "ligar" )
          //   {
          //     ledteste=!ledteste;
          //     digitalWrite(LED_BUILTIN, ledteste);
          //   }
            
          // else if(messageTemp == "desligar")
          //   {      
          //     //digitalWrite(LED_BUILTIN, HIGH);
          //   }
        }  
  
    /*recebe o topico que aciona a funcao de envio de valores para o Thingsboard*/
    else if(topic==TOPICO_SUBS_TB) 
        {  
          if(messageTemp == "send_data_tago" )
            {
              getAndSendTemperatureAndHumidityData();
            }     
        }
  
    /*recebe o topico que aciona a funcao de envio de valores para o dashboard no node-red*/
    else if(topic==TOPICO_SUBS_NODE) 
        {  
          if(messageTemp == "send_data_node" )
            {
              send_data_nodered();            
            }     
        }       
  }
