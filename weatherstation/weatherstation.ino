#include <WeatherLEDs.h>

#define LEDS_DS    7
#define LEDS_ST_CP 6
#define LEDS_SH_CP 5

WeatherLEDs leds = WeatherLEDs(LEDS_DS, LEDS_SH_CP, LEDS_ST_CP);

void setup()
{
  Serial.begin(9600);

  leds.begin();
}

bool backlight = false;

void loop()
{
  leds.backlight(backlight);
  leds.set(WEATHER_LED_TIME);
  delay(1000);

  leds.backlight(backlight);
  leds.set(WEATHER_LED_TEMPERATURE);
  delay(1000);

  leds.backlight(backlight);
  leds.set(WEATHER_LED_PRESSURE);
  delay(1000);

  leds.backlight(backlight);
  leds.set(WEATHER_LED_HUMIDITY);
  delay(1000);

  leds.backlight(backlight);
  leds.set(WEATHER_LED_UV);
  delay(1000);

  backlight = !backlight;
}
