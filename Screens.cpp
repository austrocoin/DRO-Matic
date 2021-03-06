/*
*  DROMatic.ino
*  DROMatic OS Core
*  Devin R. Olsen - Dec 31, 2016
*  devin@devinrolsen.com
*/

#include "Screens.h"
#include "Core.h"
#include "Menus.h"
#include "DatesTime.h"
#include "Crops.h"
#include "Sessions.h"

String screenName;
vector<vector<vector<byte>>> matrix;
int cursorX, cursorY;
int currentAlphaIndex = 0;

byte upArrow[8] = {
	B00000,
	B00100,
	B01110,
	B11111,
	B00100,
	B00100,
	B00100,
	B00000
};

byte downArrow[8] = {
	B00000,
	B00100,
	B00100,
	B00100,
	B11111,
	B01110,
	B00100,
	B00000
};

byte infinityLeft[8] = {
	B00000,
	B00000,
	B00110,
	B01001,
	B01001,
	B00110,
	B00000,
	B00000
};

byte infinityRight[8] = {
	B00000,
	B00000,
	B00110,
	B01001,
	B01001,
	B00110,
	B00000,
	B00000
};

byte infintyRight[8] = {
	B00000,
	B00000,
	B01100,
	B10010,
	B10010,
	B01100,
	B00000,
	B00000
};

void exitScreen(){
	menusHistory.pop_back();
	menuIndex = 0;
	screenName = "";
	tmpFile = SD.open("dromatic/" + cropName + "/" + getMenuHistory());
	getDirectoryMenus(tmpFile);
	lcd.clear();
	lcd.noBlink();
	tmpFile.close();
	printDisplayNames(menus.front());
	printScrollArrows();
	delay(350);
}

void openHomeScreen(bool sessionTuring = false){
	captureDateTime();
	char monthsBuffer[8];
	lcd.clear();

	//hour					//minute		  //AM/PM											//Month														//Day
	lcd.print(tmpDisplay[2] + F(":") + tmpDisplay[3] + tmpDisplay[4] + F(" ") + strcpy_P(monthsBuffer, (char*)pgm_read_word(&(months[rtc.getTime().mon-1]))) + F(" ") + tmpDisplay[1]);
	lcd.setCursor(0, 1);
	lcd.print(F("PPM:"));
	lcd.print((int)(analogRead(A1) * 5.00));
	lcd.print(F(" PH:"));
	lcd.print((float)analogRead(A2) * 14.0 / 1024);
	lcd.home();
	lcd.noBlink();
	if (sessionTuring == true){
		turing(); //the heart of it all, thank you Allen
		//expireSessions();
	}
}

void screenMatrix(){
	byte row0Size = matrix[0].size();
	byte row1Size = matrix[1].size();

	byte frontFrontFront = matrix.front().front().front();
	byte frontBackBack = matrix.front().back().back();

	byte backBackBack = matrix.back().back().back();
	byte backFrontFront = matrix.back().front().front();
	//Check Top Row
	if (cursorY == 0){
		if (cursorX < frontFrontFront){
			cursorX = backBackBack;
			cursorY = 1;
		}
		else if (cursorX > frontBackBack){
			cursorY = 1;
			cursorX = backFrontFront;
		}
		for (int i = 0; i < row0Size; i++){ //top columns
			if (cursorX < matrix.front()[i].front() && cursorX > matrix.front()[i - 1].back() && Key == 408 && cursorY == 0){
				cursorX = matrix.front()[i - 1].back();
			}
			if (cursorX > matrix.front()[i].back() && cursorX < matrix.front()[i + 1].front() && Key == 0 && cursorY == 0){
				cursorX = matrix.front()[i + 1].front();
			}
		}
	}
	else if (cursorY == 1){
		if (cursorX < backFrontFront){
			cursorX = frontBackBack;
			cursorY = 0;
		}
		else if (cursorX > backBackBack){
			cursorY = 0;
			cursorX = frontFrontFront;
		}
		for (int i = 0; i < row1Size; i++){ //top columns
			if (cursorX < matrix.back()[i].front() && cursorX > matrix.back()[i - 1].back() && Key == 408 && cursorY == 1){
				cursorX = matrix.back()[i - 1].back();
			}
			if (cursorX > matrix.back()[i].back() && cursorX < matrix.back()[i + 1].front() && Key == 0 && cursorY == 1){
				cursorX = matrix.back()[i + 1].front();
			}
		}
	}
	lcd.setCursor(cursorX, cursorY);
}

void printScrollArrows(){
	lcd.setCursor(15, 0);
	lcd.write(byte(0));
	lcd.setCursor(15, 15);
	lcd.write(byte(1));
	lcd.home();
}

void printDisplayNames(String menu){
	lcd.home();
	bool hasMatch = false;
	const byte isChannel = strstr(menu.c_str(), "SYSCH") != NULL;
	const byte isSession = strstr(menu.c_str(), "CHSES") != NULL;
	const String index = String(menuIndex + 1);
	if (isChannel){
		lcd.print(F("SYSTEM"));
		lcd.setCursor(0, 1);
		lcd.print(F("CHANNEL "));
		lcd.print(index);
		currentChannelIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}
	else if (isSession) {
		lcd.print(F("CHANNEL"));
		lcd.setCursor(0, 1);
		lcd.print(F("SESSION "));
		lcd.print(index);
		currentSessionIndex = menuIndex + 1;
		lcd.home();
		hasMatch = true;
	}
	else {
		byte i;
		for (i = 0; i < 18; i++){
			char match1Buffer[18];
			char match2Buffer[18];
			char match3Buffer[18];
			String match1 = strcpy_P(match1Buffer, (char*)pgm_read_word(&(displayNames[i][0])));
			String match2 = strcpy_P(match2Buffer, (char*)pgm_read_word(&(displayNames[i][1])));
			String match3 = strcpy_P(match2Buffer, (char*)pgm_read_word(&(displayNames[i][2])));

			if (menu == match1){
				hasMatch = true;
				lcd.print(match2);
				lcd.setCursor(0, 1);
				lcd.print(match3);
				break;
			}
		}
	}
	if (hasMatch == false){
		lcd.print(menu);
	}
}
