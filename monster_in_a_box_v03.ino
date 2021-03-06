#include <mcpDac.h>
#include <ArduinoPins.h>
#include <WavePinDefs.h>
#include <SdInfo.h>
#include <WaveUtil.h>
#include <WaveHC.h>
#include <SdReader.h>
#include <FatReader.h>
#include <FatStructs.h>

/* #include <ArduinoPins.h> */
#include <FatReader.h>
#include <FatStructs.h>
#include <mcpDac.h>
#include <SdInfo.h>
#include <SdReader.h>
#include <WaveHC.h>
/*#include <Wavemainpage.h>*/
#include <WavePinDefs.h>
#include <WaveUtil.h>

#include "MonsterSounds.h"

#define LID_BOUNCER A1   // Connect Digital Pin on Arduino to Relay Module
#define RED_LEDS    A2   // Connect Digital Pin on Arduino to Relay Module
#define FOG_MACHINE A3  // Connect Digital Pin on Arduino to Relay Module

#define PIR_SENSOR      A0
#define PAUSE_BUTTON    A4
#define DEBUG_BUTTON    A5
 
#define FRAMERATE          20
#define FRAMEDELAY         1000/FRAMERATE

#define WAKE_SECONDS_MIN    3 // Seconds
#define WAKE_SECONDS_MAX    5 // Seconds
#define WAKE_DELAY         30 // Seconds

#define WAKE_FRAMES_MIN     WAKE_SECONDS_MIN  * FRAMERATE
#define WAKE_FRAMES_MAX     WAKE_SECONDS_MAX * FRAMERATE
#define WAKE_DELAY_FRAMES   WAKE_DELAY  * FRAMERATE

MonsterSounds sounds;

void setup() 
{  
  // initialize serial communication:
  Serial.begin(9600);
  
  // Setup all the relay Pins
  pinMode(LID_BOUNCER, OUTPUT);
  pinMode(RED_LEDS,    OUTPUT);
  pinMode(FOG_MACHINE, OUTPUT);
  

  // Force all Effects to OFF
  stopAllEffects();

  pinMode(PIR_SENSOR,      INPUT);

  pinMode(PAUSE_BUTTON,    INPUT_PULLUP);
  pinMode(DEBUG_BUTTON,    INPUT_PULLUP);


  sounds.initialize(); // Monster Sounds
  
  sounds.playSystemReady();delay(1000);
  
  Serial.println();
  Serial.print("*** System Ready ***");
  Serial.println();
  
}

boolean monsterIsAwake = false;
int pauseCountdown         = 0;
int lidBounceCountdown     = 0;
int flashRedLightCountdown = 0;
int smokeCountdown         = 0;
int reawakeCountdown       = 0;

void loop() 
{ 
  if (pauseCountdown > -1 ) pauseCountdown--;
  
  // NOTE: All buttons are using pullups, thus LOW means the button is PRESSED
  
  if (pauseCountdown > 0 || digitalRead(PAUSE_BUTTON) == LOW) 
  {
    if (pauseCountdown <= 0 && digitalRead(PAUSE_BUTTON) == LOW)  Serial.println("PAUSED");
   
    stopAllEffects();
    sounds.stopAll();
    reawakeCountdown = WAKE_DELAY_FRAMES;

    if (digitalRead(PAUSE_BUTTON) == LOW) pauseCountdown = 50;
    
  
  } else {
    if (reawakeCountdown ==0) Serial.println("ARMED");
    if ((digitalRead(PIR_SENSOR) == HIGH) && reawakeCountdown <=0)   
    {
      wakeMonster();
    }
  
    if (monsterIsAwake) 
    {
      processAnimation();
    
    } else {
     
      sounds.playSnore();
      
      if (reawakeCountdown > -1 ) reawakeCountdown--;
    
    }
  }
  
  delay(FRAMEDELAY);
}


/*
 * Wake Monster
 */
