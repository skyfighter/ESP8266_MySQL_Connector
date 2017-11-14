/* ******Note*******
    Read more https://github.com/ChuckBell/MySQL_Connector_Arduino/blob/master/extras/MySQL_Connector_Arduino_Reference_Manual.pdf

    Test Access DATABASE first >>> mysql -u**USER** -p**PASSWOED** –h**IP ADDRESS** --port=3306 <<<

  -----------Note for Me-----------------
  1. sudo nano /etc/mysql/my.cnf --> sudo service mysql restart
  2. CREATE USER 'USER'@'%' IDENTIFIED BY 'PASSWORD'; or Use PHPMyAdmin to Add user
  3. Use command 'SELECT user, host FROM mysql.user; and 'SELECT USER(), CURRENT_USER();' read more http://stackoverflow.com/questions/10299148/mysql-error-1045-28000-access-denied-for-user-billlocalhost-using-passw
  4. Test Use can connect to MySQL Databases
*/
#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
int16_t adc0, adc1, adc2, adc3;
float vcc_pv, amp_pv, watt_pv, vcc_batt, amp_batt, watt_batt;

#include "DHT.h"
#define DHTPIN 2     // What pin we're connected to DHT
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22     // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);


byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//http://52.220.88.45/
IPAddress server_addr(, , , );  // IP of the MySQL *server* here
char user[] = "read";                        // MySQL user login username
char pass[] = "hems";                        // MySQL user login password


char INSERT_DATA[] = "INSERT INTO Test.pv (V_pv,A_pv,W_pv,V_batt,A_batt,W_batt) VALUES (%s,%s,%s,%s,%s,%s); INSERT INTO Test.temp (temp,hum) VALUES (%s,%s);";
char query[150];
char v_pv[5];
char a_pv[5];
char w_pv[5];
char v_batt[5];
char a_batt[5];
char w_batt[5];

char temperature[5];
char humidity[5];



const char* ssid = "Home";             //SSID WiFi name
const char* password = "home391418";        //Password WiFi
WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(115200);
  delay(10);

  ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  ads.begin();


  WiFi.begin(ssid, password);
  Serial.println("Connected to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("IP Address");
  Serial.print(WiFi.localIP());

  Serial.println("Connecting..SQL Server..");
  if (conn.connect(server_addr, 3306, user, pass)) {  //Mysql port 3306.
    delay(1000);

    Serial.println("Recording data...");

  }
  else
    Serial.println("Connection failed.");
  //conn.close(); //ไม่ได้สสั่งผิด conn จึงเหมือนกับ ระะบบติดต่อ database ตลองเวลา
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  vcc_pv = ((adc0 * 0.00003125) + (((adc0 * 0.00003125) / 51.25e3) * 0.98e6)) * 1.003184713375796*1.016768292682927;
  amp_pv = (((adc1 + 5) * 0.00003125) / 0.1);
  watt_pv = vcc_pv * amp_pv;

  vcc_batt = ((adc2 * 0.00003125) + (((adc2 * 0.00003125) / 51.25e3) * 0.98e6)) * 1.003184713375796*0.879020979020979;
  amp_batt = (((adc3 + 5) * 0.00003125) / 0.1)*0.3712458286985539;
  watt_batt = vcc_batt * amp_batt;

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  Serial.print("Volt_pv: "); Serial.print(vcc_pv, 2);
  Serial.print("V.   AMP_pv: "); Serial.print(amp_pv, 5);
  Serial.print("A.   Watt_pv: "); Serial.print(watt_pv, 2);
  Serial.println("W. ");
  Serial.println("");

  Serial.print("Volt_batt: "); Serial.print(vcc_batt, 2);
  Serial.print("V.   AMP_batt: "); Serial.print(amp_batt, 5);
  Serial.print("A.   Watt_batt: "); Serial.print(watt_batt, 2);
  Serial.println("W. ");
  Serial.println("");

  // Save
  dtostrf(vcc_pv, 2, 2, v_pv);
  dtostrf(amp_pv, 1, 4, a_pv);
  dtostrf(watt_pv, 2, 2, w_pv);

  dtostrf(vcc_batt, 2, 2, v_batt);
  dtostrf(amp_batt, 1, 4, a_batt);
  dtostrf(watt_batt, 2, 2, w_batt);

  dtostrf(t, 1, 1, temperature);
  dtostrf(h, 1, 1, humidity);
  
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  sprintf(query, INSERT_DATA, v_pv, a_pv, w_pv, v_batt, a_batt, w_batt, temperature, humidity);
  // Execute the query
  //Serial.println(query);
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the
  delete cur_mem;
  delay(60000);
}
