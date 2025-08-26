#include <Arduino.h>
#include <WiFiEspAT.h>
#include <WiFiUdp.h>
#include "FacengGanpatiUtilities.h"
#include "FacengGanpatiWiFiUtils.h"
#include "credentials.h"

int status = WL_IDLE_STATUS;


// Local IP address and a port to listen on
const char* macbook_ip = "192.168.31.152"; // <-- IMPORTANT: Fetch this if WiFi changes
const int macbook_port = 8888;

// Create an instance of the UDP client
WiFiUdpSender Udp;

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      DEBUG_PRINT(":");
    }
    if (mac[i] < 16) {
      DEBUG_PRINT("0");
    }
    DEBUG_PRINT("%u", mac[i]);
  }
  DEBUG_PRINT("\n");
}

void listNetworks() {
  // scan for nearby networks:
 DEBUG_PRINT("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    DEBUG_PRINT("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  DEBUG_PRINT("number of available networks: ");
  DEBUG_PRINT("%d", numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    DEBUG_PRINT("%d", thisNet + 1);
    DEBUG_PRINT(") ");
    DEBUG_PRINT("Signal: ");
    DEBUG_PRINT("%d", WiFi.RSSI(thisNet));
    DEBUG_PRINT(" dBm");
    DEBUG_PRINT("\tChannel: ");
    DEBUG_PRINT("%d", WiFi.channel(thisNet));
    byte bssid[6];
    DEBUG_PRINT("\t\tBSSID: ");
    printMacAddress(WiFi.BSSID(thisNet, bssid));
    DEBUG_PRINT("\tEncryption: ");
    DEBUG_PRINT("\t\tSSID: ");
    Serial.println(WiFi.SSID(thisNet));
    DEBUG_PRINT("\n")
  }
  DEBUG_PRINT("\n");
}

/**
 * Initializes the ESP8266 module and connects to the Wi-Fi network.
 * @return True if connection is successful, false otherwise.
 */
bool wifiInit() {
  // Initialize the ESP module
  ESP_SERIAL.begin(115200);
  WiFi.init(&ESP_SERIAL);

  // Check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    DEBUG_PRINT("WiFi shield not present\n");
    return false;
  }

  listNetworks();
  // Attempt to connect to Wi-Fi network with retries
  DEBUG_PRINT("Attempting to connect to SSID: %s\n", ssid);
  for (int i = 0; i < 5; i++) {
    status = WiFi.begin(ssid, password);
    if (status == WL_CONNECTED) {
      break;
    }
    DEBUG_PRINT("Connection failed %d, retrying...\n", status);
    delay(5000);
  }

  if (status != WL_CONNECTED) {
    DEBUG_PRINT("Failed to connect to Wi-Fi after multiple attempts.\n");
    return false;
  }

  // Print connection details
  DEBUG_PRINT("Successfully connected to Wi-Fi.\n");
  IPAddress ip = WiFi.localIP();
  DEBUG_PRINT("IP Address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

  // Start the UDP listener on a local port (required by the library)
  Udp.begin(macbook_port);

  return true;
}

/**
 * Sends a command string over UDP to the configured IP and port.
 * @param cmd The command string to send (e.g., "play_aarti").
 */
void wifiSendCmdOverUdp(String cmd) {
  // Begin the UDP packet
  Udp.beginPacket(macbook_ip, macbook_port);
  
  // Write the command string to the packet buffer
  Udp.print(cmd);
  
  // Send the packet
  if (Udp.endPacket()) {
    DEBUG_PRINT("Command '%s' sent successfully.\n", cmd.c_str());
  } else {
    DEBUG_PRINT("Failed to send command '%s'.\n", cmd.c_str());
  }
}