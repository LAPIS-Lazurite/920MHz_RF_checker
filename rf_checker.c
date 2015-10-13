
/* FILE NAME: rf_checker.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "ml7396_reg.h"

#define SUBGHZ_CH		33			// channel number (frequency)
#define CCA_LEVEL       0x65        // 3dBmのときED_VALは12になる。

static uint8_t i=0, ii=0;
static uint8_t msg[3], tmp_msg[3];
static uint8_t reg_data, max_rssi, occupancy_rate, ch_num=SUBGHZ_CH;



const unsigned char pin_order[2][4]={{10,9,8,7},{6,5,4,3}};
void get_sw_val(void)
{
	unsigned char pin;
	unsigned char digit;
	unsigned char res;
	unsigned char num;
	
	// upper digit
	res = 0;
	digit = 1;
	for(pin=0;pin<=3;pin++)
	{
		res<<=1;
		res += digitalRead(pin_order[digit][pin]);
	}
	num = res * 10;
	// lower digit
	res = 0;
	digit = 0;
	for(pin=0;pin<=3;pin++)
	{
		res<<=1;
		res += digitalRead(pin_order[digit][pin]);
	}
	num += res;
	
	ch_num=num;
}



static void create_msg(uint8_t val, uint8_t dectype, uint8_t val_len){

    if (val_len == 2) {
        Print.init(msg,sizeof(msg));
    }else{
        Print.init(msg,sizeof(msg) + 1);
    }

    if (dectype == HEX) {
        if(val<16) Print.p("0");
        Print.l(val,HEX);
    }else{
        tmp_msg[0] = val%10;
        val = val/10;
        tmp_msg[1] = val%10;
        val = val/10;
        tmp_msg[2] = val%10;

        if (val_len == 2) {
            Print.l(tmp_msg[1],DEC);
            Print.l(tmp_msg[0],DEC);
        }else{
            Print.l(tmp_msg[2],DEC);
            Print.l(tmp_msg[1],DEC);
            Print.l(tmp_msg[0],DEC);
        }
    }

    lcd.print(msg);
}



static void get_rssi(void)
{
    for(max_rssi=0, i=0, ii=0; i < 100; i++) {
        ml7396_regread(REG_ADR_ED_RSLT, &reg_data, 1);
        if (reg_data > max_rssi)  max_rssi = reg_data;
        if (reg_data > CCA_LEVEL) ii++;
        delay(4);
    }
    occupancy_rate= ii * 100 / i;
}



void setup(){
	int i;
	for(i=3;i<=10;i++)
	{
		pinMode(i,INPUT_PULLDOWN);
	}

	lcd.init();
    lcd.begin(8, 2,LCD_5x8DOTS);
    lcd.setContrast(30);
}



void loop(){

    if (ch_num >= 24 && ch_num <= 61) { 
        SubGHz.begin(ch_num, 0xABCD,  SUBGHZ_100KBPS, SUBGHZ_PWR_20MW);
    }
//  SubGHz.rxEnable(NULL);

    // 2015.07.29 Eiichi Saito CCA中シンクさせない
    reg_data = 0x00;
    ml7396_regwrite(REG_ADR_DEMSET3, &reg_data, 1);
    ml7396_regwrite(REG_ADR_DEMSET14, &reg_data, 1);

    reg_data = 0x30; // CCA無限実行
    ml7396_regwrite(REG_ADR_CCA_CNTRL, &reg_data, 1);

    reg_data = 0x06;
    ml7396_regwrite(REG_ADR_RF_STATUS, &reg_data, 1);

//  lcd.clear();
    lcd.home();
    lcd.print("100k");
    lcd.print(" ");
//  lcd.print("50k");
//  lcd.print("  ");

    get_sw_val();
    create_msg(ch_num, DEC, 2);
    lcd.print("c");

    lcd.setCursor(0, 1);
    get_rssi();

    create_msg(occupancy_rate, DEC, 3);
    lcd.print("%");
    lcd.print(" ");

    create_msg(max_rssi, HEX, 2);
    lcd.print(" ");

    reg_data = 0x40; // CCA無限実行
    ml7396_regwrite(REG_ADR_CCA_CNTRL, &reg_data, 1);

    reg_data = 0x08;
    ml7396_regwrite(REG_ADR_RF_STATUS, &reg_data, 1);

    // 2015.07.29 Eiichi Saito CCA中シンクさせない
    reg_data = 0x64;
    ml7396_regwrite(REG_ADR_DEMSET3, &reg_data, 1);
    reg_data = 0x27;
    ml7396_regwrite(REG_ADR_DEMSET14, &reg_data, 1);

	SubGHz.close();
	delay(500);
}
