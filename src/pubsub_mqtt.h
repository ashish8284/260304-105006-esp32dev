#include <WiFi.h>
#include <PubSubClient.h>
long lastReconnectAttempt = 0;
bool mqtt_sts = false;
const char* mqtt_ser_chr;
const char* mqtt_usr_chr;
const char* mqtt_pass_chr;
const char* mqtt_topic_chr;
const char* mqtt_topic_sub_char;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);  //check valid json string
  if (error) {
    return;
  }
  led_blink(4, 20);
  int LED = doc["LED"];
  if (LED) digitalWrite(status_led, HIGH);
  else digitalWrite(status_led, LOW);
  int RESET = doc["RESET"];
  if (RESET ==1) {
    ESP.restart();
  }
  if (RESET==2) {
    wm.resetSettings();
  }
  char output[26];
  doc.shrinkToFit();
  serializeJson(doc, _Debug);
  serializeJson(doc, Serial);
  Serial.println();
}
boolean reconnect() {
  int timeout_count = 0;
  led_blink(3, 200);
  while (!client.connected()) {
    if (timeout_count == 0) Serial.println("Attempting MQTT connection...");
    String clientId = "tadlcl-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_usr_chr, mqtt_pass_chr)) {
      client.subscribe(mqtt_topic_sub_char);
    } else {
      timeout_count += 1;
      if (timeout_count > 50) {
        Serial.println("Attempting Failed");
        break;
      }
    }
    Serial.print("MQTT Connected");
  }
  return client.connected();
}
void pubsub_mqtt_setup() {
  mqtt_ser_chr = mqtt_srv.c_str();
  mqtt_usr_chr = mqtt_usr.c_str();
  mqtt_pass_chr = mqtt_pwd.c_str();
  mqtt_topic_chr = mqtt_topic.c_str();
  mqtt_topic_sub = mqtt_topic + "_SUB";
  mqtt_topic_sub_char = mqtt_topic_sub.c_str();
  Serial.println(mqtt_topic_sub_char);
  client.setServer(mqtt_ser_chr, 1883);
  client.setCallback(callback);
}
void pubsub_mqtt_loop() {
  if (!client.connected()) {
    mqtt_sts = false;
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
    digitalWrite(status_led, !digitalRead(status_led));
  } else {
    mqtt_sts = true;
    client.loop();
  }
}