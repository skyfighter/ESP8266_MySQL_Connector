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

const char* ssid = "--SSID--";             //SSID WiFi name
const char* password = "--password--";        //Password WiFi
WiFiClient client;
MySQL_Connection conn((Client *)&client);

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
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * ( 1 / 1023.0);
  float amp = voltage * (30);
  float watt = amp * 220;

  Serial.print("V: ");
  Serial.print(voltage);
  Serial.print(" %\t");
  Serial.print("Watt: ");
  Serial.println(watt);


  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Save
  dtostrf(watt, 1, 1, w);
  sprintf(query, INSERT_DATA, w);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the
  delete cur_mem;
  delay(5000);
}
