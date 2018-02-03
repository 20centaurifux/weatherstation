#include <WeatherSensors.h>
#include <WeatherLog.h>
#include <WeatherLEDs.h>
#include <WeatherDisplay.h>
#include <InternalPullupButton.h>
#include <Event.h>

#define DHT11_DIO  12
#define UV_PIN     A0
#define REF_3V_PIN A1
#define LDR_PIN    A2

#define TM1637_CLK 3
#define TM1637_DIO 4

#define DS    7
#define ST_CP 6
#define SH_CP 5

#define BTN_SET 2

#define MEASURE_INTERVAL  60000ul
#define TRANSMIT_INTERVAL 120000ul
#define DISPLAY_INTERVAL  7500ul

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN, LDR_PIN);
WeatherLog<512> weatherLog(MEASURE_INTERVAL / 1000);
WeatherLEDs leds = WeatherLEDs(DS, SH_CP, ST_CP);
WeatherDisplay display = WeatherDisplay(TM1637_CLK, TM1637_DIO);

EventLoop<3> events;

class Measure : public EventCallback
{
  public: 
    unsigned long operator()()
    {
      leds.busy(true);

      LogValue value;
 
      if(sensors.measure(value))
      {
        weatherLog.append(value);
      }

      leds.busy(false);

      return MEASURE_INTERVAL;
    }
};

Measure measureEvent;

class Transmit: public EventCallback, public ProcessLogValue
{
  public: 
    unsigned long operator()()
    {
      leds.busy(true);
      weatherLog.forEach(*this);
      leds.busy(false);

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

#define DARK   250
#define BRIGHT 450

class DisplayLogValue : public EventCallback
{
  public: 
    unsigned long operator()()
    {
      unsigned long interval = DISPLAY_INTERVAL;

      if(!_state && !measure())
      {
        interval = 0;
      }
      else if(_state < 5)      
      {
        int light = sensors.light();

        setBacklight(light);
        setDisplayBrightness(light);        

        if(_state == 0)
        {
          showTime();
        }
        else if(_state == 1)
        {
          showTemperature(); 
        }
        else if(_state == 2)
        {
        }
        else if(_state == 3)
        {
          showPressure(); 
        }
        else
        {
          showUV();
        }

        ++_state;
      }
      else
      {
        interval = 0;
        off();
        _state = 0;
      }

      return interval;
    }

  private:
    uint8_t _state = 0;
    LogValue _value;

    void setBacklight(int light)
    {
      bool on = light <= DARK;

      leds.backlight(on);
    }

    void setDisplayBrightness(int light)
    {
      bool bright = light >= BRIGHT;

      display.bright(bright);
    }

    bool measure()
    {
      bool success;
      
      success = sensors.measure(_value);
      
      return success;
    }

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

      display.showTemperature(LOG_VALUE_DECODE_TEMPERATURE(_value));
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

InternalPullupButton btnSet(BTN_SET);

EventId displayEventId = 0;

void loop() 
{
  if(btnSet.pressed() && !displayEventId)
  {
    displayEventId = events.timeout(&displayEvent, 0);
  }

  if(displayEvent.completed())
  {
    displayEventId = 0; 
  }
  
  events.iteration();
  
  delay(250);
}
