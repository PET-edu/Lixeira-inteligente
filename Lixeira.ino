#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// Definindo os pinos
#define TRIG_PIN 5
#define ECHO_PIN 6
#define PIR_PIN 8
#define BUZZER_PIN 9
#define SERVO_PIN_1 7
#define SERVO_PIN_2 10

// Configurações do LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Criando objetos para os servos
Servo servo1;
Servo servo2;

// Variáveis
long duration;
int distance;
unsigned long tempoAbertura;
bool tampaAberta = false;
unsigned long ultimoBip = 0;
int tempoRestante = 15;

void setup() {
  // Configurações do LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AGUARDANDO...");

  // Configuração dos pinos
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicializando os servos
  servo1.attach(SERVO_PIN_1);
  servo2.attach(SERVO_PIN_2);
  fecharTampa();
}

void loop() {

  distance = medirDistancia();

  // Se um objeto for detectado entre 2 e 15 cm
  if (distance >= 2 && distance <= 15 && !tampaAberta) {

    abrirTampa();
    bipAbrir();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ABRINDO...");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("AGUARDANDO DEPOSITO");

    tempoAbertura = millis();
    ultimoBip = tempoAbertura;
    tampaAberta = true;
  }

  if (tampaAberta) {

    // Atualiza o tempo restante
    tempoRestante = 10 - (millis() - tempoAbertura) / 1000;

    lcd.setCursor(0, 2);
    lcd.print("Tempo: ");
    lcd.print(tempoRestante);
    lcd.print("s      ");

    // Emitir um bip a cada segundo
    if (millis() - ultimoBip >= 1000) {
      bipCurto();
      ultimoBip += 1000;
    }

    if (digitalRead(PIR_PIN) == HIGH) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DEPOSITO FEITO!");

      delay(6000);

      bipFechar();
      fecharTampa();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("OBRIGADO!");

      delay(2000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("AGUARDANDO...");

      tampaAberta = false;
    }

    else if (millis() - tempoAbertura >= 10000) {

      bipErro();
      fecharTampa();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ERRO: NAO HA LIXO");

      delay(2000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("AGUARDANDO...");

      tampaAberta = false;
    }
  }
}

// Função para medir a distância com filtro
int medirDistancia() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracao == 0) {
    return 999;
  }

  int distancia = duracao * 0.034 / 2;

  if (distancia < 2 || distancia > 400) {
    return 999;
  }

  return distancia;
}

// Função para abrir a tampa
void abrirTampa() {
  servo1.write(90);
  servo2.write(90);
}

// Função para fechar a tampa
void fecharTampa() {
  servo1.write(0);
  servo2.write(0);
}

// Função para bip curto do cronômetro
void bipCurto() {
  tone(BUZZER_PIN, 800);
  delay(50);
  noTone(BUZZER_PIN);
}

// Função para bipar ao abrir
void bipAbrir() {
  for (int i = 0; i < 2; i++) {
    tone(BUZZER_PIN, 1000);
    delay(100);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

// Função para bipar ao fechar
void bipFechar() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1200);
    delay(100);
    noTone(BUZZER_PIN);
    delay(100);
  }
}

// Função para bipar caso não haja depósito de lixo
void bipErro() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 700);
    delay(300);
    noTone(BUZZER_PIN);
    delay(100);
  }
}