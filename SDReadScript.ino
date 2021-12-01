#include <SPI.h>
#include <SD.h>

const int MOSIPIN = 11;
const int MISOPIN = 12;
const int CLK = 13;
const int slaveSelectPin = 10;
const int resetPin = 2;


const int AudioOut = 7;
const int AudioPlay = 2;

File myFile;
void init_sd(){
  Serial.print("Initializing SD card...");

  if (!SD.begin(slaveSelectPin)) {
    Serial.println("initialization failed!");
    while (1000);
  }
  Serial.println("initialization done.");
}

void init_audio_buffer(){
  Audio.begin(88200, 100);
}


void setup() {
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(MOSIPIN,OUTPUT);
  pinMode(MISOPIN,INPUT);
  pinMode(resetPin, INPUT);
   // set the slaveSelectPin as an output:
  Serial.begin(9600);
  SPI.begin();
  init_sd();
  init_audio_buffer();
  delay(1000);


}
void loop(){
  Serial.println(digitalRead(resetPin));
  if(digitalRead(resetPin) == HIGH){
  sd_check();

  delay(200);
  /*if(){
    while(digitalRead(resetPin) == HIGH){
      delay(200);
      Serial.println("Suspending Program");
    }*/
  }
}



void audio_play(){//play a sound on the speaker
  
  int count = 0;

  // open wave file from sdcard

  File mySound = SD.open("test.wav");

  if (!mySound) {

    // if the file didn't open, print an error and stop

    Serial.println("error opening test.wav");

    while (true);

  }

  const int S = 1024; // Number of samples to read in block

  short buffer[S];

  Serial.print("Playing");

  // until the file is not finished

  while (mySound.available()) {

    // read from the file into buffer

    mySound.read(buffer, sizeof(buffer));

    // Prepare samples

    int volume = 1024;

    Audio.prepare(buffer, S, volume);

    // Feed samples to audio

    Audio.write(buffer, S);

    // Every 100 block print a '.'

    count++;

    if (count == 100) {

      Serial.print(".");

      count = 0;

    }

  }

  mySound.close();

  Serial.println("End of file. Thank you for listening!");

  while (true) ;
}
}

void sd_check(){
 // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.flush();
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    Serial.flush();
  }
  delay(1);

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while(myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
}/*
void init_codec()
{
  Wire.begin();
  writeCodec(0x0, 0x0); //  select page 0
  writeCodec(0x01, 0x01); // soft reset
  writeCodec(0x04, 0x03); // pll clock in
  writeCodec(0x05, 0x91); // pll power up
  writeCodec(0x06, 0x08); // j = 8
  writeCodecDual(0x07, 0x0, 0x0); // D = 0000
  writeCodec(0x1B, 0x0); //  i2s, wordlength 16
  writeCodec(0x0B, 0x84); // NDAC is powered and set to 4
  writeCodec(0x0C, 0x84); // MDAC is powered and set to 4
  writeCodecDual(0x0D, 0x0, 0x80); // DOSR = 128, DOSR(9:8) = 0, DOSR(7:0) = 128

  writeCodec(0x74, 0x0); //  DAC -> volume control thru pin disable
  writeCodec(0x44, 0x0); // DAC -> DRC disable
  writeCodec(0x41, 0xD4); // DAC -> -22dB gain left
  writeCodec(0x42, 0xD4); // DAC -> -22dB gain right

  writeCodec(0x0, 0x01); // Select page 1
  writeCodec(0x21, 0x4E); // De-pop, power on = 800 ms, step time = 4ms
  writeCodec(0x1F, 0xC2); // HPL and HPR powered up
  writeCodec(0x23, 0x44); // LDAC routed to HPL, RDAC routed to HPR
  writeCodec(0x28, 0x06); // HPL unmute annd gain = 0dB
  writeCodec(0x29, 0x06); // HPR unmute annd gain = 0dB
  writeCodec(0x2A, 0x1C); // unmute class D left
  writeCodec(0x2B, 0x1C); // unmute class D right
  writeCodec(0x20, 0xC6); // power up class D drivers
  writeCodec(0x24, 0x92); // enables HPL output analog volume, set = -9 dB
  writeCodec(0x25, 0x92); // enables HPR output analog volume, set = -9 dB
  writeCodec(0x26, 0x92); // enables HPL output analog volume, set = -9 dB
  writeCodec(0x27, 0x92); // enables HPR output analog volume, set = -9 dB

  writeCodec(0x0, 0x0); // select page 0
  // select DAC DSP Processing Block PRB_P11
  writeCodec(0x3C, 0x0B); //
  writeCodec(0x00, 0x08); //
  writeCodec(0x01, 0x04); //
  writeCodec(0x00, 0x00); //
  writeCodec(0x3F, 0xD6); // power up DAC left and right channels (soft step disable)
  writeCodec(0x40, 0x0); // unmute DAC left and right channels
  Wire.end();

}

void HandleSPI(){
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
}

/*void writeCodec(uint8_t registerVal, uint8_t dataVal)
{
  Wire.beginTransmission(codecAddress);
  Wire.write(registerVal); // set the address to write to
  Wire.write(dataVal); // set the data to write
  Wire.endTransmission(); // end
}

void writeCodecDual(uint8_t registerVal, uint8_t data0Val, uint8_t data1Val)
{
  Wire.beginTransmission(codecAddress);
  Wire.write(registerVal); // set the address to write to
  Wire.write(data0Val); // set the data to write
  Wire.write(data1Val); // set the data to write
  Wire.endTransmission(); // end
}*/
