#include "U8glib.h"
#include <ArduPLCnano.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);

ArduPLCnano plc;
long int screen_timer = 0, activation_timer = 0, sequece_timer = 0;
int arrow = 0, sequece_step = 0;
bool initialization = false, shutdown = false;

void setup() {
  // Turn off the relays
  plc.relayWrite(RELAY1,LOW);
  plc.relayWrite(RELAY4,LOW);
  // Init the screen
  u8g.begin();
  u8g.setFont(u8g_font_9x15Br);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  // Draw a welcome screen
  u8g.firstPage();
  do{
    u8g.drawBox(0, 15, 128, 40);
    u8g.setDefaultBackgroundColor();
    u8g.drawStr(35,22, "Control");
    u8g.drawStr(55,37, "SAI");
    u8g.setDefaultForegroundColor();
    
  } while( u8g.nextPage() );
  
  delay(2000);
}

void loop() {
  // Update screen
  screen();
  // Check sequeces
  initialitationSequence();
  shutdownSequence();
  // Verify buttons
  buttons();
}

void screen(){
  // Move the selection arrow up or down
  if(plc.digitalRead(BUTTON_UP)){
    arrow--;    
    delay(200);
  }
  else if(plc.digitalRead(BUTTON_DOWN)){
    arrow++;
    delay(200);
  }
  // Check for arrow limits
  if(arrow < 0)
    arrow = 3;
  else if (arrow > 2)
    arrow = 0;
  // Update the screen
  if(millis() - screen_timer >= 40){
    screen_timer = millis();
    u8g.firstPage();
    do{
      // Draw the menu
      u8g.setDefaultForegroundColor();
      u8g.drawStr(15,0, (plc.digitalRead(RELAY1)) ? "Encender" : "Apagar");
      u8g.drawStr(15,15,"SAI");
      u8g.drawStr(15,30,"Salida");
      u8g.drawStr(0,arrow*15,">");
      // Draw circle or circunference to indicate the state of the relay
      if(plc.digitalRead(RELAY1))
        u8g.drawCircle(110, 20, 5);
      else
        u8g.drawDisc(110, 20, 5);

      if(plc.digitalRead(RELAY4))
        u8g.drawCircle(110, 35, 5);
      else
        u8g.drawDisc(110, 35, 5);
    } while( u8g.nextPage() );
  } 
}

void initialitationSequence() {
  if (initialization && sequece_step == 0){
    plc.relayWrite(RELAY1, HIGH);
    sequece_step++;
  } else if (initialization && sequece_step == 1 && millis() - sequece_timer >= 5000){
    plc.relayWrite(RELAY4, HIGH);
    sequece_step++;
  } else if (initialization && sequece_step == 2 && millis() - sequece_timer >= 5100){
    plc.relayWrite(RELAY4, LOW);
    sequece_step++;
    initialization = false;
  }
}

void shutdownSequence() {
  if (shutdown && sequece_step == 0){
    plc.relayWrite(RELAY4, HIGH);
    sequece_step++;
  } else if (shutdown && sequece_step == 1 && millis() - sequece_timer >= 100){
    plc.relayWrite(RELAY4, LOW);
    sequece_step++;
  } else if (shutdown && sequece_step == 2 && millis() - sequece_timer >= 5000){
    plc.relayWrite(RELAY1, LOW);
    sequece_step++;
    shutdown = false;
  }
}

void buttons() {
  if(millis() - activation_timer >= 1000){
    if(plc.digitalRead(BUTTON_RIGHT)){
      switch(arrow){ 
        case 0:
          sequece_timer = millis();
          sequece_step = 0;
          if (plc.relayRead(RELAY1)) {
            shutdown = true; 
          } else {           
            initialization = true;  
          }
          break;
        case 1:
          plc.relayWrite(RELAY1, !plc.relayRead(RELAY1));
          break;
        case 2:
          plc.relayWrite(RELAY4,  !plc.relayRead(RELAY4));
          break;
      }
      activation_timer = millis();
    }
  }
}


