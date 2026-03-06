#include <WiFiManager.h>
WiFiManager wm;
bool wifi_sts = false;
int reset_pin = 19;

WiFiManagerParameter custom_mqtt_server("server", "mqtt Server", "", 40);
WiFiManagerParameter custom_mqtt_user("user", "mqtt User", "", 40);
WiFiManagerParameter custom_mqtt_password("password", "mqtt Pass", "", 40);
WiFiManagerParameter custom_mqtt_topic("topic", "Topic", "", 40);

void saveParamsCallback() {
  JsonDocument doc;
  const char* data1 = (custom_mqtt_server.getValue());
  const char* data2 = (custom_mqtt_user.getValue());
  const char* data3 = (custom_mqtt_password.getValue());
  const char* data4 = (custom_mqtt_topic.getValue());
  byte a = strlen(data1);
  byte b = strlen(data2);
  byte c = strlen(data3);
  byte d = strlen(data4);
  if (strlen(data1)) mqtt_srv = data1;
  if (strlen(data2)) mqtt_usr = data2;
  if (strlen(data3)) mqtt_pwd = data3;
  if (strlen(data4)) mqtt_topic = data4;
  doc["MQTT_SRV"] = mqtt_srv;
  doc["MQTT_USR"] = mqtt_usr;
  doc["MQTT_PWD"] = mqtt_pwd;
  doc["MQTT_TOPIC"] = mqtt_topic;
  char MSG[256];
  serializeJson(doc, MSG);
  //Serial.println(MSG);
  writeFile(ParaFile, MSG);
}
void wifi_manager_setup() {
  pinMode(reset_pin, INPUT_PULLUP);
  bool reset_flag = digitalRead(reset_pin);
  if (reset_flag) {
    delay(5000);
    if (reset_flag){
    led_blink(10, 100);
    wm.setDebugOutput(false);
    wm.resetSettings();
    }
  }
  wm.setDebugOutput(false);
  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_user);
  wm.addParameter(&custom_mqtt_password);
  wm.addParameter(&custom_mqtt_topic);
  wm.setConfigPortalBlocking(true);
  wm.setSaveParamsCallback(saveParamsCallback);
  wm.setWiFiAutoReconnect(true);
  wm.setConfigPortalTimeout(120);
  led_blink(3, 200);
  if (wm.autoConnect("TADL_WM", "tadl$123")) {
    wifi_sts = true;
    String IP = WiFi.localIP().toString();
    JsonDocument doc;
    doc["IP"] = IP;
    doc.shrinkToFit();
    //serializeJson(doc, Serial);
    //Serial.println();
  } else {
    wifi_sts = false;
    ESP.restart();
  }
}
