/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
// #include <Servo.h>

// Configurations
const char* ssid = "Redmi";                     // Wifi configuration - ssid
const char* password = "12345678";              // Wifi configuration - password
const char* mqtt_server = "18.194.121.190";     // MQTT configuration - broker ip/hostname
const char* access_subscriber = "hasAccess1";   // MQTT configuration - access topic to be subscribes i.e. hasAccess
const char* rfid_publisher = "userdata";        // MQTT configuration - data topic to publish rfid info i.e. userdata
const char* connection_to_server = "connnected";// MQTT configuration - topic to publish if connected to mqtt info i.e. connected

#define RST_PIN         0                     // RFID configuration
#define SS_PIN          2                     // RFID configuration 

WiFiClient espClient;
PubSubClient client(espClient);

MFRC522 mfrc522(SS_PIN, RST_PIN);             // Create MFRC522 instance

// Servo myservo;                             // Create Servo instance

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // receives 'yes' if user has access, 'no' if user doesnt have access
  if (payload == "yes") {
    Serial.println("Allow access to the user");
    // myservo.write(90);
    // delay(2000);
    // myservo.write(0);
  } else {
    Serial.println("Dont allow access");
    // myservo.write(0);
    // delay(1000);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(connection_to_server, clientId);
      // ... and resubscribe
      client.subscribe(access_subscriber);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // myservo.attach(4);
  // myservo.write(0);
  // delay(1000);
  // pinMode(16, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
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
    String readerWithRfid = String("reader1-") + rfidUid;
    Serial.println(readerWithRfid);
    // digitalWrite(16, HIGH);
    // delay(1000);                      // Wait for a second
    // digitalWrite(16, LOW);  // Turn the LED off by making the voltage HIGH
    // delay(1000); 
   
    client.publish(access_subscriber, readerWithRfid);
  }

}