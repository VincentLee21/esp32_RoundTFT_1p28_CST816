
#include "Arduino.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Ticker.h>
#include "CST816.h"

#include "DEV_Debug.h"
#include "DEV_Config.h"
#include "MeterLine.h"


#ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#else
#error Target CONFIG_IDF_TARGET is not supported
#endif
#else // ESP32 Before IDF 4.0
#include "rom/rtc.h"
#endif

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

#define GFXFF 1

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite img_X = TFT_eSprite(&tft);
TFT_eSprite img_Y = TFT_eSprite(&tft);

volatile bool bUpdate = false;
// Ticker ticker;


drawLineMeter meterX, meterY;
uint16_t iTouch_X = 0, iTouch_Y = 0;
uint8_t iGesture = 0, iTmpDat0;
uint8_t iTmpDat[16];

const int meterRadius = 30;
const int meterWidth = 8;
const int meterOutlineWidth = 4;
const int meterSizeHeight = (meterRadius+meterWidth+meterOutlineWidth+1)*2;
const int meterSizeWidth = (meterSizeHeight)+30;

// CST816 touch(TP_I2C_SDA, TP_I2C_SCL, TP_IO_RST, TP_IO_INT); // sda, scl, rst, irq
CST816 touch( &Wire, TP_IO_RST, TP_IO_INT); // sda, scl, rst, irq

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200);
  #ifdef __ESP32C3__
  Debug("\n\n\nESP32-C3 RoundTFT 1.28\n");
  #else
  Debug("\n\n\nESP32-S3 RoundTFT 1.28\n");

  if( psramInit()) {
    Debug("PSRAM is correctly initialized\n");
  } else {
    Debug("PSRAM not available\n");
  }
  #endif

  // ticker.attach( 10, tcr10s);


  if (DEV_Module_Init() != 0)
    Debug("GPIO Init Fail!\n");
  else
    Debug("GPIO Init successful!\n");

  Serial.printf( "Init touch panel\n");
  touch.begin();

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_MOTIONMASK__); // slvadr, reg adr
  // Info("MotionMask # 0x%X\n", iTmpDat0);

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_IRQWIDTH__); // slvadr, reg adr
  // Info("IRQwidth # 0x%X\n", iTmpDat0);

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_NORSCANPER__); // slvadr, reg adr
  // Info("NorScanPer # 0x%X\n", iTmpDat0);

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_MOTIONS1ANGLE__); // slvadr, reg adr
  // Info("MotionS1Angle # 0x%X\n", iTmpDat0);

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_DEBOUCETIME__); // slvadr, reg adr
  // Info("DebouceTime # 0x%X\n", iTmpDat0);

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_LONGPRESSTIME__); // slvadr, reg adr
  // Info("LongPressTime # 0x%X\n", iTmpDat0);

  // iTmpDat0 = DEV_I2C0_Read_Byte(__CST816D_SLVADR__, __CST816_ADR_DISAUTOSLEEP__); // slvadr, reg adr
  // Info("DisAutoSleep # 0x%X\n", iTmpDat0);

  Debug( "Init LCD\n");
  // Setup LCD
  tft.init();
  tft.setRotation(0);
  img.createSprite(240, 240);
  img.setTextColor(TFT_BLACK, TFT_WHITE, true);
  img.fillScreen(TFT_WHITE);

  img_X.createSprite(meterSizeWidth, meterSizeHeight); // 40*2,40*2
  img_Y.createSprite(meterSizeWidth, meterSizeHeight);

  // Debug( "Init LCD done\n");

  xTaskCreate(
    TaskRead_Touch
    ,  "Task touch" // A name just for humans
    ,  4096        // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,  0 // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    ,  1  // Priority
    ,  NULL // Task handle is not used here - simply pass NULL
    );

  img.pushSprite(0, 0);

  meterX.Initialize( "CoordX", " X", &img_X, // char *name, char *appendString, TFT_eSprite *ptrImg,
                        70, meterWidth, meterOutlineWidth, // int height, int width, int outline_width
                        false, true, // , bool vertical, bool gradient,
                        0.0, 240.0, // float min_val, float max_val
                        0x0789, TFT_WHITE, 0x1062, TFT_BLACK); // uint32_t fg1_color, uint32_t fg2_color, uint32_t bar_bg_color, uint32_t bg_color);

  meterY.Initialize( "CoordY", " Y", &img_Y, // char *name, char *appendString, TFT_eSprite *ptrImg,
                        70, meterWidth, meterOutlineWidth, // int height, int width, int outline_width
                        true, true, // , bool vertical, bool gradient,
                        0.0, 240.0, // float min_val, float max_val
                        0x0789, TFT_WHITE, 0x1062, TFT_BLACK); // uint32_t fg1_color, uint32_t fg2_color, uint32_t bar_bg_color, uint32_t bg_color);

}

void TaskRead_Touch(void *pvParameters) {
  // uint32_t iDelayUS = *((uint32_t*) pvParameters);
  uint8_t tmpGesture;

  while( 1) {
   if( touch.getTouch( &iTouch_X, &iTouch_Y, &iGesture)){
      // Info( "GestureID 0x%02X\n", iGesture);
      // Info( "Pos %d,%d\n", iTouch_X, iTouch_Y);
    }
    bUpdate = true;
    usleep(10000); // 10ms
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  int x = 45, y, dy = meterSizeHeight+5;
  // int x = 40, y, dy = 30;
  String str;


  if( bUpdate) {
    bUpdate = false;
    img.fillScreen(TFT_WHITE);

    y = 40;

    // ------------------------
    // Draw X value
    meterX.Draw( (float)iTouch_X,
                    TFT_DARKGREY); // float value, uint32_t font_color, String string
    img_X.pushToSprite( &img, x, y, TFT_BLACK);
    y+=dy;


    // ------------------------
    // Draw X value
    meterY.Draw( (float)iTouch_Y, TFT_DARKGREY); // float value, uint32_t font_color, String string
    img_Y.pushToSprite( &img, x, y, TFT_BLACK);
    // y+=dy;
    // ------------------------

    switch( iGesture) {
      case 0x0: str = String( "None"); break;
      case 0x1: str = String( "S.up"); break;
      case 0x2: str = String( "S.down"); break;
      case 0x3: str = String( "S.left"); break;
      case 0x4: str = String( "S.right"); break;
      case 0x5: str = String( "Click"); break;
      case 0xB: str = String( "D.click"); break;
      case 0xC: str = String( "L.press"); break;
      default:  str = String(""); break;
    }
    #ifdef GFXFF
    img.setFreeFont( GFXFF_12);
    img.drawString(str, 150, 100, GFXFF);
    #else
    img.setFreeFont( NULL);
    img.drawString(str, 150, 100, 4);
    #endif

    img.pushSprite(0, 0);
  }
  usleep(100);
}

