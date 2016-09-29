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



byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//http://52.220.88.45/
IPAddress server_addr(, , , );  // IP of the MySQL *server* here
char user[] = "read";                        // MySQL user login username
char pass[] = "hems";                        // MySQL user login password


char INSERT_DATA[] = "INSERT INTO Test.pv (V_pv,A_pv,W_pv,V_batt,A_batt,W_batt) VALUES (%s,%s,%s,%s,%s,%s)";
char query[128];
char v_pv[5];
char a_pv[5];
char w_pv[5];
char v_batt[5];
char a_batt[5];
char w_batt[5];




const char* ssid = "Home_F2";             //SSID WiFi name
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

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  vcc_pv = ((adc0 * 0.00003125) + (((adc0 * 0.00003125) / 51.25e3) * 0.98e6)) * 1.003184713375796;
  amp_pv = (((adc1 + 5) * 0.00003125) / 0.1);
  watt_pv = vcc_pv * amp_pv;

  vcc_batt = ((adc2 * 0.00003125) + (((adc2 * 0.00003125) / 51.25e3) * 0.98e6)) * 1.003184713375796;
  amp_batt = (((adc3 + 5) * 0.00003125) / 0.1);
  watt_batt = vcc_batt * amp_batt;

  Serial.print("V_pv: "); Serial.println(vcc_pv, 2);
  Serial.print("AMP_pv: "); Serial.println(amp_pv, 5);
  Serial.print("watt_pv: "); Serial.println(watt_pv, 2);
  Serial.println(" ");


  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Save
  dtostrf(vcc_pv, 2, 2, v_pv);
  dtostrf(amp_pv, 1, 4, a_pv);
  dtostrf(watt_pv, 2, 2, w_pv);

  dtostrf(vcc_batt, 1, 1, v_batt);
  dtostrf(amp_batt, 1, 1, a_batt);
  dtostrf(watt_batt, 1, 1, w_batt);

  sprintf(query, INSERT_DATA, v_pv, a_pv, w_pv, v_batt, a_batt, w_batt);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the
  delete cur_mem;
  delay(5000);
}
