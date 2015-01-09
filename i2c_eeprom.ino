// i2c_eeprom.ino - A program to interface an Arduino with an I2C EEPROM
//
//  Copyright (C) 2015  Matthew Tunstall
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <Wire.h>
#define LED 13          // Pin 13 connected to an LED

void setup()
{
    Serial.begin(9600);   // start serial communications
    Wire.begin();         // join i2c bus (address optional for master)
    pinMode(LED,OUTPUT);
}

void loop()
{
    digitalWrite(LED,HIGH);
    delay(4000);
    digitalWrite(LED,LOW);
    readEEPROM();
    delay(4000);            // Adjust delay later based on readEEPROM() execution
}

void readEEPROM()
{
    // readingEEPROM
}
