#include <Arduino.h>
#include <ArduinoJson.h>
int status_led = 2;
//Serial port setting for json read form stm
#define _Debug_RX 04
#define _Debug_TX 16
#define _Debug_Baud 115200
HardwareSerial _Debug(2);
//

#include "functions.h"
#include "spiffs_handle.h"
#include "wifi_manager_set.h"
#include "pubsub_mqtt.h"
unsigned long one_sec_milli = 0;

void setup() {
  pinMode(status_led, OUTPUT);
  led_blink(5, 200);
  Serial.begin(115200);
  Serial.setTimeout(20);
  ltfs_chk();
  wifi_manager_setup();
  pubsub_mqtt_setup();
  one_sec_milli = millis();
}
void loop() {
  wm.process();
  if (WiFi.status() != WL_CONNECTED) {
    wifi_sts = false;
    digitalWrite(status_led, HIGH);
  } else {
    wifi_sts = true;
  }
  if (wifi_sts) {
    pubsub_mqtt_loop();
  }
}
void serial_read_json_chk() {
  if (_Debug.available() > 0) {                                 //Check Serial port for data
    JsonDocument doc;                                           //defining static json
    String Jinput = _Debug.readString();                        //reading serial string and storing in variable
    DeserializationError error = deserializeJson(doc, Jinput);  //check valid json string
    if (error) {
      return;
    }
    //led_blink(2, 50);
    char output[512];
    doc.shrinkToFit();
    serializeJson(doc, output);
    Serial.print(output);
    if (mqtt_sts) client.publish(mqtt_topic_chr, output);
  }
}
void one_sec_call() {
  if (millis() - one_sec_milli > 60000) {
    one_sec_milli = millis();
    //Networ_Sts_Post();
  }
}
void Networ_Sts_Post() {
  String IP = WiFi.localIP().toString();
  JsonDocument ip_doc;
  ip_doc["IP"] = IP;
  if (mqtt_sts) {
    ip_doc["Mqt"] = 1;
  } else {
    ip_doc["Mqt"] = 0;
  }
  ip_doc.shrinkToFit();
  serializeJson(ip_doc, Serial);
  _Debug.println();
}
