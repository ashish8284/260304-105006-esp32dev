#include <FS.h>
#include <LittleFS.h>

String mqtt_srv;
String mqtt_usr;
String mqtt_pwd;
String mqtt_topic;
String mqtt_mode;
String mqtt_topic_sub;
const char* ParaFile = "/Para.json";

void fs_mount();

void readFile(const char* path) {
  fs_mount();
  Serial.printf("Reading file: %s\n", path);
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.print("Read from file: ");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  mqtt_srv = doc["MQTT_SRV"].as<String>();
  mqtt_usr = doc["MQTT_USR"].as<String>();
  mqtt_pwd = doc["MQTT_PWD"].as<String>();
  mqtt_topic = doc["MQTT_TOPIC"].as<String>();
  mqtt_mode = doc["MQTT_MODE"].as<String>();
  char MSG[250];
  serializeJson(doc, MSG);
  Serial.println(MSG);
  file.close();
  // LittleFS.end();
}
void writeFile(const char* path, const char* message) {
  fs_mount();
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000);  // Make sure the CREATE and LASTWRITE times are different
  file.close();
  // LittleFS.end();
}

void ltfs_chk() {
  readFile(ParaFile);
  // LittleFS.end();
}
void fs_mount() {
  if (!LittleFS.begin(true)) {
    Serial.println("LITTLEFS Mount Failed.");
    return;
  } else {
    Serial.println("LittleFS Mount SUCCESSFUL.");
  }
}