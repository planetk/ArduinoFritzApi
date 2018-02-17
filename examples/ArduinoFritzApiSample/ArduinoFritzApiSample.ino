/* This Sketch demontrates how to switch an actor on and 
 * get the current switch, power consumption, energy and
 * temperature.
 */

#include <ArduinoFritzApi.h>
#include <WiFi.h>

const char* ssid           = "your-wifi-ssid";
const char* password       = "your-wifi-password";

const char* fritz_user     = "your-fritzbox-user";
const char* fritz_password = "your-fritzbox-user-password";
const char* fritz_ip       = "address-of-fritzbox"; // ip or fritz.local

const char* fritz_ain      = "ain-of-your-device";
/* 
 * The actor identification number (ain) can be fount in the fritzbox  
 * web interface or is found on a label on the device
 * 
 */

FritzApi fritz(fritz_user, fritz_password, fritz_ip);

void setup() {
  Serial.begin(115200);

  // WiFi Connection stuff
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Initialize Fritzbox stuff;
  try {
    fritz.init();
  } catch (int e) {
    Serial.println("Could not connect to fritzbox: " + String(e));
  }
  Serial.println("Fritz connected");
}

void loop() {

  try {
    boolean b = fritz.setSwitchOn(fritz_ain);
    if (b) {
      Serial.println("Switch is on");
    } else {
      Serial.println("Switch is off");
    }
  } catch (int e) {
    Serial.println("Got errorCode during execution " + String(e));
  }

  try {
    double power = fritz.getSwitchPower(fritz_ain);
    Serial.printf("Current power consumption is %.3f W\n", power);
  } catch (int e) {
    Serial.println("Got errorCode during execution " + String(e));
  }

  try {
    int energy = fritz.getSwitchEnergy(fritz_ain);
    Serial.printf("Total Energy is %i Wh\n", energy);
  } catch (int e) {
    Serial.println("Got errorCode during execution " + String(e));
  }

  try {
    double temp = fritz.getTemperature(fritz_ain);
    Serial.printf("Current temperature is %.1f °C\n", temp);
  } catch (int e) {
    Serial.println("Got errorCode during execution " + String(e));
  }

  delay(2000);

}