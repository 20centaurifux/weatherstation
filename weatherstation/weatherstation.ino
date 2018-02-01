#include <WeatherSensors.h>
#include <WeatherLog.h>
#include <WeatherLEDs.h>
#include <WeatherDisplay.h>
#include <Event.h>

#define DHT11_DIO  12
#define UV_PIN     A0
#define REF_3V_PIN A1

#define TM1637_CLK 3
#define TM1637_DIO 4

#define DS    7
#define ST_CP 6
#define SH_CP 5

#define BTN_SET 2

#define MEASURE_INTERVAL  60000ul
#define TRANSMIT_INTERVAL 120000ul
#define DISPLAY_INTERVAL  10000ul

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN);
WeatherLog<512> weatherLog(MEASURE_INTERVAL / 1000);
WeatherLEDs leds = WeatherLEDs(DS, SH_CP, ST_CP);
WeatherDisplay display = WeatherDisplay(TM1637_CLK, TM1637_DIO);

EventLoop<3> events;

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

class DisplayLogValue : public EventCallback
{
  public: 
    unsigned long operator()()
    {
      unsigned long interval = DISPLAY_INTERVAL;

      switch(_state)
      {
        case 0:
          showTime();
          break;

        case 1:
          showTemperature();
          break;

        case 2:
          showPressure();
          break;

        case 3:
          showHumidity();
          break;

        case 4:
          showUV();
          break;

        default:
          interval = 0;
          off();
      }

      if(interval)
      {
        ++_state;
      }
      else
      {
        _state = 0;
      }

      return interval;
    }

    void setValue(LogValue value)
    {
      _value = value;
    }

  private:
    uint8_t _state = 0;
    LogValue _value;

    void showTime()
    {
      leds.set(WEATHER_LED_TIME);

      DateTime dt;

      if(sensors.now(dt))
      {
        display.showTime(dt.hour(), dt.minute());
      }
      else
      {
        display.showTime(0, 0);
      }
    }

    void showTemperature()
    {
      leds.set(WEATHER_LED_TEMPERATURE);

      display.showNumber(LOG_VALUE_DECODE_TEMPERATURE(_value));
    }

    void showPressure()
    {
      leds.set(WEATHER_LED_PRESSURE);

      display.showNumber(LOG_VALUE_DECODE_PRESSURE(_value));
    }

    void showHumidity()
    {
      leds.set(WEATHER_LED_HUMIDITY);

      display.showNumber(LOG_VALUE_DECODE_HUMIDITY(_value));
    }

    void showUV()
    {
      leds.set(WEATHER_LED_UV);

      display.showNumber(LOG_VALUE_DECODE_UV(_value));
    }

    void off()
    {
      leds.off();
      display.off();
    }
};

DisplayLogValue displayEvent;

void setup()
{
  Serial.begin(9600);

  sensors.begin();
  leds.begin();
  display.off();

  events.timeout(&measureEvent, 0);
  events.timeout(&transmitEvent, TRANSMIT_INTERVAL);
}

class InternalPullupButton
{
  public:
    InternalPullupButton(int pin) : _pin(pin)
    {
      pinMode(pin, INPUT_PULLUP);
      digitalWrite(pin, HIGH);

      _lastDebounce = 0;
      _pressed = false;
    }

    bool pressed()
    {
      bool pressed = digitalRead(_pin) == LOW;

      unsigned long interval = millis() - _lastDebounce;

      if(interval > 50)
      {
        _pressed = pressed;
      }

      return _pressed;
    }
    
  private:
    int _pin;
    unsigned long _lastDebounce;
    bool _pressed;
};

InternalPullupButton btnSet(BTN_SET);

EventId displayEventId = 0;
LogValue currentValue;

void loop()
{
  if(btnSet.pressed() && !displayEventId)
  {
    displayEvent.setValue(currentValue);
    displayEventId = events.timeout(&displayEvent, 0);
  }

  if(displayEvent.completed())
  {
    displayEventId = 0; 
  }
  
  events.iteration();

  if(measureEvent.completed())
  {
    if(measureEvent.success())
    {
      currentValue = measureEvent.value();

      weatherLog.append(currentValue);
    }

    measureEvent.clear();

    events.timeout(&measureEvent, MEASURE_INTERVAL);
  }
  
  delay(250);
}