void wakeMonster() 
{
  if (!monsterIsAwake)
  {
    int activityDuration = random(WAKE_FRAMES_MIN, WAKE_FRAMES_MAX); // this is how long the monster will be active

    Serial.print("WAKE UP : ");
    Serial.print(WAKE_FRAMES_MIN);
    Serial.print(" : ");
    Serial.print(WAKE_FRAMES_MAX);
    Serial.print(" : ");
    Serial.println(activityDuration);
    
    reawakeCountdown = isDebug() ? 0 : WAKE_DELAY_FRAMES;    // force a delay between wake cycles
    Serial.print("******"); 
    Serial.print(reawakeCountdown); 
    Serial.println( "******");
    
    monsterIsAwake = true;

    bounceLid(activityDuration);
    flashRedLight(activityDuration);
    activateSmoke(activityDuration);    // Hold the smoke button for 1 second 

    sounds.playRoar();
    
  }
}

 
/*
 * 
 */
void processAnimation()
{ 
  if (lidBounceCountdown % 10 == 0) {
    Serial.print(lidBounceCountdown);
    Serial.print("  -  ");
    Serial.print(flashRedLightCountdown);
    Serial.print("  -  ");
    Serial.print(smokeCountdown);
    Serial.println(" ");
  }
  
  monsterIsAwake = lidBounceCountdown > 0 || flashRedLightCountdown > 0 || smokeCountdown > 0;
  
  if (lidBounceCountdown <= 0) 
  {
    bounceLid(0);
  } else {
    lidBounceCountdown--;
  }
  
  if (flashRedLightCountdown <= 0) 
  {
    flashRedLight(0);
  } else {
    flashRedLightCountdown--;
  }
  
  if (smokeCountdown <= 0) 
  {
    activateSmoke(0);
  } else {
    smokeCountdown--;
  }
  
}


/*
 * Disable all of the Effects 
 * This effectively turns the monster off
 */
void stopAllEffects()
{
  openRelay( LID_BOUNCER );
  openRelay( RED_LEDS );
  openRelay( FOG_MACHINE );
  
  
  lidBounceCountdown     = 0;
  flashRedLightCountdown = 0;
  smokeCountdown         = 0;
  reawakeCountdown       = 0;

  monsterIsAwake = false;
}


/*
 * Bounce the box lid
 *     duration of 0 means STOP
 */
void bounceLid(int duration)
{
  if (duration <= 0) 
  {
    openRelay(LID_BOUNCER);    
    
  } else {
    // start the lid bouncing
    closeRelay(LID_BOUNCER);
    
    // set a timeout to stop after duration
    lidBounceCountdown = duration;
  }
}


/*
 * Flash Red Lights
 *     duration of 0 means STOP
 */
void flashRedLight(int duration)
{
  if (duration <= 0) 
  {
    openRelay(RED_LEDS);
    
  } else {
    // start the light flashing
    closeRelay(RED_LEDS);
    
    // set a timeout to stop after duration
    flashRedLightCountdown = duration;
  }
  
}

/*
 * Activate Smoke
 */
 void activateSmoke(int duration)
 {
  // 'press' the smoke button
  // duration should be a fixed amount of time needed for the machine to respond to the action
  // set a timeout to stop after duration
  
  if (duration <= 0) 
  {
    openRelay(FOG_MACHINE);
    
  } else {
    // start the light flashing
    closeRelay(FOG_MACHINE);
    
    // set a timeout to stop after duration
    smokeCountdown = duration;
  }
 }

/* 
 *  Sets the Normally Open (NO) terminal to OPEN
 *  Normally Closed will become Closed
 */
 void openRelay(int channel)
 {
  digitalWrite(channel, HIGH);
 }

/* 
 *  Sets the Normally Open (NO) terminal to Closed.
 *  Normally Closed will become OPEN
 */
 void closeRelay(int channel)
 {
  digitalWrite(channel, LOW);
 }

/*
 * 
 */
bool isDebug()
{
  return digitalRead(DEBUG_BUTTON) == LOW;
}


