#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>

namespace WIFICONFIG
{

    // conexao à rede wifi
#define WIFI_AP "IPT-IoT"
#define WIFI_PASSWORD "r@cion@l"

//Static IP address configuration
IPAddress staticIP(10,56,56,88); //ESP static ip
IPAddress gateway(10,56,56,1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
//IPAddress dns(10,11,39,5);  //DNS

    int status = WL_IDLE_STATUS;

    void conectarWifi()
    {
       
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Connecting to AP ...");
            // attempt to connect to WiFi network
            
             WiFi.mode(WIFI_AP_STA);
             WiFi.config(staticIP, gateway,subnet);
            WiFi.begin(WIFI_AP, WIFI_PASSWORD);
            
            while (WiFi.status() != WL_CONNECTED)
            {
                SENSOR::aquisitarTemperaturaOffline();
                digitalWrite(LED_BUILTIN, 0);
                delay(100);
            }
            Serial.println("Connected to AP");
            Serial.println(WiFi.localIP());
            
        }
    }
}