# ESP8266 MySQL Connector

  ใช้ MySQL Connector/Arduino เป็น Lib หลักการติดต่อข้อมูลระว่าง Arduino (Node MCU) และ MySQL โดยใช้งาน MySQL Connector 1.1.1a หรืออ่านได้จาก link นี้ https://github.com/ChuckBell/MySQL_Connector_Arduino
ซึ่งใน Lib ของ MySQL Connector/Arduino จะใช้ Arduino uno ร่วมกับ Ethernet shield หรือ Wi-Fi shield ในติดต่อกับ Network แต่ใน ESP8266 MySQL Connector จะใช้ NodeMCU V2 (ESP12-E) โดยติดต่อ Network ผ่าน Wi-Fi

## Conn send dth22
  เป็นตัวอย่างที่ใช้เซ็นเซอร์ วัดอุณหภูมิ และความชื้น (DTH22,DTH11) ส่งข้อมูลไปยัง MySQL

## Conn send data
  เป็นตัวอย่าส่งข้อความ จาก NodeMCU ข้อมูลไปยัง MySQL

## Conn select
  เป็นตัวอย่างอ่านข้อมูลจาก (หาข้อมูลอัตโนมัติ) MySQL แล้วจึงแสดงผลยัง Node MCU
