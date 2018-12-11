#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_credentials.h> 

String USERNAME = "Trojan52/";
String PREAMBLE = "feeds/";
String T_COMMAND = "kitchen.udlinnitiel";
#define LEDBLUE 2
#define RELAY   4

WiFiClient WiFiClient;

PubSubClient client(WiFiClient);

void setup() {
  HTTPClient http;
  pinMode(LEDBLUE, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  digitalWrite(LEDBLUE, HIGH);
  Serial.begin(9600);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LEDBLUE, LOW);
    delay(500);
    digitalWrite(LEDBLUE, HIGH);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);

  String Url = "http://io.adafruit.com/api/v2/" + USERNAME + PREAMBLE + T_COMMAND + "/data/retain/?X-AIO-Key=" MQTT_KEY;
  http.begin(Url);
  int httpCode = http.GET();
  String payload = http.getString();
  int pos = payload.indexOf(',');
  if(pos != -1) {
    String val = payload.substring(0, pos);
    if(val.equals("ON")) {
       digitalWrite(RELAY, HIGH);
    } else {
       digitalWrite(RELAY, LOW);
    }
  } else {
    digitalWrite(RELAY, HIGH);
  }
  http.end();
  client.setServer(SERVER, SERVERPORT);
  client.setCallback(callback);
}


void loop() {
  yield();
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    digitalWrite(LEDBLUE, LOW);
    if (client.connect("", MQTT_USERNAME, MQTT_KEY)) {
      digitalWrite(LEDBLUE, HIGH);
      Serial.println("connected");  
      String topic = USERNAME + PREAMBLE + T_COMMAND;
      client.subscribe(topic.c_str(), 1);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  client.loop();
}




void callback(char* topic, byte * data, unsigned int length) {

  Serial.print(topic);
  Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println();
  if (memcmp(data, "OFF", length) == 0)  {    
    digitalWrite(RELAY, LOW);
  } else if(memcmp(data, "ON", length) == 0){    
    digitalWrite(RELAY, HIGH);
  }
}
