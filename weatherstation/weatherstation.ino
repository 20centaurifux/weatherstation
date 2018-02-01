#include <WeatherSensors.h>
#include <WeatherLog.h>
#include <Event.h>

#define DHT11_DIO  12
#define UV_PIN     A0
#define REF_3V_PIN A1

#define MEASURE_INTERVAL  10000ul
#define TRANSMIT_INTERVAL 30000ul

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN);
WeatherLog<512> weatherLog(MEASURE_INTERVAL / 1000);

EventLoop<2> events;

class Measure : public EventCallback
{
  public:
    Measure()
    {
      clear(); 
    }
 
    unsigned long operator()()
    {
      Serial.println("MEASURE");

      _success = sensors.measure(_value);

      return 0;
    }

    void clear()
    {
      _success = false;  
    }
    
    inline bool success() { return _success; }

    inline LogValue& value() { return _value; }

  private:
    LogValue _value;
    bool _success;
};

Measure measureEvent;

class Transmit: public EventCallback, public ProcessLogValue
{
  public: 
    unsigned long operator()()
    {
      Serial.println("TRANSMIT");

      weatherLog.forEach(*this);

      return TRANSMIT_INTERVAL;
    }

    void operator()(const LogValue& value)
    {
      Serial.print("TIMESTAMP: ");
      Serial.println(LOG_VALUE_DECODE_TIMESTAMP(value));
      Serial.print("TEMP: ");
      Serial.println(LOG_VALUE_DECODE_TEMPERATURE(value));
      Serial.print("PRESSURE: ");
      Serial.println(LOG_VALUE_DECODE_PRESSURE(value));
      Serial.print("HUMIDITY: ");
      Serial.println(LOG_VALUE_DECODE_HUMIDITY(value));
      Serial.print("UV: ");
      Serial.println(LOG_VALUE_DECODE_UV(value));
    }
};

Transmit transmitEvent;

void setup()
{
  Serial.begin(9600);

  sensors.begin();

  events.timeout(&measureEvent, 0);
  events.timeout(&transmitEvent, TRANSMIT_INTERVAL);
}

void loop()
{
  events.iteration();

  if(measureEvent.completed())
  {
    if(measureEvent.success())
    {
      LogValue value = measureEvent.value();

      weatherLog.append(value);
    }

    measureEvent.clear();

    events.timeout(&measureEvent, MEASURE_INTERVAL);
  }
  
  delay(500);
}
