#include <Adafruit_ADS1X15.h>
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

void ads_config(){
  ads.setGain(GAIN_TWOTHIRDS);
  if (!ads.begin())
  {
    if (ALLOW_DEBUG) Serial.println("*Failed to initialize ADS.");
    while (1);
  }
}
