/*
  First atempt at serial comunications
*/
const int txClockPin = 3;
const int txPin = 4;
const int sendButtonPin = 6;
const int clockSpeed = 75;
const int crcPoly = 0x07;

char message[17] = "Hi";
int numOfBytes = 0; //Number of bytes sent over serial
byte crcValue = 0; //Store crc value here

void setup() {
  Serial.begin(9600);
  pinMode(txPin, OUTPUT);
  pinMode(txClockPin, OUTPUT);
  pinMode(sendButtonPin, INPUT);
  delay(1500);
  digitalWrite(txPin, LOW);
  digitalWrite(txClockPin, LOW);
  Serial.println("Start");
  Serial.println(message);
}

void loop() {

  if (digitalRead(sendButtonPin)) {  //Dugme je pritisnuto
    while (digitalRead(sendButtonPin)) {} //Dugme je pušteno
    crcValue = calcCrc(message, crcPoly);
    sendMessage(message); //Posšalji poruku
    sendCrc(crcValue);
  }
  
  if (Serial.available()) { //Dostupna nova poruka na serijskom portu
    readSerial(message); //Pročitaj novu poruku
  };
  
}

//funkcija za slanje poruke
void sendMessage(char *TXstring) {

  for (int byteIndex = 0; byteIndex <= strlen(TXstring); byteIndex++) { //For koji prolazi kroz svaki karakter u poruci (ako se stavi byteIndex <= strlen(TXstring) nece se poslati null character )

    char currByte = TXstring[byteIndex];

    Serial.println(TXstring[byteIndex]);
    Serial.println();

    sendByte(currByte);
    
    Serial.println();
  }

}

void sendCrc(byte messageCrc) {
  sendByte(messageCrc);
}

void sendByte(byte tmpByte) {
  for (int bitIndex = 0; bitIndex < 8; bitIndex++) { //For koji prolazi kroz svaki bit u trenutnom bajtu

  bool currBit = (0x80 >> bitIndex) & tmpByte; //Ako je na bitIndex mjestu u trenutnom bitu 1 postaviti currBit na true

    if (currBit) { //Ako je trenutni bit 1 poslati 1
      digitalWrite(txPin, HIGH);
      pulseClock();
      Serial.println('1');
    } else { //Ako je trenutni bit 0 poslati 0
      digitalWrite(txPin, LOW);
      pulseClock();
      Serial.println('0');
    }
  }
}

byte calcCrc(char *codeword, byte poly) { //Calculate CRC given the message and poly
  
  char regByte = 0;
  char bufferByte = 0;
  bool regByteCheck = false;
  
  for (int byteIndex = 0; byteIndex <= strlen(message); byteIndex++) {
    
    bufferByte = codeword[byteIndex]; //Set the buffer byte to the next byte in message
    
    for (int cycleIndex = 0; cycleIndex < 8; cycleIndex++) { //For every bit in the register byte
      regByteCheck = regByte & 0x80; //Check if the first bit is a one
      regByte = regByte << 1; //Shift register to left by one byte
      regByte = regByte | ((bufferByte << cycleIndex) & 0x80) >> 7; //Put the corresponeding bit from the buffer byte in the last place of the register
      if (regByteCheck)  {
        regByte = regByte ^ poly; //If the first bit of the register was a one XOR the poly into the register
      }
    }
  } 
    return regByte;
}

void readSerial(char *charArr) { //Read a message over serial
  
  char tmpChar;
  
  tmpChar = Serial.read();
  Serial.println(tmpChar);
  
  if (tmpChar == 0x0A) { //If recieved byte is new line, end of message
    charArr[numOfBytes] = 0;
    numOfBytes = 0;
    Serial.println("Ready to send new message");
  } else { //If not new line go one
    charArr[numOfBytes] = tmpChar; //Add current byte to message
    numOfBytes++;
  };
  
}

void printByte (byte currentByte) { //Prints out every bit of a byte
  Serial.println("");
  for (int i = 0; i < 8; i++) {
    Serial.println(bool(currentByte & (0x80 >> i)));
  }
  Serial.println("");
}

//Pulsiranje clocka
void pulseClock() {
  delay(clockSpeed / 2);
  digitalWrite(txClockPin, HIGH);
  delay(clockSpeed / 2);
  digitalWrite(txClockPin, LOW);
}