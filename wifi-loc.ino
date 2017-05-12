/* WiFi Signal Strength
 * Shows RSSI of a predefined network
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "circle_buffer.h"

// for Serial <<
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 


// Pins
const int8_t RST_PIN = D2;
const int8_t CE_PIN = D1;
const int8_t DC_PIN = D6;
//const int8_t DIN_PIN = D7;  // Uncomment for Software SPI
//const int8_t CLK_PIN = D5;  // Uncomment for Software SPI
const int8_t BL_PIN = D0;

// Graph the RSSI of this WiFi
#include "wifi_config.h"

CBuf<long> cbuf(50);

void setup(void) {
	Serial.begin(115200);
	Serial.println("\nESP8266 WiFi Signal Strength");
	Serial.println("WeMos D1 Mini\n");

	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	WiFi.begin(ssid, password);
	Serial.print("Connecting");

	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial << "Conn. fail! Rebooting...";
		delay(5000);
		ESP.restart();
	}

	Serial.print("\nConnected to: ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println("");

	cbuf.init(0);
}

float avglastf(int n) {
	float tot = 0;
	for (int i=0; i<n; i++) {
		//Serial << "Last[" << i << "] " << cbuf.lastn(i);
		tot += cbuf.lastn(i);
		//Serial << "  Tot: " << tot << "\n";
	}
	//Serial << "  Avg: " << tot/n << "\n";
	return tot/n;
}
int avglast(int n) {
	int tot = 0;
	for (int i=0; i<n; i++) {
		tot += cbuf.lastn(i);
	}
	return tot/n;
}

void loop(void) {
  static int skippy=0;
  long rssi;
  rssi = WiFi.RSSI();  // eg. -63
  cbuf.add(rssi);
  skippy++;
  if (!(skippy % 10)) {  // Decrease this for faster plotting
	  //Serial << rssi;
	  //Serial << " ";
	  //Serial << avglast(20);
	  //Serial << " ";
	  Serial << avglastf(40);  // Smooth out (40 samples)
	  Serial << "\n";
  }
  delay(5);              // Can decrease this for more samples (faster plotting) too
}

