#include <LWiFi.h>
#include <WiFiClient.h>
#include "MCS.h"

// Assign AP ssid / password here
#define _SSID "AA202_2G"
#define _KEY  "iteaa202"

// Assign device id / key of your test device
MCSDevice mcs("DLMBUGBq", "pXVu9tVMl5aJ1Den");

// Assign channel id
// The test device should have 2 channel
// the first channel should be "Controller" - "On/Off"
// the secord channel should be "Display" - "On/Off"
MCSControllerOnOff led("ledc");
MCSDisplayOnOff    remote("ledremote");
MCSDisplayInteger  Temp("Temp");

#define LED_PIN 7

void setup() {
  // setup Serial output at 9600
  Serial.begin(9600);

  // setup LED/Button pin
  pinMode(LED_PIN, OUTPUT);

  // setup Wifi connection
  while (WL_CONNECTED != WiFi.status())
  {
    Serial.print("WiFi.begin(");
    Serial.print(_SSID);
    Serial.print(",");
    Serial.print(_KEY);
    Serial.println(")...");
    WiFi.begin(_SSID, _KEY);
  }
  Serial.println("WiFi connected !!");

  // setup MCS connection
  mcs.addChannel(led);
  mcs.addChannel(remote);
  mcs.addChannel(Temp);

  while (!mcs.connected())
  {
    Serial.println("MCS.connect()...");
    mcs.connect();
  }
  Serial.println("MCS connected !!");

  // read LED value from MCS server
  while (!led.valid())
  {
    Serial.println("read LED value from MCS...");
    led.value();
  }
  Serial.print("done, LED value = ");
  Serial.println(led.value());
  digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
}

void loop() {
  // call process() to allow background processing, add timeout to avoid high cpu usage
  Serial.print("process(");
  Serial.print(millis());
  Serial.println(")");
  mcs.process(100);

  // updated flag will be cleared in process(), user must check it after process() call.
  if (led.updated())
  {
    Serial.print("LED updated, new value = ");
    Serial.println(led.value());
    digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
    if (!remote.set(led.value()))
    {
      Serial.print("Failed to update remote");
      Serial.println(remote.value());
    }
   
  }
  Temp.set(analogRead(A0));   //把A0 data 丟到 temp data channel
  Serial.print("input value: ");
  Serial.println(analogRead(A0)); //顯示A0 data
  delay(1000);   //每秒上傳一次
  // check if need to re-connect
  while (!mcs.connected())
  {
    Serial.println("re-connect to MCS...");
    mcs.connect();
    if (mcs.connected())
      Serial.println("MCS connected !!");
  }
}
