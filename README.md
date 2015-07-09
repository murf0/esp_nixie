# esp_nixie
Nixe Clock driven by ESP8266

# Features
Time
Date
Temperature Custom no via mqtt
Alarm with snooze
Flashing leds under Nixie tubes
Sleep Nixie tubes
cathode anti-poisoning on every 5 minutes, (Rotating thru all numbers on all tubes)
NTP support (Set the RTC also)
Config saved to Flash and Set via MQTT

#Used libs:
Spi:
https://github.com/MetalPhreak/ESP8266_SPI_Driver http://d.av.id.au/blog/
Mqtt:
https://github.com/tuanpmt/esp_mqtt.git
Httpd:
http://git.spritesserver.nl/libesphttpd.git/
DS1307 RTC
https://github.com/raburton/esp8266.git


#Resources / Insperation?
https://sv.wikipedia.org/wiki/BCD

http://mightydevices.com/wp-content/uploads/2014/12/Nixie-1.png

#auhm
struct for saving config. - https://github.com/tuanpmt/esp_mqtt/blob/master/modules/config.c