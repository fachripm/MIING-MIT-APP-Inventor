//Node-red + DHT11 + ESP32 + MIT APP (esp32 > Mqtt(HiveMQ)) 
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define LedPin 2
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Giarts'24";                   // Giarts'24 fachripm
const char* password = "102210ferdi";              // 102210ferdi 12345678a
const char* mqtt_server   = "fd1de063f17441319d602d06ce46bcc2.s1.eu.hivemq.cloud";   // Mosquitto Server URL
const char* mqtt_user     = "mqtt_fachri";
const char* mqtt_password = "Mqttfachri2112";
const int mqtt_port = 8883;

 // set MQTT topic         
const char* mqtt_topic_lamp = "esp32/lamp";
const char* mqtt_topic_temp = "esp32/temp";  
const char* mqtt_topic_hum  = "esp32/hum";            

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long prevMillis = 0;

void setup_wifi(){ 
    delay(10);
    Serial.print("Connecting to " + String(ssid));
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print("."); }

    Serial.println(F("\nWiFi connected"));
    Serial.println("IP address: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to HiveMQ...");
    
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to HiveMQ!");
      client.subscribe(mqtt_topic_lamp);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {    
    Serial.begin(115200);
    dht.begin();
    setup_wifi(); 
    espClient.setInsecure();
    pinMode(LedPin, OUTPUT);
    digitalWrite(LedPin, LOW);
    client.setServer(mqtt_server, mqtt_port);    //server,port
    client.setCallback(callback);         // use if have callback
    client.setKeepAlive(60);
    randomSeed(micros());
}

void callback(char* topic, byte* payload, unsigned int length) { 

  String msg;
    for (int i = 0; i < length; i++) { msg += (char)payload[i];}

    if      (msg == "1"){ digitalWrite(LedPin, LOW); 
      Serial.println("LED OFF"); }    // LED On
    else if (msg == "2"){ digitalWrite(LedPin, HIGH); 
      Serial.println("LED ON"); }     // LED Off
    
    if (String(topic) == mqtt_topic_lamp) {
    Serial.println("LED msg : " + String(msg));
    }
}

void loop() {
  if(!client.connected()) { reconnect(); }
  client.loop();
  unsigned long currMillis = millis();

  if (currMillis - prevMillis > 5000) { // 1k = 1sec
    prevMillis = currMillis;

    float h = dht.readHumidity();         // read humidity
    float t = dht.readTemperature();      // read celcius

    // check for failed sensors
    if (isnan(h) || isnan(t) ) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    } 

    // Create MIT Payload JSON
    char temppayload[50];
    char humpayload[50];

    snprintf(temppayload, sizeof(temppayload),"%.2f", t);
    snprintf(humpayload, sizeof(humpayload),"%.2f", h);

    client.publish(mqtt_topic_temp, temppayload);
    client.publish(mqtt_topic_hum, humpayload);

    Serial.println("inject MIT APP : " +String(temppayload) +" || "+String(humpayload));

    //delay = 1000 ;
  } //millis end no need more delay 
}
