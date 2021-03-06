// i2c_eeprom.ino - A program to interface an Arduino with an I2C EEPROM
/*
  Copyright (C) 2015  Matthew Tunstall

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* Notes: Addressing
 The EEPROM being used is an M24M01-R 1-Mbit serial I2C EEPROM
 http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00147128.pdf

 Device Addressing

 Memory Address b7 b6 b5 b4 b3 b2 b1  b0
                 1  0  1  0 E2 E1 A16 RW
 E2/E1 are the external address pins of the chip
 RW is the read/write bit. 1 = read, 0 = write
 The memory has a 17 bit address, the 17th bit A16 is specified in the
 device address. This effectively splits the chip in half allowing
 access to two 512kbit (64kByte) sections.

 For the purposes of this script E1 & E2 are both connected to ground.

 Addresses:
                    1010 0001 - Lower 512kbit Read
                    1010 0000 - Lower 512kbit Write
                    1010 0011 - Upper 512kbit Read
                    1010 0010 - Upper 512kbit Write

 The Arduino wire library uses 7-bit addressing. The R/W bit is dropped.
 The addresses for use with the wire library become
                    0101 0000 - Lower 512kbit - 0x50
                    0101 0001 - Upper 512kbit - 0x51

*/
/* Notes: Misc
 The byte type is the same as an unsigned char and comes from the arduino style syntax.
 The EEPROM uses 256 byte pages.
 Arduino wire library has 32 byte buffer. 30 bytes when 2 byte addresses
 are used. To prevent any problems 16 byte chunks of data will be used.
*/
#include <Wire.h>
#define LED 13          // Pin 13 connected to an LED
#define MEM0 0x50       // Lower 512kbit
#define MEM1 0x51       // Upper 512kbit
//Function Prototypes
void readEEPROM();      // Reads data from EEPROM
unsigned char readEEPROMbyte(int ADDR, int MSB, int LSB);  // Read a byte from EEPROM
void readEEPROMPage(int ADDR, int pageMSB, int pageLSB);  // Read a page from EEPROM to Serial Output
int msblsb2int(unsigned char MSB, unsigned char LSB); // Convert MSB & LSB to an integer
//----------------------------------------------------------------------
void setup()
{
    Serial.begin(9600);   // start serial communications
    Wire.begin();         // join i2c bus (address optional for master)
    pinMode(LED,OUTPUT);
}

void loop()
{
    digitalWrite(LED,HIGH);
    delay(1000);
    digitalWrite(LED,LOW);
    readEEPROM();
    byte selected_byte = readEEPROMbyte(0x50,0x10,0x10);
    Serial.print("Selected Byte: ");
    Serial.println(selected_byte,HEX);
    readEEPROMPage(0x50,0x00,0x00);
    delay(1000);            // Adjust delay later based on readEEPROM() execution
}

void readEEPROM()
{
    // readingEEPROM (MEM0)
    Wire.beginTransmission(MEM0); // transmit to device MEM0 (0x50)
    Wire.write(byte(0x00));     // load start address MSB into register
    Wire.write(byte(0x00));     // load start address LSB into register
    Wire.endTransmission();     // stop transmitting

    // Wait for readings to happen
    // datasheet indicates write within 5 ms, read should be faster
    delay(5);

    // Read data from EEPROM
    Wire.requestFrom(MEM0, 2);    // request 2 bytes from MEM0 (0x50)

    // Receive data from EEPROM
    if(2 <= Wire.available())   // if two bytes were received
    {
        byte b1 = Wire.read();  // receive 1st byte
        byte b2 = Wire.read();  // receive 2nd byte

        // Write output to the serial port.
        Serial.println("EEPROM Read:");
        Serial.print("First Byte: ");
        Serial.println(b1, HEX);
        Serial.print("Second Byte: ");
        Serial.println(b2, HEX);
    }
}

unsigned char readEEPROMbyte(int ADDR, int MSB, int LSB)
{
    unsigned char b = 0x00;
    Wire.beginTransmission(ADDR); // transmit to device address ADDR
    Wire.write(byte(MSB));        // load byte MSB into register
    Wire.write(byte(LSB));        // load byte LSB into register
    Wire.endTransmission();       // stop transmitting
    // Wait for reading, should be faster than 5ms write operation
    delay(5);
    // Read byte from EEPROM
    Wire.requestFrom(ADDR,1);     // request single byte (device ADDR)
    // Receive Byte
    if(1 <= Wire.available())   // if byte received
    {
        b = Wire.read(); // read byte
    }
    return b;
}

void readEEPROMPage(int ADDR, int pageMSB, int pageLSB)
{
    //TODO: Add in a check for pageMSB & pageLSB to ensure that they are
    //      addressing a correct page boundary. (256 byte pages)
    
    int byteCount = 256;          // Total number of bytes in a page
    Wire.beginTransmission(ADDR); // transmit to device address ADDR
    Wire.write(byte(pageMSB));        // load byte MSB into register
    Wire.write(byte(pageLSB));        // load byte LSB into register
    Wire.endTransmission();       // stop transmitting
    // Wait for reading, should be faster than 5ms write operation
    delay(5);

    char tmp[32];   // Temp character buffer for address formatting.
    int address = msblsb2int(pageMSB,pageLSB); // int of page start addr

    while (byteCount != 0){
        sprintf(tmp,"%06d",address); //Format address and store in buffer.
        Serial.print(tmp);    //Print formatted address
        Serial.print(" ");
        Serial.print(" ");

        Wire.requestFrom(ADDR,16);     // request 16 bytes (device ADDR)
        if(16 <= Wire.available())   // if byte received
        {
            int dataCount = 16;
            while (dataCount != 0){
                int value = Wire.read();    // read received byte    
                Serial.print(value,HEX);    // byte to serial console
                Serial.print(" ");          // space between bytes
                if ((dataCount == 13) | (dataCount == 9) | (dataCount == 5)){
                    // Additional spaces every 4 bytes for readability
                    Serial.print(" ");
                }
                dataCount = dataCount - 1;
            }
        byteCount = byteCount - 16;

        Serial.print(" ");
        address = address + 16;
        sprintf(tmp,"%06d",address-1);
        Serial.print(tmp);
        Serial.println(""); // Add a new line for readability
        }
    }
}

int msblsb2int(unsigned char MSB, unsigned char LSB)
{
    // Make an integer by combining MSB & LSB
    int val = (MSB << 8) | LSB;
    return val;
}

