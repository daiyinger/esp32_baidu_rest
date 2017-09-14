# esp32

## hardware:
<img src="https://wiki.whyengineer.com/images/c/c3/Esp32_480x480.png" width=256 height=256 />

Know More:
[WhyEngineer](https://www.whyengineer.com/esp32/)

## software:
The project use baidu [rest_api](http://yuyin.baidu.com/docs/asr/57), use the ESP32_circle board's mic collec voice send the fifo,
the http thread read from fifo and use http chunked encode way to send these data to baidu.

Use my webserver framework to control the start and stop.

<img src="https://wiki.whyengineer.com/images/1/10/3.png"/>


the delay about 1s.

## how to use 
change the componets/espressif/wifi.c wifi ssid and password
