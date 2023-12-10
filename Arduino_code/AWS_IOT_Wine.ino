/*
  AWS IoT WiFi

  This sketch securely connects to an AWS IoT using MQTT over WiFi.
  It uses a private key stored in the ATECC508A and a public
  certificate for SSL/TLS authetication.

  It publishes a message every 5 seconds to arduino/outgoing
  topic and subscribes to messages on the arduino/incoming
  topic.

  The circuit:
  - Arduino MKR WiFi 1010 or MKR1000

  The following tutorial on Arduino Project Hub can be used
  to setup your AWS account and the MKR board:

  https://create.arduino.cc/projecthub/132016/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core-a9f365

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000

#include "arduino_secrets.h"

#include "DHT.h"
#define DHTPIN1 7     // Digital pin connected to the DHT sensor - 1층 온습도센서
#define DHTPIN3 6     // 3층 온습도 센서
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN1, DHTTYPE); 
DHT dht3(DHTPIN3, DHTTYPE); //3층 DHT 객체

#define LED_1_PIN 3 //히터
#define LED_3_PIN 1 //선풍기팬

int t1 = 4; // 1층 초음파 센서 핀 (trig)
int e1 = 5; // 1층 초음파 센서 핀 (echo)
int t3 = 8; // 3층 초음파 센서 핀 (trig)
int e3 = 9; // 3층 초음파 센서 핀 (echo)

#include <ArduinoJson.h>
#include "Led.h"

/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;

Led led1(LED_1_PIN);
Led led3(LED_3_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  dht.begin();
  dht3.begin();

  pinMode(e1, INPUT);
  pinMode(t1, OUTPUT);

  pinMode(e3, INPUT);
  pinMode(t3, OUTPUT);


  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  //
  // mqttClient.setId("clientId");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  // publish a message roughly every 5 seconds.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    char payload[512];
    getDeviceStatus(payload);
    //sendMessage(payload);
  }
}

unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("$aws/things/MyMy/shadow/update/delta");
}

void getDeviceStatus(char* payload) {
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  float t3 = dht3.readTemperature();
  float h3 = dht3.readHumidity();

  long duration1, distance1, duration3, distance3;
  digitalWrite(t1, HIGH);  // trigPin에서 초음파 발생(echoPin도 HIGH)        
  delayMicroseconds(10);
  digitalWrite(t1, LOW);
  duration1 = pulseIn(e1, HIGH);    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  distance1 = ((340 * duration1) / 1000) / 2;  //mm단위

  digitalWrite(t3, HIGH);  // trigPin에서 초음파 발생(echoPin도 HIGH)        
  delayMicroseconds(10);
  digitalWrite(t3, LOW);
  duration3 = pulseIn(e3, HIGH);    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  distance3 = ((340 * duration3) / 1000) / 2;  //mm단위

  // Read led status
  const char* led = (led1.getState() == LED_ON)? "ON" : "OFF";
  const char* ledd = (led3.getState() == LED_ON)? "ON" : "OFF";
  
  // 인벤토리 상태 초기화
  int inventoryStatus = 100; // 기본적으로 인벤토리는 가득 찼다고 가정합니다.
  int inventoryStatus3 = 100;

  //자동제어 (앱 제어할때는 필요시, 주석 처리)
  if (t > 10.0 || h > 70.0) { //1층은 온도를 낮게 유지해야하므로 온도가 높으면 냉각팬 on
      led1.on();
  }  
  else {
      led1.off();
  }

  if (t3 < 15.0 || h3 < 50.0) { //3층은 온도를 높게 유지해야하므로 온도가 낮으면 히터 on
      led3.on();
  }
  else {
      led3.off();
  }

   // 거리 센서 값 확인하고 인벤토리 상태 업데이트
  if (distance1 < 20) {
    inventoryStatus = 100;
  } else if (distance1 >= 20 && distance1 < 50) { //예를 들어 거리가 2~5cm이면, 재고가 80%남았다.
    inventoryStatus = 80;
  } else if (distance1 >= 50 && distance1 < 80) {
    inventoryStatus = 60;
  } else if (distance1 >= 80 && distance1 < 110) {
    inventoryStatus = 40;
  } else if (distance1 >= 110 && distance1 < 140) {
    inventoryStatus = 20;
  } else if (distance1 >= 140) {
    inventoryStatus = 0;
  }

    if (distance3 < 20) {
    inventoryStatus3 = 100;
  } else if (distance3 >= 20 && distance3 < 50) { //예를 들어 거리가 2~5cm이면, 재고가 80%남았다.
    inventoryStatus3 = 80;
  } else if (distance3 >= 50 && distance3 < 80) {
    inventoryStatus3 = 60;
  } else if (distance3 >= 80 && distance3 < 110) {
    inventoryStatus3 = 40;
  } else if (distance3 >= 110 && distance3 < 140) {
    inventoryStatus3 = 20;
  } else if (distance3 >= 140) {
    inventoryStatus3 = 0;
  }

  // 디바이스 업데이트 토픽에 대한 페이로드 생성
  sprintf(payload,"{\"state\":{\"state\":{\"reported\":{\"temperature\":\"%0.2f\",\"humidity\":\"%0.2f\",\"temperature3\":\"%0.2f\",\"humidity3\":\"%0.2f\",\"distance\":\"%d\",\"inventory\":\"%d%%\",,\"distance3\":\"%d\",\"inventory3\":\"%d%%\"\"LED\":\"%s\",\"LED3\":\"%s\"}}}", t, h, t3, h3, distance1, inventoryStatus, distance3, inventoryStatus3, led, ledd);
  // MQTT를 통해 페이로드 전송
  sendMessage(payload);

}

void sendMessage(char* payload) {
  char TOPIC_NAME[]= "$aws/things/MyMy/shadow/update";
  
  Serial.print("Publishing send message:");
  Serial.println(payload);
  mqttClient.beginMessage(TOPIC_NAME);
  mqttClient.print(payload);
  mqttClient.endMessage();
}


void onMessageReceived(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // store the message received to the buffer
  char buffer[512] ;
  int count=0;
  while (mqttClient.available()) {
     buffer[count++] = (char)mqttClient.read();
  }
  buffer[count]='\0'; // 버퍼의 마지막에 null 캐릭터 삽입
  Serial.println(buffer);
  Serial.println();

  // JSon 형식의 문자열인 buffer를 파싱하여 필요한 값을 얻어옴.
  // 디바이스가 구독한 토픽이 $aws/things/MyMKRWiFi1010/shadow/update/delta 이므로,
  // JSon 문자열 형식은 다음과 같다.
  // {
  //    "version":391,
  //    "timestamp":1572784097,
  //    "state":{
  //        "LED":"ON"
  //    },
  //    "metadata":{
  //        "LED":{
  //          "timestamp":15727840
  //         }
  //    }
  // }
  //
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buffer);
  JsonObject root = doc.as<JsonObject>();
  JsonObject state = root["state"];
  const char* led = state["LED"];
  const char* ledd = state["LED3"];
  Serial.println(led);
  Serial.println(ledd);
  
  char payload[512];

  // 앱제어
  if (strcmp(led,"ON")==0) {
    led1.on();
    sprintf(payload,"{\"state\":{\"reported\":{\"LED\":\"%s\"}}}","ON");
    sendMessage(payload);
    
  } else if (strcmp(led,"OFF")==0) {
    led1.off();
    sprintf(payload,"{\"state\":{\"reported\":{\"LED\":\"%s\"}}}","OFF");
    sendMessage(payload);
  }

    if (strcmp(ledd,"ON")==0) {
    led3.on();
    sprintf(payload,"{\"state\":{\"reported\":{\"LED3\":\"%s\"}}}","ON");
    sendMessage(payload);
    
  } else if (strcmp(ledd,"OFF")==0) {
    led3.off();
    sprintf(payload,"{\"state\":{\"reported\":{\"LED3\":\"%s\"}}}","OFF");
    sendMessage(payload);
  }
 
}
