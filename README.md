/* The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

# Marquee Scroller (Clock, Weather, News, and More)

## Features include:
* Accurate Clock refresh off Internet Time Servers
* Local Weather and conditions (refreshed every 10 - 30 minutes)
* News Headlines from all the major sources
* Configured through Web Interface
* Display 3D print progress from your OctoPrint Server
* Option to display random goofy advice
* Option to display Bitcoin current value
* Basic Authorization around Configuration web interface
* Support for OTA (loading firmware over WiFi)
* Configurable scroll speed
* Configurable scrolling frequency
* Configurable number of LED panels
* Options of different types of Clock Displays (display seconds or temperature) -- only on 8 or more display panels
* Video: https://youtu.be/DsThufRpoiQ

## Required Parts:
* Wemos D1 Mini: https://amzn.to/2qLyKJd
* Dot Matrix Module: https://amzn.to/2HtnQlD  

Note: Using the links provided here help to support these types of projects. Thank you for the support.  

## Wiring for the Wemos D1 Mini to the Dot Matrix Display
CLK -> D5 (SCK)  
CS  -> D6  
DIN -> D7 (MOSI)  
VCC -> 5V+  
GND -> GND-  

![Marquee Scroller Wiring](/images/marquee_scroller_pins.png)  

## 3D Printed Case by David Payne:  
Original Single Panel version: https://www.thingiverse.com/thing:2867294  
Double Wide LED version: https://www.thingiverse.com/thing:2989552  

## Compiling and Loading to Wemos D1
It is recommended to use Arduino IDE.  You will need to configure Arduino IDE to work with the Wemos board and USB port and installed the required USB drivers etc.  
* USB CH340G drivers:  https://wiki.wemos.cc/downloads
* Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.  This will add support for the Wemos D1 Mini to Arduino IDE.
* Open Boards Manager from Tools > Board menu and install esp8266 platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).
* Select Board:  "WeMos D1 R2 & mini"

## Loading Supporting Library Files in Arduino
Use the Arduino guide for details on how to installing and manage libraries https://www.arduino.cc/en/Guide/Libraries  
**Packages** -- the following packages and libraries are used (download and install):  
<ESP8266WiFi.h>  
<ArduinoJson.h>  --> https://github.com/bblanchon/ArduinoJson (version 5.13.X)  
<ESP8266WebServer.h>  
<WiFiManager.h> --> https://github.com/tzapu/WiFiManager  
"FS.h"  
<SPI.h>  
<Adafruit_GFX.h>  --> https://github.com/adafruit/Adafruit-GFX-Library  
<Max72xxPanel.h> --> https://github.com/markruys/arduino-Max72xxPanel  
<JsonStreamingParser.h> --> https://github.com/squix78/json-streaming-parser  

**日本語の設定**  
ニュースだけ日本語になります。  
半角は全角に変換されます。  
縦表示には対応していません。 
1. 言語変換ファイルをアップロードする必要があるので https://github.com/esp8266/arduino-esp8266fs-plugin の「Installation」を参考にインストール。
3. Arduino IDEからdataフォルダのファイルをアップロード。
4. Settings.hを以下のように変更。  
NEWS_SOURCE = "jp"

![Marquee Lang Ja](/images/marquee_lang_ja.gif)  

Thanks:  
https://github.com/mgo-tec/UTF8_to_Shift_JIS  
http://hatenaclang.blogspot.com/2011/03/blog-post_3072.html

## Initial Configuration
Starting with version 2.0 editing the **Settings.h** file is optional.  All API Keys are now managed in the Web Interface except for the GeoNames Key. It is not required to edit the Settings.h file before loading and running the code.  
* Open Weather Map API Key: http://openweathermap.org/
* News API Key: https://newsapi.org/
* GeoNames Key (optional as you can use mine or create your own) http://www.geonames.org/ -- this key is only managed in the Settings.h file.
* Your OctoPrint API Key (optional if you use the OctoPrint status)
* Version 2.0 supports Chained 4x1 LED displayes -- configure in the Settigns.h file.  

NOTE: The settings in the Settings.h are the default settings for the first loading. After loading you will manage changes to the settings via the Web Interface. If you want to change settings again in the settings.h, you will need to erase the file system on the Wemos or use the “Reset Settings” option in the Web Interface.  

## Web Interface
The Marquee Scroller uses the **WiFiManager** so when it can't find the last network it was connected to 
it will become a **AP Hotspot** -- connect to it with your phone and you can then enter your WiFi connection information.

After connected to your WiFi network it will display the IP addressed assigned to it and that can be 
used to open a browser to the Web Interface.  You will be able to manage your API Keys through the web interface.  
The default user / password for the configuration page is: admin / password  

The Clock will display the time of the City selected for the weather.  

<p align="center">
  <img src="/images/2018-04-19%2006.58.05.png" width="200"/>
  <img src="/images/2018-04-19%2006.58.15.png" width="200"/>
  <img src="/images/2018-04-19%2006.58.32.png" width="200"/>
  <img src="/images/2018-04-19%2006.58.58.png" width="200"/>
</p>

## Donation or Tip
Please do not feel obligated, but donations and tips are warmly welcomed.  I have added the donation button at the request of a few people that wanted to contribute and show appreciation.  Thank you, and enjoy the application and project.  

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=A82AT6FLN2MPY)

## Contributors
David Payne  
Nathan Glaus  
Daniel Eichhorn -- Author of the TimeClient class  
yanvigdev  
nashiko  

![Marquee Scroller](/images/5d7f02ccbf01125cabbf246f97f2ead1_preview_featured.jpg)  
![Marquee Parts](/images/1ffa0c835554d280258c13be5513c4fe_preview_featured.jpg)
