#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
   ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.begin();
}

void loop(void)
{
  int16_t adc0, adc1, adc2, adc3;
  float vcc,amp,watt;
// V = I/R



  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  

  vcc = ((adc0 * 0.00003125)+(((adc0 * 0.00003125)/51.25e3)*0.98e6))*1.003184713375796;
  amp = (((adc1+5) * 0.00003125)/0.1);
  watt = vcc*amp;
  Serial.print("AIN0: "); Serial.print(adc0); Serial.print (" "); Serial.println (adc0 * 0.00003125,5);
  Serial.print("AIN1: "); Serial.print(adc1+5); Serial.print (" "); Serial.println ((adc1+5) * 0.00003125,5);
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.print("AIN3: "); Serial.println(adc3);
  Serial.print("VCC: "); Serial.println(vcc,2);
  Serial.print("AMP: "); Serial.println(amp,5);
  Serial.print("watt: "); Serial.println(watt,2);
  Serial.println(" ");

  delay(1000);
}
