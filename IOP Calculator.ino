#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRIGGER_PIN D4 
#define ECHO_PIN D3 
#define RED_LED D5
#define GREEN_LED D6
#define BUZZER_PIN D7


#define SOUND_SPEED 343.2 

int count = 0;
double result = 0.0;
bool calculated = false;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

float measureARF(float pressure, float area, float density, float soundSpeed) {
    return (2 * pressure * pressure * area) / (density * soundSpeed);
}

float calculateDeformation(float F_arf, float stiffness) {
    if (stiffness <= 0) {
        Serial.println("Invalid stiffness value. Must be greater than 0.");
        return 0.0;
    }
    return F_arf / stiffness;
}

float calculateIOP(float deformation) {
    return 15 + (deformation * 9);
}


float measureDistance() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = (duration * SOUND_SPEED) / (2 * 10000.0);
    return distance;
}

void setup() {
    Serial.begin(115200);
    Wire.begin(D2,D1);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);

    lcd.init();
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("IOP Measurement");

    Serial.println("Ultrasound-Based IOP Measurement with Calibration");

    float pressure = 1000;     // Pressure in Pascals
    float area = 0.001;        // Initial estimated area in square meters
    float density = 1000;      // Density in kg/m^3
    float stiffness = 5000;    // Stiffness of cornea (N/m)

    
    float arf = measureARF(pressure, area, density, SOUND_SPEED);
    Serial.print("Initial Acoustic Radiation Force (ARF): ");
    Serial.println(arf);

    float deformation = calculateDeformation(arf, stiffness);
    Serial.print("Initial Deformation (Static): ");
    Serial.println(deformation);

   
    float iop = calculateIOP(deformation);
    Serial.print("Initial IOP: ");
    Serial.println(iop);
}

//testcase eluthuvom

// distance = !0;


void loop() {
    float distance = measureDistance();
    if (distance == 0.0) { // device off la iruku
        count = 0;
        result=0.0;
        calculated = false;
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("IOP: -- mmHg");
        Serial.println("Turned off");
        return;
    }

    if(calculated==true){
      lcd.setCursor(0, 0);
      lcd.print("Result IOP: ");
      lcd.setCursor(0,1);
      lcd.print(result/5.0, 2);
      lcd.print(" mmHg");
//      delay(100);
  
    }

    if (count == 5 && !calculated) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        calculated = true;   
    }
    else if(!calculated){
       lcd.setCursor(0,0);
       lcd.print("Calculating ...");
    }
   
//    if (distance <= 3.0 && !calculated) {
//        digitalWrite(RED_LED, LOW);
//        digitalWrite(GREEN_LED, HIGH);
//        count++;
//    } else if(!calculated) {
//        digitalWrite(RED_LED, HIGH);
//        digitalWrite(GREEN_LED, LOW);
//    }

    Serial.print("Measured Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    float pressure = 1000; // Pressure remains static for simplicity
    float area = 3.14159 * (distance / 2) * (distance / 2); // Area of circular contact in m^2
    float density = 1000;  // Density in kg/m^3
    float stiffness = 5000; // Stiffness (N/m)

    // Calculate ARF dynamically
    float arf = measureARF(pressure, area, density, SOUND_SPEED);
    Serial.print("Dynamic Acoustic Radiation Force (ARF): ");
    Serial.println(arf);

    // Calculate deformation dynamically
    float deformation = calculateDeformation(arf, stiffness);
    Serial.print("Dynamic Deformation: ");
    Serial.println(deformation);

    delay(1000);

    // Calculate IOP dynamically using calibration
    float iop = calculateIOP(deformation);

      if (distance <= 3.0 && !calculated) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
         Serial.print("Dynamic IOP: ");
         Serial.print(iop);
         Serial.println(" mmHg");
         result += iop;
         count++;
         delay(1000);
    } else if(!calculated) {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
    }

//    if(distance<=3.0 ){
//    
//      Serial.print("Dynamic IOP: ");
//      Serial.print(iop);
//      Serial.println(" mmHg");
//      result += iop;
//+//      lcd.clear();
////      lcd.setCursor(0, 0);
////      lcd.print("IOP: ");
////      lcd.print(iop, 2);
////      lcd.print(" mmHg");
//      delay(1000); 
//  }
}
