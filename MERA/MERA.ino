#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_credentials.h> 

#define USERNAME          "Trojan52/"
#define PREAMBLE          "feeds/"
#define T_CLIENTSTATUS    "clientStatus"
#define T_COMMAND         "kitchen.lamp-on-the-kitchen-1"
#define LEDBLUE 2

WiFiClient WiFiClient;

PubSubClient client(WiFiClient);

void setup() {

  pinMode(LEDBLUE, OUTPUT);
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
      client.subscribe(USERNAME PREAMBLE T_COMMAND, 1);
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
    digitalWrite(LEDBLUE, HIGH);
  } else if(memcmp(data, "ON", length) == 0){    
    digitalWrite(LEDBLUE, LOW);
  }
}
