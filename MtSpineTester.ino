#include <Arduino.h>
#include <SPI.h>
#include "HX711.h"
#include <U8g2lib.h>
#include <Math.h>

HX711 scale1;
HX711 scale2;

#define DOUT_1  3                                   
#define CLK_1  2
#define DOUT_2  4
#define CLK_2  5

const float calibration_factor_1 = -1020;
const float calibration_factor_2 = -985;

 
float weight = 0.0;
float weight1 = 0.0;
float weight2 = 0.0;
float arrowWeight = 0.0;
float arrowWeight1 = 0.0;
float arrowWeight2 = 0.0;

float spine = 0.0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

String outputText;

void setup(void) {
  scale1.begin(DOUT_1, CLK_1);
  scale2.begin(DOUT_2, CLK_2);
  Serial.begin(9600);
  u8g2.begin();

  
  scale1.set_scale(calibration_factor_1);
  scale2.set_scale(calibration_factor_2);    
  
  scale1.tare();
  scale2.tare();

  outputText = String();
  writeOutput("MT spine tester", "");
}

void loop(void) {
  
  weight1=scale1.get_units(5);
  weight2=scale2.get_units(5);
  weight = weight1 + weight2;


  if(arrowWeight > 10 && weight < -5) {

    for (int i = 3; i > 0; i--) {
      outputText = "Will reset in ";
      outputText += i;
      writeOutput("Arrow Off", outputText);
      delay(200);
      float tempWeight1=scale1.get_units(5);
      float tempWeight2=scale2.get_units(5);
      float tempWeight = tempWeight1 + tempWeight2;  
      if(arrowWeight > 10 && tempWeight > -5){      
        writeOutput("Arrow back on", "");
        return;
      }
    }
        
    writeOutput("Reseting...", "");
    weight1=0;
    weight2=0;
    weight = 0;
    arrowWeight = 0;
    scale1.tare();
    scale2.tare();
    return;
  }
  
  
  if(arrowWeight < 10 && weight < 10){
    writeOutput("Place an Arrow", "");
    return;    
  }

  if(arrowWeight < 10 && weight > 10){    
    writeOutput("Arrow Detected", "");
    delay(2000);    
    writeOutput("Measuring", "arrow weight");
    delay(500);    
    arrowWeight1=scale1.get_units(20);
    arrowWeight2=scale2.get_units(20);
    arrowWeight = arrowWeight1 + arrowWeight2;
    arrowWeight = arrowWeight * 10;
    arrowWeight = round(arrowWeight);
    arrowWeight = arrowWeight / 10;
    
    outputText = "";
    outputText += arrowWeight;
    outputText += " grams";
    writeOutput("Arrow Weights", outputText);

    scale1.tare();
    scale2.tare();    
    return;
  } 

  if(arrowWeight > 10  && weight < 10){
    writeOutput("Press to measure", "spine");
    spine = 0;
    delay(500);
    return;
  }

  if(spine > 0){
    float totalWeight = scale1.get_units(10) + scale2.get_units(10);    
    spine = calculateSpine(totalWeight);
    outputText = "";
    outputText += spine;
    writeOutput("Arrow Spine is", outputText);
    //outputText += totalWeight;
    //writeOutput("Weight is", outputText);
    delay(250);
    return;
  }

  if(arrowWeight > 10  && weight > 10){
    writeOutput("Measuring Spine", "Keep on pessing");
    delay(250);
    spine = 1;
    return;
  }
}

void writeOutput(String text1, String text2) {  
  Serial.println(text1);
  Serial.println();
  Serial.println(text2);
  Serial.println();
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_16_tr);
  u8g2.setCursor(0, 16);
  u8g2.print(text1);
  u8g2.setCursor(0, 32);
  u8g2.print(text2);
  u8g2.sendBuffer();
}

float calculateSpine(float totalWeightInGrams){
  return (0.333*2*1000)/(((totalWeightInGrams/1000)/0.45359237)*0.825419);  
}
