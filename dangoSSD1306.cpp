
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include  <math.h>
#include "dangoSSD1306.h"
#include "kanji.h"
#include "shiftJIS.h"
uint8_t OLED_buff[1024];
void oledDraw8(char dx,char dy,const uint8_t  bmp[])
{
	char j = 0;
	char hy = (dy / 8);
	char tmp_buff = 0x00;
	for (int i = 0;i < 8;i++)
	{
		tmp_buff = OLED_buff[(dx + (hy * 128)) + i];
		OLED_buff[(dx + (hy * 128)) + i] = ((tmp_buff & (0xFF >> (8 - (dy % 8)))) + ((bmp[j++]) << dy % 8));
	}
	j = 0;
	for (int i = 0;i < 8;i++)
	{
		tmp_buff = OLED_buff[(dx + ((hy + 1) * 128)) + i];
		OLED_buff[(dx + ((hy + 1) * 128)) + i] = ((tmp_buff & (0xFF << ((dy % 8)))) + ((bmp[j++]) >> (8 - (dy % 8))));
	}
}
void oledInit(void)
{
        i2c_init(I2C_PORT, 1000*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    i2c_write_blocking(I2C_PORT,OLED_ADD,initCommand0,30,false);
    i2c_write_blocking(I2C_PORT,OLED_ADD,initCommand1,13,false);

}
void oledSend(void)
{
    uint8_t sendByte[1025];
    sendByte[0] = 0x40;
    for (int i = 0;i < 1024;i ++)
    {
        sendByte[i + 1] = OLED_buff[i];
    }
    i2c_write_blocking(I2C_PORT,OLED_ADD,sendByte,1025,false);
}
void oledDrawFill(const char bmp[])
{
	uint16_t j = 0;
	for (int i = 0;i < 1024;i++)
	{
		OLED_buff[i] = bmp[j++];
	}
}
void oledPixel(char dx, char dy, char po)
{
	char tmp_buff = 0x00;
	char y1 = dy / 8;
	if (po == 0)
	{
		tmp_buff = OLED_buff[(dx + (y1 * 128))] & ~(0b00000001 << (dy % 8));
	}
	if (po == 1)
	{
		tmp_buff = OLED_buff[(dx + (y1 * 128))] | (0b00000001 << (dy % 8));
	}
	OLED_buff[(dx + (y1 * 128))] = tmp_buff;
}
void oledLine(char x1, char y1, char x2, char y2) 
{
  char dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
  char dy = abs(y2-y1), sy = y1<y2 ? 1 : -1;
  char err = (dx>dy ? dx : -dy)/2, e2;
  for(;;){
     oledPixel(x1,y1,1);
     if (x1==x2 && y1==y2) break;
     e2 = err;
     if (e2 >-dx) { err -= dy; x1 += sx; }
     if (e2 < dy) { err += dx;y1 += sy; }
  }
}
void oledBox(char x1, char y1, char x2, char y2)
{
	for (char i = x1; i <= x2; i++)
	{
		oledPixel(i, y1,1);
		oledPixel(i, y2, 1);
	}
	for (char i = y1; i <= y2; i++)
	{
		oledPixel(x1, i, 1);
		oledPixel(x2, i, 1);
	}
}
void oledClear(char dat)
{
	for (int i = 0;i < 1024;i++)
	{
		OLED_buff[i] = dat;
	}
}
/*
oledChar(x,y,char)
yが8単位でしかつかえんから多分使わん
*/
void oledChar(char dx, char dy,char ch) 
{
    ch -= 32;
    for (char i = 0; i < 6; i++)
    {
        OLED_buff[(dy * 128) + dx + i] = (font8[ch * 6 + i]);

    }
}
/*
oledString(x,y,string)
上のやつの文字列版
ポインタ変数を増やしていくやつ 終端文字\0になったら終了する
ここまで書いておいて多分使わん
*/
void oledString(char dx, char dy,char* str) 
{
	while (*str) {
		oledChar(dx+=6,dy,*str++);
	}
}
/*
oledFont8(x,y,fontWidth,startAdd,font)
font[]のstartAddからfontWidth分x,yに描く
*/
void oledFont8(char dx,char dy,char fontSize,int startAdd,const uint8_t bmp[])
{
	char j = 0;
	char hy = (dy / 8);
	char tmp_buff = 0x00;
	for (int i = 0;i < fontSize;i++)
	{
		tmp_buff = OLED_buff[(dx + (hy * 128)) + i];
		OLED_buff[(dx + (hy * 128)) + i] = ((tmp_buff & (0xFF >> (8 - (dy % 8)))) + ((bmp[startAdd + j++]) << dy % 8));
	}
	j = 0;
	for (int i = 0;i < fontSize;i++)
	{
		tmp_buff = OLED_buff[(dx + ((hy + 1) * 128)) + i];
		OLED_buff[(dx + ((hy + 1) * 128)) + i] = ((tmp_buff & (0xFF << ((dy % 8)))) + ((bmp[startAdd + j++]) >> (8 - (dy % 8))));
	}
}
/*
oledFullString
ひらがなと漢字対応!
ソースコードはUTF-8で保存してください
*/
void oledFullString(char dx,char dy,const char str[])
{
	int strC = 0;
	while (1) {
		if (str[strC] == 227)
		{
			//ひらがなデコード
			uint16_t drawAdd = ((uint8_t)str[strC + 1] << 8);
			drawAdd |= str[strC + 2];
			drawAdd -= 33153;
			if (drawAdd >= 0xFF)
			{
				drawAdd -= 0xC0;
			}
			oledFont8(dx,dy,8,drawAdd * 8,font_jp_8);
			strC += 3;
			dx += 8;
		}
		else if (str[strC] >= 228)
		{
			//漢字でコード
			uint16_t cByte[3];
            cByte[0] = str[strC] & 0b00001111;
            cByte[1] = str[strC + 1] & 0b00111111;
            cByte[2] = str[strC + 2] & 0b00111111;
            uint16_t mojiCode;
			
            mojiCode = cByte[0] << 12 | cByte[1] << 6 | cByte[2];
			printf("mojiCode:%x\n",mojiCode);
			uint16_t drawAdd = shiftJIS[mojiCode - 0x4E00];
			printf("%d,",drawAdd);
			oledFont8(dx,dy,8,drawAdd * 8,kanji);
			strC += 3;
			dx += 8;
		}
		else
		{
			int drawAdd = str[strC] - 32; 
			oledFont8(dx,dy,6,drawAdd * 6,font8);
			strC += 1;
			dx += 6;
		}

		if (str[strC] == 0)
		{
			break;
		}
	}
}
/*
oledNumber(x,y,Number)
数字Numberをx,yに表示する
*/
void oledNumber(char dx, char dy, int deg)
{
	int countDeg = 1;
	while (1)
	{
		//桁数(1,10,100..を求める)
		if (deg <= countDeg * 10)
		{
			break;
		}
		else
		{
			countDeg *= 10;
		}
	}
	while (1)
	{
		oledFont8(dx,dy,6, ((deg / countDeg) % 10 + 16) * 6,font8);
		if (countDeg >= 10)
		{
			dx += 6;
			countDeg /= 10;
		}
        else
        {
            break;
        }
	}
}