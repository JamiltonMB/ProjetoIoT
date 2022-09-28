#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

WiFiClient ESPWiFiClient;
PubSubClient mqttClient(ESPWiFiClient);

const char* wifi_ssid = "imd0902";
const char* wifi_password = "imd0902iot";
int wifi_timeout = 200000;

const char* mqtt_broker = "broker.hivemq.com";
const int mqtt_port = 1883;
int mqtt_timeout = 10000;

int tringPin = 13;
int echoPin = 12;

#define SOUND_SPEED 0.034

long duracaoPulso;
float distanciaCm;

void connectMQTT() {
  Serial.print("Reconectando ao MQTT Broker..");

  unsigned long startTime = millis();
  while (!mqttClient.connected() && (millis() - startTime < mqtt_timeout)) {
      Serial.print(".");
      String clientId = "ESP32ClientHeitor-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println();
        Serial.print("Conectado ao broker MQTT!");
      }
      delay(100);
  }
  Serial.println();
}

void conecteWiFi(){
  WiFi.mode(WIFI_STA); //"station mode": permite o ESP32 ser um cliente da rede WiFi
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Conectando à rede WiFi .. ");

  unsigned long startTime = millis();
  
  while(WiFi.status() != WL_CONNECTED && (millis() - startTime < wifi_timeout)){
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Falhou!");
  }else{
    Serial.print("Conectado com o IP: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(tringPin, OUTPUT);
  pinMode(echoPin, INPUT);
  conecteWiFi();
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Conectando ao broker MQTT ..");
    mqttClient.setServer(mqtt_broker, mqtt_port);
  }
}

void loop() {
  if (!mqttClient.connected()){
    connectMQTT();
  }

  //Envio de sinal de tring
  digitalWrite(tringPin, LOW);
  delayMicroseconds(2);
  //Seta o pino Tring alto por 10ms
  digitalWrite(tringPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(tringPin, LOW);

  //Calcular o tempo que o pino fica alto em MS
  duracaoPulso = pulseIn(echoPin, HIGH);

  //Calcula a distancia
  distanciaCm = (duracaoPulso * SOUND_SPEED) / 2;
  Serial.print("distância é:");
  Serial.println(distanciaCm);

  if (mqttClient.connected()){
    mqttClient.loop();
    mqttClient.publish("/imd0902/T02/G4", String(distanciaCm).c_str(), true);
  }

  delay(1000);

  
}
