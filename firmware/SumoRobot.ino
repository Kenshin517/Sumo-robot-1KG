#include <Ps3Controller.h>

#define IN1 33
#define IN2 25
#define ENA 26 

#define IN3 32
#define IN4 35
#define ENB 34 

#define PWM_FREQ 5000
#define PWM_RES 8

bool overrideControl = false;

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(0, PWM_FREQ, PWM_RES); 
  ledcAttachPin(ENA, 0);

  ledcSetup(1, PWM_FREQ, PWM_RES); 
  ledcAttachPin(ENB, 1);

  // connect ke PS3 
  Ps3.begin("6C:C8:40:4F:86:5C"); // Ganti dengan MAC ESP32 
  Ps3.attach(notify);
  Ps3.attachOnConnect([]() {
    Serial.println("Controller Connected");
  });
}

void loop() {
  if (!Ps3.isConnected()) return;

  if (overrideControl) return;

  int leftY = Ps3.data.analog.stick.ly;
  int rightY = Ps3.data.analog.stick.ry;

  controlMotor(leftY, IN1, IN2, 0);   
  controlMotor(rightY, IN3, IN4, 1);  
}

void controlMotor(int value, int inA, int inB, int pwmChannel) {
  int speed = map(abs(value), 0, 128, 0, 255); 

  if (value > 15) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (value < -15) { 
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else { 
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
    speed = 0;
  }

  ledcWrite(pwmChannel, speed); 
}

void notify() {
  // Circle → Putar di tempat ke kiri
  if (Ps3.event.button_down.circle) {
    Serial.println("Putar ke kanan!");
    digitalWrite(IN1, LOW);  
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH); 
    digitalWrite(IN4, LOW);
    ledcWrite(0, 255);
    ledcWrite(1, 255);
    overrideControl = true;
  }

  // Square → Putar di tempat ke kanan
  if (Ps3.event.button_down.square) {
    Serial.println("Putar ke kiri!");
    digitalWrite(IN1, HIGH); 
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);  
    digitalWrite(IN4, HIGH);
    ledcWrite(0, 255);
    ledcWrite(1, 255);
    overrideControl = true;
  }

  // Triangle → Jalan lurus ke depan
  if (Ps3.event.button_down.triangle) {
    Serial.println("Jalan lurus!");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(0, 255);
    ledcWrite(1, 255);
    overrideControl = true;
  }

  // Cross → STOP semua motor
  if (Ps3.event.button_down.cross) {
    Serial.println("STOP semua!");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    overrideControl = true;
  }

  // L1 / R1 → Balik ke kontrol joystick
  if (Ps3.event.button_down.l1 || Ps3.event.button_down.r1) {
    Serial.println("Kembali ke kontrol joystick.");
    overrideControl = false;
  }
}