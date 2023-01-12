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

        // Check if any reads failed and exit early (to try again).

        String t = String(tempC);
        return t;
    }

}