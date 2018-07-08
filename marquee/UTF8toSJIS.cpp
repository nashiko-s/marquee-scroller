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

#include "Arduino.h"
#include "UTF8toSJIS.h"


UTF8toSJIS::UTF8toSJIS(){}

//***********String型文字列をShift_JISコードに変換************************************
void UTF8toSJIS::UTF8_to_SJIS_str_cnv(const char* UTF8SJIS_file, String strUTF8, uint16_t* sjis_byte, uint16_t* sj_length)
{
  SPIFFS.begin();
  File f2 = SPIFFS.open(UTF8SJIS_file, "r");

  UTF8toSJIS::UTF8_to_SJIS_str_cnv(f2, strUTF8, sjis_byte, sj_length);

  f2.close();
}


//***********String型文字列をShift_JISコードに変換************************************
void UTF8toSJIS::UTF8_to_SJIS_str_cnv(String strUTF8, uint16_t* sjis_byte, uint16_t* sj_length)
{
  UTF8toSJIS::UTF8_to_SJIS_str_cnv(__UtoS, strUTF8, sjis_byte, sj_length);
}


//***********String型文字列をShift_JISコードに変換************************************
void UTF8toSJIS::UTF8_to_SJIS_str_cnv(File f2, String strUTF8, uint16_t* sjis_byte, uint16_t* sj_length)
{
  uint16_t sj_cnt = 0;
  uint16_t fnt_cnt = 0;
  uint32_t sp_addres=0x9DCC; //スペース
  uint8_t SJ[2];
  uint8_t SJ_NEXT[2];
  uint8_t code_type;
  uint16_t str_length = strUTF8.length();

  while (strUTF8[fnt_cnt] != '\0') {
    if(strUTF8[fnt_cnt] >= 0xE2 && strUTF8[fnt_cnt] <= 0xEF) { // 3バイト
      UTF8toSJIS::UTF8_To_SJIS_code_cnv(strUTF8[fnt_cnt],strUTF8[fnt_cnt+1],strUTF8[fnt_cnt+2], &sp_addres);
      UTF8toSJIS::SPIFFS_Flash_UTF8SJIS_Table_Read(f2, sp_addres, SJ);

      if ((SJ[0]>=0xA1 && SJ[0]<=0xDF)) { // Shift_JISで半角カナコードが返ってきた場合の対処
        int kana_pos = SJ[0] - 0xA1;
        Serial.println("pos:" + String(kana_pos));

        if (strUTF8[fnt_cnt+3] >= 0xEF && strUTF8[fnt_cnt+4] >= 0xBE && strUTF8[fnt_cnt+5] >= 0x9E) {
          // 次の文字が濁点か半濁点は結合して全角カナに変換
          UTF8toSJIS::UTF8_To_SJIS_code_cnv(strUTF8[fnt_cnt+3], strUTF8[fnt_cnt+4], strUTF8[fnt_cnt+5], &sp_addres);
          UTF8toSJIS::SPIFFS_Flash_UTF8SJIS_Table_Read(f2, sp_addres, SJ_NEXT);
          SJ[1] = SJ_NEXT[0];

          uint8_t code_type = UTF8toSJIS::getCodeType(SJ);
          if (code_type == SJ_KANA_D || code_type == SJ_KANA_HD) {
              SJ[0] = ((KANA_ZEN[kana_pos] & 0xff00)>>8);
              SJ[1] = ((KANA_ZEN[kana_pos] & 0xff));
              if ( code_type == SJ_KANA_D ) {
                SJ[1] ++;
              } else if (code_type == SJ_KANA_HD) {
                SJ[1] += 2;
              }
            fnt_cnt += 3;
          }
          
        } else {
          // 全角カナに変換
          SJ[0] = ((KANA_ZEN[kana_pos] & 0xff00)>>8);
          SJ[1] = ((KANA_ZEN[kana_pos] & 0xff));
        }
      }

      sjis_byte[sj_cnt] = SJ[0];
      sjis_byte[sj_cnt+1] = SJ[1];
      sj_cnt += 2;
      fnt_cnt += 3;

    }else if (strUTF8[fnt_cnt] >= 0xC2 && strUTF8[fnt_cnt] <= 0xD1) { // 2バイト
      UTF8toSJIS::UTF8_To_SJIS_code_cnv(strUTF8[fnt_cnt],strUTF8[fnt_cnt+1],0x00, &sp_addres);
      UTF8toSJIS::SPIFFS_Flash_UTF8SJIS_Table_Read(f2, sp_addres, SJ);

      sjis_byte[sj_cnt] = SJ[0];
			sjis_byte[sj_cnt+1] = SJ[1];
			sj_cnt = sj_cnt + 2;
      fnt_cnt = fnt_cnt + 2;

    } else if (strUTF8[fnt_cnt] >= 0x20 && strUTF8[fnt_cnt] <= 0x7E) { // 1バイト ASCII
      int ascii_pos = strUTF8[fnt_cnt] - 0x20;
      sjis_byte[sj_cnt] = ((ASCII_ZEN[ascii_pos] & 0xff00)>>8);
      sjis_byte[sj_cnt+1] = ((ASCII_ZEN[ascii_pos] & 0xff));
      sj_cnt +=2;
      fnt_cnt ++;

    } else { // その他は全て全角スペースとする。
      sjis_byte[sj_cnt] = ((ASCII_ZEN[0] & 0xff00)>>8);
      sjis_byte[sj_cnt+1] = ((ASCII_ZEN[0] & 0xff));
      sj_cnt += 2;
      fnt_cnt++;

    }

		yield();
  }

  *sj_length = sj_cnt;
}


