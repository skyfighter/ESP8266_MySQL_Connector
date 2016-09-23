//http://henrysbench.capnfatz.com/henrys-bench/arduino-current-measurements/acs712-arduino-ac-current-tutorial/

const int sensorIn = A0;

double Voltage = 0;
double Amps = 0;
double Watt = 0;

void setup() {
  Serial.begin(9600);
}


float getVPP()
{
  float result;

  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(sensorIn);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      //record the maximum sensor value
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      //record the maximum sensor value
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue)) / 1024.0;

  return result;
}

void loop() {

  Voltage = getVPP();
  Amps = (Voltage * 30);
  Watt = (Amps * 220);

  Serial.print("Vrms : ");
  Serial.print(Voltage,4);

  Serial.print("   Arms : ");
  Serial.print(Amps,4);

  Serial.print("   W : ");
  Serial.println(Watt,4);

}
