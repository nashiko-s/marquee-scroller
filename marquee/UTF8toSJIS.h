/*
  UTF8toSJIS.cpp - for ESP-WROOM-02 ( esp8266 )
  Beta version 1.50
  
  This is a library for converting from UTF-8 code string to Shift_JIS code string.
  In advance, you need to upload a conversion table file Utf8Sjis.tbl using SPIFFS file system ESP-WROOM-02(ESP8266) to flash.
  GitHub---> https://github.com/mgo-tec/UTF8_to_Shift_JIS
  
The MIT License (MIT)

Copyright (c) 2016 Mgo-tec
Blog URL ---> https://www.mgo-tec.com

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

--------------------------------------------------------------------------------
Extended
Auther: nashiko
Reference:
https://www.mgo-tec.com/blog-entry-utf8sjis01.html
http://hatenaclang.blogspot.com/2011/03/blog-post_3072.html
*/

#ifndef UTF8toSJIS_h_
#define UTF8toSJIS_h_
#include "Arduino.h"
#include "FS.h"

#define iskanji(c)            ((0x81 <= (unsigned char)(c&0xff) && (unsigned char)(c&0xff) <= 0x9f) || (0xe0 <= (unsigned char)(c&0xff) && (unsigned char)(c&0xff) <= 0xfc))
#define iskana(c)             ((c)-0xa1u < 63)
#define isdaku(c)             (((c)&0xff) == 0xde)
#define ishandaku(c)          (((c)&0xff) == 0xdf)
#define maybedaku(c)          ((c)-0xb6u < 15 || (c)-0xcau < 5)
#define maybehandaku(c)       ((c)-0xcau < 5)

static unsigned int ASCII_ZEN[] = {
  0x8140/*　*/, 0x8149/*！*/, 0x8168/*”*/, 0x8194/*＃*/, 0x8190/*＄*/, 
  0x8193/*％*/, 0x8195/*＆*/, 0x8166/*’*/, 0x8169/*（*/, 0x816A/*）*/, 
  0x8196/*＊*/, 0x817B/*＋*/, 0x8143/*，*/, 0x817C/*－*/, 0x8144/*．*/, 
  0x815E/*／*/, 0x824F/*０*/, 0x8250/*１*/, 0x8251/*２*/, 0x8252/*３*/, 
  0x8253/*４*/, 0x8254/*５*/, 0x8255/*６*/, 0x8256/*７*/, 0x8257/*８*/, 
  0x8258/*９*/, 0x8146/*：*/, 0x8147/*；*/, 0x8183/*＜*/, 0x8181/*＝*/, 
  0x8184/*＞*/, 0x8148/*？*/, 0x8197/*＠*/, 0x8260/*Ａ*/, 0x8261/*Ｂ*/, 
  0x8262/*Ｃ*/, 0x8263/*Ｄ*/, 0x8264/*Ｅ*/, 0x8265/*Ｆ*/, 0x8266/*Ｇ*/, 
  0x8267/*Ｈ*/, 0x8268/*Ｉ*/, 0x8269/*Ｊ*/, 0x826A/*Ｋ*/, 0x826B/*Ｌ*/, 
  0x826C/*Ｍ*/, 0x826D/*Ｎ*/, 0x826E/*Ｏ*/, 0x826F/*Ｐ*/, 0x8270/*Ｑ*/, 
  0x8271/*Ｒ*/, 0x8272/*Ｓ*/, 0x8273/*Ｔ*/, 0x8274/*Ｕ*/, 0x8275/*Ｖ*/, 
  0x8276/*Ｗ*/, 0x8277/*Ｘ*/, 0x8278/*Ｙ*/, 0x8279/*Ｚ*/, 0x816D/*［*/, 
  0x818F/*￥*/, 0x816E/*］*/, 0x814F/*＾*/, 0x8151/*＿*/, 0x814D/*｀*/, 
  0x8281/*ａ*/, 0x8282/*ｂ*/, 0x8283/*ｃ*/, 0x8284/*ｄ*/, 0x8285/*ｅ*/, 
  0x8286/*ｆ*/, 0x8287/*ｇ*/, 0x8288/*ｈ*/, 0x8289/*ｉ*/, 0x828A/*ｊ*/, 
  0x828B/*ｋ*/, 0x828C/*ｌ*/, 0x828D/*ｍ*/, 0x828E/*ｎ*/, 0x828F/*ｏ*/, 
  0x8290/*ｐ*/, 0x8291/*ｑ*/, 0x8292/*ｒ*/, 0x8293/*ｓ*/, 0x8294/*ｔ*/, 
  0x8295/*ｕ*/, 0x8296/*ｖ*/, 0x8297/*ｗ*/, 0x8298/*ｘ*/, 0x8299/*ｙ*/, 
  0x829A/*ｚ*/, 0x816F/*｛*/, 0x8162/*｜*/, 0x8170/*｝*/, 0x8160/*～*/ 
};

