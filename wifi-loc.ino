/* WiFi Signal Strength
 * Shows RSSI of a predefined network
 */

// #define METHOD_SINGLE_AP  // if using a specified AP
#define METHOD_SCAN          // new way, scan all AP's

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "circle_buffer.h"
using namespace std;
#include <vector>
#include <Vector.h>

// for Serial <<
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

#include "wifi_config.h"

#define MAX_KEEP_READINGS 50
#define AVG_COUNT 40
CBuf<long> cbuf(MAX_KEEP_READINGS);

#ifdef METHOD_SCAN
Vector<String> ap_seen;
Vector<CBuf<long>> ap_rssi;
Vector<String> ap_ssid;

#endif

void setup(void) {
	Serial.begin(115200);
	Serial.println("\nESP8266 WiFi Signal Strength");
	Serial.println("WeMos D1 Mini\n");

	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

#ifdef METHOD_SINGLE_AP
	WiFi.begin(myssid, password);
	Serial.print("Connecting");

	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial << "Conn. fail! Rebooting...";
		delay(5000);
		ESP.restart();
	}

	Serial.print("\nConnected to: ");
	Serial.println(myssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println("");
#endif

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

void loop_single(void) {
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

void loop(void) {
	loop_scan();
}

#ifdef METHOD_SCAN
int8_t ap_lookup(String id) { // -1 if not found
	for (int8_t i=0; i<ap_seen.size(); i++) {
		if (ap_seen[i] == id) return(i);
	}
	return(-1);
}
void loop_scan() {
	static int aps=0;             // total ap count seen
	int n = WiFi.scanNetworks();
	//Serial.println("scan done");
	//if (n == 0) Serial.println("no networks found");
	//Serial << "\n";
	for (int i=0; i<n; i++) {
		int api;
		String ssid = WiFi.SSID(i);
		String bssid = WiFi.BSSIDstr(i);
		//const char *ssidc = ssid.c_str();
		api = ap_lookup(bssid);
		if (api < 0) { // FOUND
			//Serial << " *NEW* BSSID: Adding " << ssid << " (" << bssid << ") to hash at " << api << "\n";
			// append to hash, marking for later free()
			CBuf<long> readings(MAX_KEEP_READINGS);
			readings.init(0); // zero out all data points
			ap_rssi.push_back(readings);
			ap_seen.push_back(bssid);
			ap_ssid.push_back(ssid);
			api=ap_seen.size()-1;
		}
		//Serial << " api is: " << api << "\n";
		ap_rssi[api].add(WiFi.RSSI(i));
		//Serial << ssid << " stored at " << api << "\n";
		//aps_cbuf[api-1]
	}
	for (int i=0; i<ap_seen.size(); i++) {
		//Serial << i << ": " << ap_ssid[i] << " (" << ap_seen[i] << ") strength: "
			//<< ap_rssi[i].lastn(0) << "\n";
		//Serial << ap_rssi[i].lastn(0) << " ";  // single reading
		Serial << ap_rssi[i].avglastf(10) << " ";
	}
	Serial << "\n";

	// Wait a bit before scanning again
	//delay(5);
}
#endif
