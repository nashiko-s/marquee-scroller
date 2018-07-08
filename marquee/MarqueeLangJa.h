/** The MIT License (MIT)

Copyright (c) 2018 nashiko

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
#ifndef _MarqueeLangJa_h
#define _MarqueeLangJa_h
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <ArduinoOTA.h>
#include <Max72xxPanel.h>
#include "UTF8toSJIS.h"


class MarqueeLangJa{
  private:
    ESP8266WebServer *server;
    Max72xxPanel *matrix;
    int *displayScrollSpeed;

    UTF8toSJIS u8ts;
    const char *utf8_sjis_file = "/Utf8Sjis.tbl";
    const char *zen_font_file = "/MSKG13KU.FNT";
      
    File fp_zen;
    uint16_t fnt_adrs_zen;
    uint8_t tmp_buf[8] = {0,0,0,0,0,0,0,0};
    uint8_t tmp_buf2[8] = {0,0,0,0,0,0,0,0};

    void Sjis_To_Misaki_Font_Adrs(uint8_t jisH, uint8_t jisL, uint16_t *fnt_adrs);


  public:
    // Take over parent parameters. [Settings.h, maquee.ino]
    boolean WEBSERVER_ENABLED = true;
    boolean ENABLE_OTA = true;
    int refresh = 0;

    MarqueeLangJa(ESP8266WebServer *server, Max72xxPanel *matrix, int *displayScrollSpeed);
    void scrollMessage(String msg);
};

#endif