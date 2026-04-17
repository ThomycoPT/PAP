#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>

char Incoming_value = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo cancela;
const int pinoServo = 12;

const int trigPins[3] = {3, 5, 7};
const int echoPins[3] = {2, 4, 6};
const int distanciaLimite = 15;

const int ledVerde[3]   = {A0, A2, 8};
const int ledVermelho[3]= {A1, A3, 9};

int lugaresOcupados = 0;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 3; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(ledVerde[i], OUTPUT);
    pinMode(ledVermelho[i], OUTPUT);
    digitalWrite(ledVerde[i], HIGH);
    digitalWrite(ledVermelho[i], LOW);
  }

  cancela.attach(pinoServo);
  cancela.write(0);

  lcd.setCursor(0, 0);
  lcd.print("Sistema Ativo!");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (Serial.available() > 0) {
    Incoming_value = Serial.read();
    if (Incoming_value == '1') abrirCancela();
    else if (Incoming_value == '0') fecharCancela();
  }

  lugaresOcupados = 0;

  for (int i = 0; i < 3; i++) {
    float distancia = medirDistancia(trigPins[i], echoPins[i]);

    if (distancia > 0 && distancia < distanciaLimite) {
      digitalWrite(ledVermelho[i], HIGH);
      digitalWrite(ledVerde[i], LOW);
      lugaresOcupados++;
    } else {
      digitalWrite(ledVermelho[i], LOW);
      digitalWrite(ledVerde[i], HIGH);
    }

    delay(50);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lugares ocupados:");
  lcd.setCursor(0, 1);
  lcd.print(lugaresOcupados);
  lcd.print(" / 3");

  delay(500);
}

float medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);

  if (duration == 0) return -1;

  float distance = duration * 0.034 / 2;
  return distance;
}

void abrirCancela() {
  cancela.write(90);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cancela aberta!");
  delay(1500);
}

void fecharCancela() {
  cancela.write(0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cancela fechada!");
  delay(1500);
}
