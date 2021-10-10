/* NOTE ABOUT ARDUINO PINS
 * Pins 13, 12, 11 are always used by the SD card (they are the only pins that have a high speed SPI interface). 
 * Then there are 5 other pins used to talk to the DAC and SD card, but they can be set to connect to any arduino pin. 
 * However, by default, the library is configured to use pins 10 (for SD card) and pins 2, 3, 4 and 5 for the DAC. 
 * To chanage these pins requires modifying the library - the pins are referenced by their 'hardware' pin names (ie PORTD, etc) not by arduino pins. 
 * That means pins 6, 7, 8, 9 and the 6 analog in pins (also known as digital i/o pins 14-20) are available.
 * 
 */
 
/* #include <Arduino.h> */
#include <Wire.h>
#include <WaveHC.h>
#include <WaveUtil.h>

    static char roar0[] = "ROAR0.wav";
    static char roar1[] = "ROAR1.wav";
    static char roar2[] = "ROAR2.wav";
    static char * roarSounds[] = {roar0, roar1, roar2};
    
    static char sleep0[] = "SNORE0.wav";
    static char sleep1[] = "SNORE1.wav";
    static char sleep2[] = "SNORE2.wav";
    static char * sleepSounds[] = {sleep0, sleep1, sleep2};
    
    int previousRoarSound = -1;
  
class MonsterSounds 
{
  private:  

    SdReader  card; // This object holds the information for the card
    FatVolume vol;  // This holds the information for the partition on the card
    FatReader root; // This holds the information for the volumes root directory
    FatReader file; // This object represent the WAV file for a phrase
    WaveHC    wave; // A single wave object -- only one sound is played at a time
    
    void playfile(char *name);

  public:
    void initialize(); 
    void playSystemReady();
    void playRoar();    
    void playSnore();    
    void stopAll();
};
