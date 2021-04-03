#include <M5StickCPlus.h>
#include <BleKeyboard.h>
#define PWR_BTN_SHORT_PRESS 2

BleKeyboard bleKeyboard("ZOOM", "SamuraiBiz");

enum ConfSystem {
  vaps
};
//Switch, video, share mode
String switch_mode = "mic";
int n_conf = 2;
bool on_air;
double chargingBattery = 4.9;
double maxBattery = 4.08;
double minBattery = 3.5;
double defBattery = maxBattery - minBattery;
double currentBattery;

int screen = 0;

float accX = 0;
float accY = 0;
float accZ = 0;

void show_need_connect() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.drawString("Plz Connect", 120, 70);
}

void show_onair() {
  M5.Lcd.fillScreen(TFT_GREEN);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_GREEN);
  //  M5.Lcd.setTextSize(5);
  //  M5.Lcd.setTextDatum(MC_DATUM);
  //  M5.Lcd.drawString("Execute!", 120, 70);

  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextDatum(BR_DATUM);
  if (switch_mode == "mic") {
    M5.Lcd.setTextSize(9);
    M5.Lcd.drawString("Mic", 180, 90);
  }
  else if (switch_mode == "video") {
    M5.Lcd.setTextSize(9);
    M5.Lcd.drawString("Video", 230, 90);
  }
  //  else {
  //    M5.Lcd.setTextSize(3);
  //    M5.Lcd.drawString("Share", 240, 130);
  //  }
}


void show_inmute() {
  M5.Lcd.fillScreen(TFT_DARKGREY);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_DARKGREY);
  //  M5.Lcd.setTextSize(5);
  //  M5.Lcd.setTextDatum(MC_DATUM);
  //  M5.Lcd.drawString("Execute", 120, 70);

  M5.Lcd.setTextSize(9);
  M5.Lcd.setTextDatum(BR_DATUM);
  if (switch_mode == "mic") {
    M5.Lcd.setTextSize(9);
    M5.Lcd.drawString("Mic", 180, 90);
  }
  else if (switch_mode == "video") {
    M5.Lcd.setTextSize(9);
    M5.Lcd.drawString("Video", 230, 90);
  }
  //  else if (switch_mode == "share") {
  //    M5.Lcd.setTextSize(5);
  //    M5.Lcd.drawString("Share", 120, 70);
  //  }
}

void send_mute_mic() {
  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.press('a');
  delay(100);
  bleKeyboard.releaseAll();
}

void send_off_video() {
  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.press('v');
  delay(100);
  bleKeyboard.releaseAll();
}

//void send_off_share() {
//  bleKeyboard.press(KEY_LEFT_GUI);
//  bleKeyboard.press(KEY_LEFT_SHIFT);
//  bleKeyboard.press('s');
//  delay(100);
//  bleKeyboard.releaseAll();
//}

void update_display() {
  if (on_air) {
    M5.Beep.tone(4000);
    delay(10);
    M5.Beep.mute();
    show_onair();
  } else {
    M5.Beep.tone(2000);
    delay(10);
    M5.Beep.mute();
    show_inmute();
  }
}

void setup() {
  M5.begin();
  bleKeyboard.begin();
  M5.IMU.Init();

  M5.Lcd.setRotation(3); // BtnA is left side to LCD
  M5.Beep.setBeep(4000, 100);
  //Make display daker, it is recommended more than 7
  M5.Axp.ScreenBreath(8);
  setCpuFrequencyMhz(80);

  on_air = true;
  update_display();
}

void loop() {
  M5.update();
  // Switch Screen
  int axpButton = M5.Axp.GetBtnPress();
  if (axpButton == PWR_BTN_SHORT_PRESS) {
    if ( screen == 1 ) {
      M5.Axp.ScreenBreath(8);
      screen = 0;
    } else {
      M5.Axp.ScreenBreath(0);
      screen = 1;
    }
  }

  if (M5.BtnA.wasPressed()) {
    if (bleKeyboard.isConnected()) {
      on_air = !on_air;
      if (switch_mode == "mic") {
        M5.Beep.tone(5000);
        delay(10);
        M5.Beep.mute();
        send_mute_mic();
      }
      else if (switch_mode == "video") {
        M5.Beep.tone(2000);
        delay(10);
        M5.Beep.mute();
        send_off_video();
      }
      //      else if (switch_mode == "share") {
      //        M5.Beep.tone(1000);
      //        delay(10);
      //        M5.Beep.mute();
      //        send_off_share();
      //      }

      //      update_display();
    } else {
      M5.Beep.tone(4000);
      delay(10);
      M5.Beep.mute();
      show_need_connect();
      delay(3000);
    }
    update_display();
  }

  if (M5.BtnB.wasPressed()) {
    if (switch_mode == "mic") {
      switch_mode = "video";
    }
    else if (switch_mode == "video") {
      switch_mode = "mic";
    }
    //    else if (switch_mode == "share") {
    //      switch_mode = "mic";
    //    }

    update_display();
  }
  M5.Beep.update();
  //When battery is low
  if (M5.Axp.GetWarningLeve() == 1) {
    M5.Lcd.setTextSize(3);
    M5.Lcd.drawString("LowBattery", 0, 0);
    M5.Beep.beep();
    delay(10);
  } else {
    //Get Voltage of internal battery
    M5.Lcd.setTextSize(3);
    currentBattery = M5.Axp.GetAPSVoltage();
    double battery = (currentBattery - minBattery) / defBattery * 100;
    if (battery > 100) {
      M5.Lcd.drawString("100%       ", 0, 0);
      //M5.Lcd.drawString("100%       ",0,0);
    } else if (battery < 0 ) {
      M5.Lcd.drawString("  0%", 0, 0);
      M5.Beep.beep();
    } else if (battery < 10) {
      M5.Lcd.drawString(String((int)battery) + "  %", 0, 0);
    } else {
      M5.Lcd.drawString(String((int)battery) + " %", 0, 0);
    }
    delay(100);
  }

  //Control PgeUp and PageDown
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  if ((accX * 1000) < -900) {
    Serial.println("Sending Page DOWN key...");
    M5.Beep.tone(3000);
    delay(10);
    M5.Beep.mute();
    bleKeyboard.write(KEY_LEFT_CTRL);
    bleKeyboard.write('p');
    delay(1000);
  }
  if ((accX * 1000) > 900) {
    Serial.println("Sending Page UP key...");
    M5.Beep.tone(3000);
    delay(10);
    M5.Beep.mute();
    bleKeyboard.write(KEY_LEFT_CTRL);
    bleKeyboard.write('n');
    delay(1000);
  }
}
