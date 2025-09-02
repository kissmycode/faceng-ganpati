#ifndef _faceng_wifi_utils_h_
#define _faceng_wifi_utils_h_

#define ESP_SERIAL Serial3

extern bool wifiSetupSuccessful;
bool setupWiFiForESP();
bool wifiInit();
void wifiSendCmdOverUdp(String cmd);
void wifiSendCmdOverUdpWithRetries(String cmd, int retries = 3, int retry_delay_ms = 250);

#endif // _faceng_wifi_utils_h_