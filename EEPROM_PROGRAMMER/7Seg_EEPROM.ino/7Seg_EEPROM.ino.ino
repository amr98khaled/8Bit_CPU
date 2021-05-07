#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, ((address >> 8) | (outputEnable ? 0x00 : 0x80)));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }
   setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, (data & 0x01));
    data = (data >> 1);
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 2047; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


// 4-bit hex decoder for common anode 7-segment display
//byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };

// 4-bit hex decoder for common cathode 7-segment display
//byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };


void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);
  
  

   



  // Program data bytes
  Serial.println("Programming EEPROM");
  
  byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};

         /*Programming Positive numbers*/
  Serial.println("Programming ones place");
  //Write ones places (Neither A8 nor A9 will be set)
  for(int value=0;value<=255;value++)
  {
    writeEEPROM(value, digits[(value%10)]);
  }

  //Write tens places (A8 will be set to 1 so add 256 to the value)
  Serial.println("Programming tens place");
  for(int value=0;value<=255;value++)
  {
    writeEEPROM(value+256, digits[((value%100)/10)]);
  }

  //Write Hundreds places (A9 will be set to 1 so add 512 to the value)
  Serial.println("Programming Hundreds place");
  for(int value=0;value<=255;value++)
  {
    writeEEPROM(value+512, digits[(value/100)]);
  }

  //Write rest of memory in case : (A8+A9 will be set to 1 so add 768 to the value)
  Serial.println("Programming sign place");
  for(int value=0;value<=255;value++)
  {
    writeEEPROM(value+768, 0x00);
  }


          /*Prgramming negative numbers (add 1024 to value)*/
   Serial.println("Programming ones place (two's complement)");
   //Write ones place (A8 + A9 are 0)

   for(int value=-128;value<=127;value++)
   {
     writeEEPROM((byte)value+1024,digits[(abs(value)%10)]);
   }


   //Write tens place (A8 is 1)
   Serial.println("Programming tens place (two's complement)");
   for(int value=-128;value<=127;value++)
   {
     writeEEPROM((byte)value+1280,digits[((abs(value)%100)/10)]);
   }


   //Write Hundreds place (A9 is 1)
   Serial.println("Programming Hundreds place (two's complement)");
   for(int value=-128;value<=127;value++)
   {
     writeEEPROM((byte)value+1536,digits[(abs(value)/100)]);
   }

   //Write sign place (A8+A9 are 1)
    Serial.println("Programming sign place (two's complement)");
   for(int value=-128;value<=127;value++)
   {
     writeEEPROM((byte)value+1792,((value>=0)? 0x00:0x01));
     // if value is positive g in 7seg should be off , otherwise g should be on , g pin is connected to D0 
   }

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
 
}


void loop() {
  // put your main code here, to run repeatedly:

}
