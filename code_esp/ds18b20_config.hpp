#include <OneWire.h>  
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

namespace SENSOR
{
    // configuração do sensor DS18B20
    OneWire pino(4); // D2
    DallasTemperature barramento(&pino);
    DeviceAddress sensor;
    String aquisitarTemperatura();
    void aquisitarTemperaturaOffline();

    String formatarPayload()
    {
        // Prepare a JSON payload string
    String payload = "{";
    payload += "\"temp\":";
    payload += aquisitarTemperatura();
    payload += "}";
        return payload;
    }

    void iniciarSensor()
    {
        barramento.begin();
        barramento.getAddress(sensor, 0);
    }

    String aquisitarTemperatura()
    {
        barramento.requestTemperatures();
        float tempC = barramento.getTempC(sensor);
        // float tempC = 20.4;
        String t = String(tempC);
        return t;
    }

    void aquisitarTemperaturaOffline()
    {
        barramento.requestTemperatures();
        float tempC = barramento.getTempC(sensor);
        // float tempC = 20.4;

        if (tempC > -8.0)
        {
           digitalWrite(LED_BUILTIN, 1);
                delay(50);
                digitalWrite(LED_BUILTIN, 0);
                delay(50);
        }
        
    }

}