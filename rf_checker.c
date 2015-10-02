/* FILE NAME: rf_checker.c
 * The MIT License (MIT)
 * 
 * Copyright:	(c) 2015 Lapis Semiconductor
 * Author: 		Naotaka Saito
 * Division:	New Business Development Project
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


const unsigned char pin_order[2][4]={{10,9,8,7},{6,5,4,3}};
unsigned char get_sw_val(void)
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
	
	return num;
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
    lcd.print("ÃÞ·À!!");
}

void loop(){
	unsigned char num;
	uint8_t msg[16];
	Print.init(msg,sizeof(msg));
	
	num = get_sw_val();
	
	if(num<10)
	{
		Print.p("0");
	}
	Print.l(num,DEC);
    lcd.setCursor(0, 1);
    lcd.print(msg);
	delay(100);
}
