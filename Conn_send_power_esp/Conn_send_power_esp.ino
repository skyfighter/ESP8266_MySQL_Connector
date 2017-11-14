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

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//http://52.220.88.45/
IPAddress server_addr(, , , );  // IP of the MySQL *server* here
char user[] = "read";                        // MySQL user login username
char pass[] = "hems";                        // MySQL user login password


char INSERT_DATA[] = "INSERT INTO Test.power_esp (watt) VALUES (%s)";
char query[128];
char w[10];

const char* ssid = "Home_F2";             //SSID WiFi name
const char* password = "home391418";        //Password WiFi
WiFiClient client;
MySQL_Connection conn((Client *)&client);


const int sensorIn = A0;

double Voltage = 0;
double Amps = 0;
double Watt = 0;

float getVPP()
{
  float result;

  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1023;          // store min value here

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
  result = ((maxValue - minValue)) / 1023.0;

  return result;
}




void setup() {
  Serial.begin(115200);
  delay(10);

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
  
  Voltage = getVPP();
  Amps = (Voltage * 30);
  Watt = (Amps * 220);

  Serial.print("Vrms : ");
  Serial.print(Voltage,4);

  Serial.print("   Arms : ");
  Serial.print(Amps,4);

  Serial.print("   W : ");
  Serial.println(Watt,4);

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Save
  dtostrf(Watt, 1, 1, w);
  sprintf(query, INSERT_DATA, w);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the
  delete cur_mem;
  delay(5000);
}