static unsigned int KANA_ZEN[] = {
  0x8142/*。*/, 0x8175/*「*/, 0x8176/*」*/, 0x8141/*、*/, 0x8145/*・*/,
  0x8392/*ヲ*/, 0x8340/*ァ*/, 0x8342/*ィ*/, 0x8344/*ゥ*/, 0x8346/*ェ*/,
  0x8348/*ォ*/, 0x8383/*ャ*/, 0x8385/*ュ*/, 0x8387/*ョ*/, 0x8362/*ッ*/,
  0x815b/*ー*/, 0x8341/*ア*/, 0x8343/*イ*/, 0x8345/*ウ*/, 0x8347/*エ*/,
  0x8349/*オ*/, 0x834a/*カ*/, 0x834c/*キ*/, 0x834e/*ク*/, 0x8350/*ケ*/,
  0x8352/*コ*/, 0x8354/*サ*/, 0x8356/*シ*/, 0x8358/*ス*/, 0x835a/*セ*/,
  0x835c/*ソ*/, 0x835e/*タ*/, 0x8360/*チ*/, 0x8363/*ツ*/, 0x8365/*テ*/,
  0x8367/*ト*/, 0x8369/*ナ*/, 0x836a/*ニ*/, 0x836b/*ヌ*/, 0x836c/*ネ*/,
  0x836d/*ノ*/, 0x836e/*ハ*/, 0x8371/*ヒ*/, 0x8374/*フ*/, 0x8377/*ヘ*/,
  0x837a/*ホ*/, 0x837d/*マ*/, 0x837e/*ミ*/, 0x8380/*ム*/, 0x8381/*メ*/,
  0x8382/*モ*/, 0x8384/*ヤ*/, 0x8386/*ユ*/, 0x8388/*ヨ*/, 0x8389/*ラ*/,
  0x838a/*リ*/, 0x838b/*ル*/, 0x838c/*レ*/, 0x838d/*ロ*/, 0x838f/*ワ*/,
  0x8393/*ン*/, 0x814a/*゛*/, 0x814b/*゜*/,
};

enum SJ_TYPE {
    SJ_ASCII = 0, SJ_KANJI = 1, SJ_KANA  = 2, SJ_KANA_D = 3, SJ_KANA_HD = 4,
};

extern File __UtoS;

class UTF8toSJIS
{
public:
  UTF8toSJIS();

  void UTF8_to_SJIS_str_cnv(const char* UTF8SJIS_file, String sclUTF8, uint16_t* sjis_byte, uint16_t* sj_length);
  void UTF8_to_SJIS_str_cnv(String strUTF8, uint16_t* sjis_byte, uint16_t* sj_length);
  void UTF8_to_SJIS_str_cnv(File f2, String strUTF8, uint16_t* sjis_byte, uint16_t* sj_length);
  void UTF8_To_SJIS_code_cnv(uint8_t utf8_1, uint8_t utf8_2, uint8_t utf8_3, uint32_t* spiffs_addrs);
  void SPIFFS_Flash_UTF8SJIS_Table_Read(File ff, uint32_t addrs, uint8_t* buf);
  
private:
  uint8_t getCodeType(uint8_t *sjis_code);
  
};

#endif