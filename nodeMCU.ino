// library imports
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         0          // Configurable, see typical pin layout above
#define SS_PIN          2         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Application constants
const char ssid[] = "Xiaomi_F345";            // Wifi id
const char pass[] = "SriKrishna123$$";        // Wifi password
const char mqtt_broker_ip[] = "35.205.227.69" // MQTT broker IP
const char subsciber_topic = "hasaccess"      // subscribe to topic
const char publisher_topic = "userdata"       // publish to topic
const char reader_name = "reader1"            // provide a reader name to identify the reader. This name should be synced with registered users in server.

WiFiClient net;
MQTTClient client;

void setup() {
  Serial.begin(115200);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

//  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin(mqtt_broker_ip, net);
  client.onMessage(messageReceived);

  connect();
}

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(subsciber_topic);
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // receives 'yes' if user has access, 'no' if user doesnt have access
  if (payload == "yes") {
    Serial.println("Allow access to the user");
  } else {
    Serial.println("Dont allow access");
  }
}

void loop() {

  client.loop();
  delay(1000);
  
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (mfrc522.uid.uidByte[0]) {
   String rfidUid= String(mfrc522.uid.uidByte[0]) + String(mfrc522.uid.uidByte[1]) + String(mfrc522.uid.uidByte[2]) + String(mfrc522.uid.uidByte[3]);
   String readerWithRfid = reader_name + "-" + rfidUid
   Serial.println(readerWithRfid);
   client.publish(publisher_topic, readerWithRfid);
  }

}