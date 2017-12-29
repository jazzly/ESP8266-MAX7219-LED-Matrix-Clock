/*
 Name:		led_Matrix_clock.ino
 Created:	2017/12/25 10:57:34
 Author:	Marco
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <time.h>
#include "num_font.h"


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

int pinCS = D8; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
char time_value[20];

// LED Matrix Pin -> ESP8266 Pin
// Vcc            -> 3v  (3V on NodeMCU 3V3 on WEMOS)
// Gnd            -> Gnd (G on NodeMCU)
// DIN            -> D7  (Same Pin for WEMOS)
// CS             -> D4  (Same Pin for WEMOS)
// CLK            -> D5  (Same Pin for WEMOS)

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int wait = 90; // In milliseconds
int count = 0;
bool showcolon = true;

int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

void setup()
{
  Serial.begin(115200);
	configTime(8 * 3600, 0, "pool.ntp.org", "2.cn.pool.ntp.org");
	matrix.setIntensity(0);	     // Use a value between 0 and 15 for brightness
	matrix.setRotation(0, 1);    // The first display is position upside down
	matrix.setRotation(1, 1);    // The first display is position upside down
	matrix.setRotation(2, 1);    // The first display is position upside down
	matrix.setRotation(3, 1);    // The first display is position upside down
}

void loop()
{
	count++;
	matrix.fillScreen(LOW);
	time_t now = time(nullptr);
	//now = mktime(localtime(&now));
	String time = String(ctime(&now));
	time.trim();
	time.substring(11, 19).toCharArray(time_value, 10);
	//matrix.drawChar(2, 0, time_value[0], HIGH, LOW, 1); // H
	//matrix.drawChar(8, 0, time_value[1], HIGH, LOW, 1); // HH

	//if (showcolon) {
	//	showcolon = false;
	//	matrix.drawChar(14, 0, time_value[2], HIGH, LOW, 1); // HH:
	//}
	//else {
	//	showcolon = true;
	//}

	//matrix.drawChar(20, 0, time_value[3], HIGH, LOW, 1); // HH:M
	//matrix.drawChar(26, 0, time_value[4], HIGH, LOW, 1); // HH:MM

	display_time(time_value, showcolon);

	if (showcolon) {
		showcolon = false;
	}
	else {
		showcolon = true;
	}

	matrix.write(); // Send bitmap to display
	delay(1000);
	if (0 == count % 60) {
    if (0 == count % 1800){
          String strout = String("count is: ") + String(count, DEC);
          Serial.println(strout);
          configTime(8 * 3600, 0, "pool.ntp.org", "2.cn.pool.ntp.org");
    }

		display_message(time); // Display time in format 'Wed, Mar 01 16:03:20 2017
	}
}

void display_time(const char * str, bool displaycolon)
{
	const uint8_t * bitmap = nullptr;
	for (size_t i = 0; i < 5; i++) {
		switch (str[i]) {
		case '1':
			bitmap = FontData[1];
			break;
		case '2':
			bitmap = FontData[2];
			break;
		case '3':
			bitmap = FontData[3];
			break;
		case '4':
			bitmap = FontData[4];
			break;
		case '5':
			bitmap = FontData[5];
			break;
		case '6':
			bitmap = FontData[6];
			break;
		case '7':
			bitmap = FontData[7];
			break;
		case '8':
			bitmap = FontData[8];
			break;
		case '9':
			bitmap = FontData[9];
			break;
		case '0':
			bitmap = FontData[0];
			break;
		case ':':
			bitmap = FontData[10];
			break;
		default:
			break;
		}

		//if (2 == i && !displaycolon) {
		//	continue;
		//}
		//matrix.drawBitmap(2 + i * 6, 0, bitmap, 8, 8, RED);

    switch (i) {
    case 0:
      matrix.drawBitmap(2, 0, bitmap, 8, 8, RED);
      break;
    case 1:
      matrix.drawBitmap(2 + 6, 0, bitmap, 8, 8, RED);
      break;
    case 2:
      if (displaycolon){
        matrix.drawBitmap(2 + 11, 0, bitmap, 8, 8, RED);
      }
      break;
    case 3:
      matrix.drawBitmap(2 + 16, 0, bitmap, 8, 8, RED);
      break;
    case 4:
      matrix.drawBitmap(2 + 22, 0, bitmap, 8, 8, RED);
      break;
    default:
      break;
    }
	}

}

void display_message(String message)
{
	for (int i = 0; i < width * message.length() + matrix.width() - spacer; i++) {
		//matrix.fillScreen(LOW);
		int letter = i / width;
		int x = (matrix.width() - 1) - i % width;
		int y = (matrix.height() - 8) / 2; // center the text vertically
		while (x + width - spacer >= 0 && letter >= 0) {
			if (letter < message.length()) {
				matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background off, reverse to invert the image
			}
			letter--;
			x -= width;
		}
		matrix.write(); // Send bitmap to display
		delay(wait / 2);
	}
}

