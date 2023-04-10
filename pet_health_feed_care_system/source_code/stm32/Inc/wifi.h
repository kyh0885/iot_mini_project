/*
 * wifi.h
 *
 *  Created on: Apr 4, 2023
 *      Author: IoT21
 */

#ifndef SRC_WIFI_H_
#define SRC_WIFI_H_

#define SSID "iot1"
#define PASS "iot10000"
#define LOGID "6"
#define PASSWD "PASSWD"
#define DST_IP "10.10.141.32"
#define DST_PORT 5000


int connectWiFi();
int wifi_send_cmd (char *str,const char *rstr, unsigned long timeout);
int wifi_wait(const char *str, const char *rstr, unsigned long timeout);
int WIFI_init();
int WIFI_send(char * data);
void send_wifi_simple(char * data);


#endif /* SRC_WIFI_H_ */

