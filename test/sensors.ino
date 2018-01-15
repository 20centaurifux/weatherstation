#include <WeatherSensors.h>

#define DHT11_DIO  12
#define UV_PIN     A0
#define REF_3V_PIN A1

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN);

void setup()
{
  Serial.begin(9600);

  sensors.begin();
}

void loop()
{
  LogValue v;

  sensors.measure(v);

  Serial.print("TIMESTAMP: ");
  Serial.println(LOG_VALUE_DECODE_TIMESTAMP(v));
  Serial.print("TEMP: ");
  Serial.println(LOG_VALUE_DECODE_TEMPERATURE(v));
  Serial.print("PRESSURE: ");
  Serial.println(LOG_VALUE_DECODE_PRESSURE(v));
  Serial.print("HUMIDITY: ");
  Serial.println(LOG_VALUE_DECODE_HUMIDITY(v));
  Serial.print("UV: ");
  Serial.println(LOG_VALUE_DECODE_UV(v));

  
  
  delay(5000);
}
