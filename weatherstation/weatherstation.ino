#include <WeatherDisplay.h>

#define DISPLAY_CLK 3
#define DISPLAY_DIO 4

WeatherDisplay display = WeatherDisplay(DISPLAY_CLK, DISPLAY_DIO);

void setup()
{
  Serial.begin(9600);

  display.begin();
}

void loop()
{  
  for(int i = 0; i < 9999; ++i)
  {
    display.showNumber(i);
    delay(5);
  }

  display.showTime(13, 37);
  delay(2000);  
}
