/* ******Note*******
    Read more https://github.com/ChuckBell/MySQL_Connector_Arduino/blob/master/extras/MySQL_Connector_Arduino_Reference_Manual.pdf

    Test Access DATABASE first >>> mysql -u**USER** -p**PASSWOED** –h**IP ADDRESS** --port=3306 <<<

  ----------MySQL Command --------
  SELECT *  FROM sw ORDER BY Timestamp DESC LIMIT 1; // Select the last record from MySQL table using SQL syntax.



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

IPAddress server_addr(52, 220, 88, 45);  // IP of the MySQL *server* here
char user[] = "conn";                        // MySQL user login username
char pass[] = "tum354527";                        // MySQL user login password

// IN & OUT PUT




//---- Senser -----
int sw_status = 0; //รับค่ามาจากปุ่มกดที่ module สำหรับส่งค่าไปยัง server
int senser_no = 1; // ตำแหน่งของ sw 1-14

//------------SEND------------
char INSERT_DATA[] = "INSERT INTO Test.sw (s%d) VALUES (%d)";
char query[128];
int toggle_sw = 0;

//---------READ--------------

char SELECT_DATA[] = "SELECT s%d FROM Test.sw ORDER BY Timestamp DESC LIMIT 1";
char query_read[128];
int sw_read_out; // อ่านค่า status ของ sw จาก server


const char* ssid = "iVPN";             //SSID WiFi name
const char* password = "tum354527";        //Password WiFi
WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT); // (PIN D1)  LED
  pinMode(13, INPUT);  // (PIN D7) SW
  //delay(10);

  attachInterrupt(13, toggle, RISING);



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
    toggle();
  }
  else
    Serial.println("Connection failed.");
  //conn.close(); //ไม่ได้สสั่งผิด conn จึงเหมือนกับ ระะบบติดต่อ database ตลองเวลา
}

void toggle() {
  sw_status = !sw_status;
  digitalWrite(5, sw_status);
  Serial.print("Status change...> ");
  Serial.println(sw_status);
  toggle_sw = 1;

}

void loop() {
  
      row_values *row = NULL;
     // delay(1000);

      // Initiate the query class instance
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

      // Execute the query
      sprintf(query_read,SELECT_DATA,senser_no);
      cur_mem->execute(query_read);
      // Fetch the columns (required) but we don't use them.
      column_names *columns = cur_mem->get_columns();
      // Read the row (we are only expecting the one)
      do {
        row = cur_mem->get_next_row();
        if (row != NULL) {
          sw_read_out = atol(row->values[0]);
        }
      } while (row != NULL);
      // Deleting the cursor also frees up memory used
      delete cur_mem;
      // Show the result
      Serial.print("Status senser S");
      Serial.print(senser_no);
      Serial.print(" = ");
      Serial.println(sw_read_out);

      if (sw_read_out == 1){
        digitalWrite(5, HIGH);
           }
      else digitalWrite(5, LOW);


  
  if ( toggle_sw == 1) {
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

    sprintf(query, INSERT_DATA, senser_no, sw_status);
    Serial.println(query);
    // Execute the query
    cur_mem->execute(query);
    // Note: since there are no results, we do not need to read any data
    // Deleting the
    delete cur_mem;
    toggle_sw = 0;

  }
  /*

  */


  //delay(1000);
}
