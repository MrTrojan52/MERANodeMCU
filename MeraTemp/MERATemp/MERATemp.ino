#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_credentials.h> 
#include <DHT.h>

#define USERNAME          "Trojan52/"
#define PREAMBLE          "feeds/"
#define T_TEMP            "zal.tiempieratura"
#define T_HUMID           "zal.vlazhnost"
#define LEDBLUE 2
#define DHT11_PIN 4
#define DHTTYPE DHT11

WiFiClient WiFiClient;

PubSubClient client(WiFiClient);

double last_temp = 0.0;
double last_humid = 0.0;

DHT dht(DHT11_PIN, DHTTYPE);
void setup() {

  pinMode(LEDBLUE, OUTPUT);
  digitalWrite(LEDBLUE, HIGH);
  Serial.begin(9600);
  dht.begin();
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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  } else {
    char buf[10];
    double cur_temp = dht.readTemperature();
    double cur_humid = dht.readHumidity();
    if(last_temp != cur_temp) {
      sprintf(buf, "%.1lf", cur_temp);
      client.publish(USERNAME PREAMBLE T_TEMP, buf);
      last_temp = cur_temp;
    }
    if(last_humid != cur_humid) {
      sprintf(buf, "%.1lf", cur_humid);
      client.publish(USERNAME PREAMBLE T_HUMID, buf);
      last_humid = cur_humid;
    }
    delay(10000);
  }
  
  delay(1000);
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
