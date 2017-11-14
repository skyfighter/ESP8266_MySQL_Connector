#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

/*Sonoff Pinout
LED = 13 
Relay = 12
Option = 14
SW = 0 */
const int button = 0;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int relay =  12;      // the number of the relay pin



char sensor_no[3];
bool shouldSaveConfig = false;

WiFiClient client;
IPAddress server_addr(, , , );  // IP of the MySQL *server* here
char user[] = "read";                        // MySQL user login username
char pass[] = "hems";                        // MySQL user login password


//---- Senser -----
int sw_status = 0; //รับค่ามาจากปุ่มกดที่ module สำหรับส่งค่าไปยัง server
int s_no = 1; // ตำแหน่งของ sw 1-14

//------------SEND------------
char INSERT_DATA[] = "INSERT INTO Test.sw (s%d) VALUES (%d)";
char query[128];
int toggle_sw = 0;

//---------READ--------------

char SELECT_DATA[] = "SELECT s%d FROM Test.sw ORDER BY Timestamp DESC LIMIT 1";
char query_read[128];
int sw_read_out; // อ่านค่า status ของ sw จาก server


void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void toggle() {

  sw_status = !sw_read_out;
  //digitalWrite(5, sw_status);
  // Serial.print("Status change...> ");
  //  Serial.println(sw_status);
  toggle_sw = 1;

}


MySQL_Connection conn((Client *)&client);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  pinMode(relay, OUTPUT); 
  pinMode(ledPin, OUTPUT); 
  pinMode(button, INPUT);  
  attachInterrupt(button, toggle, RISING);

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(sensor_no, json["sensor_no"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read



  WiFiManagerParameter custom_sensor_no("sNo", "sensor no (1-14)", sensor_no, 5);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_sensor_no);

  //reset settings - for testing
  //wifiManager.resetSettings();

  if (!wifiManager.autoConnect("hems_config")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  strcpy(sensor_no, custom_sensor_no.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["sensor_no"] = sensor_no;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting..SQL Server..");
  if (conn.connect(server_addr, 3306, user, pass)) {  //Mysql port 3306.
    // delay(1000);

    Serial.println("Recording data...");
    toggle();
  }
  else
    Serial.println("Connection failed.");
  //conn.close(); //ไม่ได้สสั่งผิด conn จึงเหมือนกับ ระะบบติดต่อ database ตลองเวลา

}

void loop() {
  //Serial.println(sensor_no);
  s_no = atoi(sensor_no);

  Serial.println(s_no);
  if ( toggle_sw == 1) {
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

    sprintf(query, INSERT_DATA, s_no, sw_status);
    // Serial.println(query);
    // Execute the query
    cur_mem->execute(query);
    // Note: since there are no results, we do not need to read any data
    // Deleting the
    delete cur_mem;
    toggle_sw = 0;

  }

  row_values *row = NULL;
  // delay(1000);

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

  // Execute the query
  sprintf(query_read, SELECT_DATA, s_no);
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
  // Serial.print("Status senser S");
  // Serial.print(s_no);
  //  Serial.print(" = ");
  Serial.print("sw read from sql :");
    Serial.println(sw_read_out);

  if (sw_read_out == 1) {
    digitalWrite(ledPin, LOW);
    digitalWrite(relay, HIGH);
    Serial.println("SW-ON");
  }
  else {
    digitalWrite(ledPin, HIGH);
    digitalWrite(relay, LOW);
    Serial.println("SW-OFF");
  }


  //delay(1000);
}