//***********UTF-8コードをSPIFFS内の変換テーブルを読み出してShift-JISコードに変換****
void UTF8toSJIS::UTF8_To_SJIS_code_cnv(uint8_t utf8_1, uint8_t utf8_2, uint8_t utf8_3, uint32_t* spiffs_addrs)
{
  if(utf8_1>=0xC2 && utf8_1<=0xD1){
    //0xB0からS_JISコード実データ。0x00-0xAFまではライセンス文ヘッダ。
    *spiffs_addrs = ((utf8_1<<8 | utf8_2)-0xC2A2)*2 + 0xB0; //文字"¢" UTF8コード C2A2～、S_jisコード8191
  }else if(utf8_2>=0x80){
		uint32_t UTF8uint = (utf8_1<<16) | (utf8_2<<8) | utf8_3;

		switch(utf8_1){
			case 0xE2:
				*spiffs_addrs = (UTF8uint-0xE28090)*2 + 0x1EEC; //文字"‐" UTF8コード E28090～、S_jisコード815D
				break;
			case 0xE3:
				*spiffs_addrs = (UTF8uint-0xE38080)*2 + 0x9DCC; //スペース UTF8コード E38080～、S_jisコード8140
				break;
			case 0xE4:
				*spiffs_addrs = (UTF8uint-0xE4B880)*2 + 0x11CCC; //文字"一" UTF8コード E4B880～、S_jisコード88EA
				break;
			case 0xE5:
				*spiffs_addrs = (UTF8uint-0xE58085)*2 + 0x12BCC; //文字"倅" UTF8コード E58085～、S_jisコード98E4
				break;
			case 0xE6:
				*spiffs_addrs = (UTF8uint-0xE6808E)*2 + 0x1AAC2; //文字"怎" UTF8コード E6808E～、S_jisコード9C83
				break;
			case 0xE7:
				*spiffs_addrs = (UTF8uint-0xE78081)*2 + 0x229A6; //文字"瀁" UTF8コード E78081～、S_jisコードE066
				break;
			case 0xE8:
				*spiffs_addrs = (UTF8uint-0xE88080)*2 + 0x2A8A4; //文字"耀" UTF8コード E88080～、S_jisコード9773
				break;
			case 0xE9:
				*spiffs_addrs = (UTF8uint-0xE98080)*2 + 0x327A4; //文字"退" UTF8コード E98080～、S_jisコード91DE
				break;
			default:
				if(utf8_1>=0xEF && utf8_2>=0xBC){
					*spiffs_addrs = (UTF8uint-0xEFBC81)*2 + 0x3A6A4; //文字"！" UTF8コード EFBC81～、S_jisコード8149
					if(utf8_1==0xEF && utf8_2==0xBD && utf8_3==0x9E){
						*spiffs_addrs = 0x3A8DE; // "～" UTF8コード EFBD9E、S_jisコード8160
					}
				}
				break;
		}
  } 
}


//***********************************************
void UTF8toSJIS::SPIFFS_Flash_UTF8SJIS_Table_Read(File ff, uint32_t addrs, uint8_t* buf)
{
  if(ff){
    ff.seek(addrs,SeekSet);
		ff.read(buf, 2);
  }else{
    Serial.println(" file has not been uploaded to the flash in SPIFFS file system");
    delay(30000);
  }
}

uint8_t UTF8toSJIS::getCodeType(uint8_t *sjis_code){
  uint8_t sc0 = sjis_code[0];
  uint8_t sc1 = sjis_code[1];
  //unsigned char c2 = (unsigned char)(sjis_code[1]);
  Serial.println(String(sjis_code[0]) + " " + String(sjis_code[1]));
  //Serial.println(String(c1) + " " + String(c2));
  if ( iskanji(sc0) ){ 
    return SJ_KANJI;

  } else if ( iskana(sc0) ) {
    if ( maybedaku(sc0) && isdaku(sc1) ) return SJ_KANA_D;
    if ( maybehandaku(sc0) && ishandaku(sc1) ) return SJ_KANA_HD;
    return SJ_KANA;
  }
  return SJ_ASCII;
  
}