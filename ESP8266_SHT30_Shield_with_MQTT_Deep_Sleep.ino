#define HOSTNAME  "Deep"
#include <WEMOS_SHT3X.h>
#include <ESP8266WiFi.h>
#include "myntp.h"
#include "mqtt.h"

/*  Configuration values to be filled in as needed  */
#define SSID  "..";
#define PASS  "..";
#define WSHOST  "..";
#define WSPORT  81;
#define MQTT_CLIENT ESP.getChipId()
#define MQTT_SERVER "...";
#define MQTT_PORT 17332;
#define MQTT_USER "...";
#define MQTT_PASS "..";

#include <init.h> //  I use this library file to redefine the configuration values - remove if config data are defined abowe

#define TMPTOPIC  &("esp8266/" + String(HOSTNAME) + "/tmp/state")[0]
#define HUMTOPIC  &("esp8266/" + String(HOSTNAME) + "/hum/state")[0]
#define VCCTOPIC  &("esp8266/" + String(HOSTNAME) + "/vcc/state")[0]
ADC_MODE(ADC_VCC);

const char *ssid = SSID;
const char *pass = PASS;
char* mqtt_client = "ESP123"; //String(ESP.getChipId());
char* mqtt_host = MQTT_SERVER;
uint16 mqtt_port = MQTT_PORT;
char* mqtt_user = MQTT_USER;
char* mqtt_pass = MQTT_PASS;

AUTH options = { mqtt_client, mqtt_host, mqtt_port, mqtt_user, mqtt_pass };

bool mqttdone = false;
char t_buf[20];
float val;

SHT3X sht30(0x45);
void onMessage(char* topic, char* value) {  //  MQTT message arraived
}

void onStatus(bool status) {  //  MQTT status changed (true=connected, false=disconnected)
  if (status) {
    float vcc = ESP.getVcc()/1000;
    if (sht30.get() == 0) {
      char buf[80];
      sprintf(buf, "{\"temp\":%.1f,\"hum\":%.0f,\"vcc\":%.2f,\"date\",\"%s\", \"time\":\"%s\"}",sht30.cTemp, sht30.humidity, vcc, MyNTP.dateNow().c_str(), MyNTP.timeNow().c_str());
      mqtt.publish(&("esp8266/" + String(HOSTNAME))[0],buf, true);
      delay(200);
    }
  }
  mqttdone = true;
}

void setup() {
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500); Serial.println();
  MyNTP.set();
  mqtt.onMessage(onMessage);
  mqtt.onStatus(onStatus);
  mqtt.options = options;
  mqtt.begin();
  delay(200);
  while (!mqttdone) {
    mqtt.loop();
    delay(20);
  }
  //Serial.println("Going into deep sleep for 20 seconds");
  //ESP.deepSleep(20e6); // 20e6 is 20 seconds
  ESP.deepSleep(900e6); //  15 minutes

}

void loop() {

  delay(10000);

}
