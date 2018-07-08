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

#include "MarqueeLangJa.h"

MarqueeLangJa::MarqueeLangJa(ESP8266WebServer *server, Max72xxPanel *matrix, int *displayScrollSpeed)
{
  MarqueeLangJa::server = server;
  MarqueeLangJa::matrix = matrix;
  MarqueeLangJa::displayScrollSpeed = displayScrollSpeed;
}

void MarqueeLangJa::scrollMessage(String msg)
{
  uint16_t sj_txt[512] = {};
  uint16_t sj_cnt = 0;
  uint16_t sj_length = 0;

  u8ts.UTF8_to_SJIS_str_cnv(utf8_sjis_file, msg, sj_txt, &sj_length);
  uint8_t font_buf[sj_length / 2][8];
  uint16_t font_buf_index = 0;
  fp_zen = SPIFFS.open(zen_font_file, "r");

  while (sj_txt[sj_cnt] != NULL) {
    Sjis_To_Misaki_Font_Adrs(sj_txt[sj_cnt], sj_txt[sj_cnt + 1], &fnt_adrs_zen);
    fp_zen.seek(fnt_adrs_zen, SeekSet);
    for (int j = 0; j < 8; j++) {
      tmp_buf[j] = fp_zen.read();
    }

    // reflect rotate
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        bitWrite(font_buf[font_buf_index][7-col], row, bitRead(tmp_buf[row], 7-col));
      }
    }
    sj_cnt += 2;
    font_buf_index++;
  }

  fp_zen.close();

  int font_width = 8;
  int width_max = font_buf_index * font_width + matrix->width() + font_width;
  int x, letter, br;
  for (int i = 0; i < width_max; i++){

    if (WEBSERVER_ENABLED) {
      server->handleClient();
    }
    
    if (ENABLE_OTA) {
      ArduinoOTA.handle();
    }

    if (refresh==1) i=0;
    refresh=0;
    matrix->fillScreen(LOW);

    letter = i / font_width;
    x = matrix->width() - i % font_width;

    while (x + font_width >= 0 && letter >= 0) {
      if (letter < font_buf_index) {
        for (int row = 0; row < 8; row++) {
          for (int col = 0; col < 8; col++) {
            br = bitRead(font_buf[letter][row], col);
            matrix->drawPixel(x + col, row, br);
          }
        }
      }

      letter--;
      x -= font_width;
    }

    matrix->write(); // Send bitmap to display
    delay(*displayScrollSpeed);
  }
  matrix->setCursor(0, 0);
};


//*******************Shift_JISコードから美咲フォントアドレス計算********************************************
void MarqueeLangJa::Sjis_To_Misaki_Font_Adrs(uint8_t jisH, uint8_t jisL, uint16_t *fnt_adrs) 
{ 
  // S-JISコードからMisakiフォントファイル上のバイト位置をポインタで返す。
  uint16_t SjisCode;
  int16_t adj;

  if (jisH != '\0') { //'\0'ならば読み込まない。
    if ((jisH >= 0x81 && jisH <= 0x9f) || (jisH >= 0xe0 && jisH <= 0xef)) { //全角の場合
      SjisCode = ((uint16_t)jisH << 8) + jisL;
      if (SjisCode >= 0x8140 && SjisCode <= 0x88fc) {
        if (SjisCode >= 0x8140 && SjisCode <= 0x817e)
          adj = 16; // 一般記号
        else if (SjisCode >= 0x8180 && SjisCode <= 0x81fc)
          adj = 15; // 一般記号
        else if (SjisCode >= 0x824f && SjisCode <= 0x8279)
          adj = -52; // 数字、英語大文字
        else if (SjisCode >= 0x8281 && SjisCode <= 0x82f1)
          adj = -53; // 英小文字、ひらがな
        else if (SjisCode >= 0x8340 && SjisCode <= 0x837e)
          adj = -120; // カタカナ
        else if (SjisCode >= 0x8380 && SjisCode <= 0x83d6)
          adj = -121; // カタカナ
        else if (SjisCode >= 0x8440 && SjisCode <= 0x847e)
          adj = -188; // 外国大文字
        else if (SjisCode >= 0x8480 && SjisCode <= 0x84be)
          adj = -189; // 外国小文字、罫線、※丸囲み文字やその他特殊文字は美咲フォントには無い。
        else if (SjisCode >= 0x8740 && SjisCode <= 0x877e)
          adj = -768; // １３区
        else if (SjisCode >= 0x8780 && SjisCode <= 0x879c)
          adj = -769; // １３区
        else if (SjisCode >= 0x889f && SjisCode <= 0x88fc)
          adj = -837; // 第一水準漢字 亜～蔭まで
        *fnt_adrs = ((SjisCode - 0x8140) + adj) * 8;

      } else if (SjisCode >= 0x8940 && SjisCode <= 0x9ffc) { //院～滌
        if (jisL <= 0x7e) {
          *fnt_adrs = ((SjisCode - 0x8140) - (836 + (jisH - 0x88)) - 67 * (jisH - 0x88)) * 8;
        } else if (jisL >= 0x80) {
          *fnt_adrs = ((SjisCode - 0x8140) - (837 + (jisH - 0x88)) - 67 * (jisH - 0x88)) * 8;
        }
      } else if (SjisCode >= 0xe040 && SjisCode <= 0xeaa4) { //漾～熙
        if (jisL <= 0x7e) {
          *fnt_adrs = ((SjisCode - 0x8140) - (836 + (jisH - 0x88)) - 67 * (jisH - 0x88) - 12032) * 8;
        } else if (jisL >= 0x80) {
          *fnt_adrs = ((SjisCode - 0x8140) - (837 + (jisH - 0x88)) - 67 * (jisH - 0x88) - 12032) * 8;
        }
      } else {
        *fnt_adrs = (0x8140 + 16) * 8; // 対応文字コードがなければ 全角スペースを返す
      }
    } else {
      *fnt_adrs = (0x8140 + 16) * 8; // 対応文字コードがなければ 全角スペースを返す
    }
  } else {
    *fnt_adrs = (0x8140 + 16) * 8; // 対応文字コードがなければ 全角スペースを返す
  }
}
