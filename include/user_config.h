#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#define CFG_HOLDER	0x00FF55A4	/* Change this value to load default configurations */
#define CFG_LOCATION	0x2C	/* Please don't change or if you know what you doing */


/*DEFAULT CONFIGURATIONS*/


#define MQTT_BUF_SIZE		1024
#define MQTT_KEEPALIVE		120	 /*second*/


#define MQTT_RECONNECT_TIMEOUT 	5	/*second*/

#define DEFAULT_SECURITY	1 /*1=SSL 0=None*/
#define QUEUE_BUFFER_SIZE		 		2048

#define PROTOCOL_NAMEv31


/*Necessary for build of mqtt not actually used */


#define MQTT_CLIENT_ID		"NA"
#define MQTT_ROOT_TOPIC     "NA"
#define MQTT_USER			"NA"
#define MQTT_PASS			"NA"
#define STA_SSID            "NA"
#define STA_PASS            "NA"
#define STA_TYPE AUTH_WPA2_PSK
#define MQTT_HOST			"NA" 
#define MQTT_PORT			1000
#define CLIENT_SSL_ENABLE   0

#endif
