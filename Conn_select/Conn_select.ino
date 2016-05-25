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

IPAddress server_addr(, , , ); // IP of the MySQL *server* here
char user[] = "***";                      // MySQL user login username
char pass[] = "***";                      // MySQL user login password

char INSERT_SQL[] = "INSERT INTO test_arduino.hello_arduino (message) VALUES('Hello, Arduino!')";

const char* ssid = "***";           //SSID WiFi name
const char* password = "***";       //Password WiFi
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// Sample query
char query[] = "SELECT population FROM world.City WHERE name = 'New York'";

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  Serial.println("Connected to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.print("IP Address = ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting..SQL Server..");
  if (conn.connect(server_addr, 5106, user, pass)) {
    delay(1000);

    row_values *row = NULL;
    long head_count = 0;
    delay(1000);

    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Execute the query
    cur_mem->execute(query);
    // Fetch the columns (required) but we don't use them.
    column_names *columns = cur_mem->get_columns();
    // Read the row (we are only expecting the one)
    do {
      row = cur_mem->get_next_row();
      if (row != NULL) {
        head_count = atol(row->values[0]);
      }
    } while (row != NULL);
    // Deleting the cursor also frees up memory used
    delete cur_mem;
    // Show the result
    Serial.print(" NYC pop = ");
    Serial.println(head_count);

  }
  else
    Serial.println("Connection failed.");
  conn.close();
}

void loop() {

}
