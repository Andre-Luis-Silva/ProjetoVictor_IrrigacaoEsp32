#include <DHT.h>
#include <LiquidCrystal.h>

#define DHT_PIN_4 4 // Define o pino 4 para leitura do DHT11
#define RELE1 33  // Define o pino 33 para acionamento do relé 1
#define RELE2 32  // Define o pino 32 para acionamento do relé 2

LiquidCrystal lcd(16, 17, 5, 18, 19, 21);

DHT dht(DHT_PIN_4, DHT11);

/* Variáveis float */
float umidadeDoAr = 0;
float tempC = 0;

/* Variáveis bool */
bool flagBomba1 = 0, flagBomba2 = 0;

/* Variáveis int */
int umidadeDoSolo,sensor_analog;
const int sensor_pin = A0;  /* Soil moisture sensor O/P pin */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  digitalWrite(RELE1, HIGH);
  digitalWrite(RELE2, HIGH);
  lcd.begin(16, 2);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  LogicaELeituraDosDados();
  DadosParaLcd();
  DadosParaTagoIo();
  delay(1000);
}

void LogicaELeituraDosDados(void)
{
  umidadeDoAr = dht.readHumidity();  // Faz a leitura da umidade
  tempC = dht.readTemperature();  // Faz a leitura da temperatura
  if (isnan(tempC) || isnan(tempC))  // Verifica se teve erro na leitura
  {
    Serial.println(F("Failed to read from DHT sensor!")); // Envio de mensagem se a leitura falhou
  }
  /* Escrita da umidade e temperatura para teste na serial*/
  Serial.print(F("umidadeDoArdity: "));
  Serial.print(umidadeDoAr);
  Serial.print(F("%  Temperature: "));
  Serial.print(tempC);
  Serial.print(F("°C "));

  sensor_analog = analogRead(36); // Faz a leitura analógica do pino AD
  umidadeDoSolo = ( 100 - ( (sensor_analog/4095.00) * 100 ) );  // Faz o cálculo para a porcentagem de umidade do solo
  Serial.print("Umidade do Solo = ");
  Serial.print(umidadeDoSolo); // Printa o valor na serial para teste
  Serial.println("%");

  /* 
    Lógica da umidade do ar. Caso a umidade do ar esteja abaixo de 80%, então as bombas podem ser acionadas
    conforme a quantidade de umidade do solo. Separei em 2 condições, de acordo com a quantidade de bombas.
    Se for abaixo de 30%, liga as duas bombas. Entre 30 e 60, liga apenas uma bomba. Acima de 60 não liga nenhuma
    bomba. Se a umidade do ar estiver acima de 80%, não liga nenhuma bomba.
  */
  if(umidadeDoAr < 80)
  {
    if(umidadeDoSolo < 30)
    {
      flagBomba1 = flagBomba2 = 1;
      digitalWrite(RELE1, LOW);
      digitalWrite(RELE2, LOW);
    }
    else if(umidadeDoSolo < 60)
    {
      flagBomba1 = 1;
      flagBomba2 = 0;
      digitalWrite(RELE1, LOW);
      digitalWrite(RELE2, HIGH);
    }
    else
    {
      flagBomba1 = 0;
      flagBomba2 = 0;
      digitalWrite(RELE1, HIGH);
      digitalWrite(RELE2, HIGH);
    }
  }
  else
  {
      flagBomba1 = 0;
      flagBomba2 = 0;
      digitalWrite(RELE1, HIGH);
      digitalWrite(RELE2, HIGH);
  }
  
}

void DadosParaLcd(void)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UA:");
  lcd.print(umidadeDoAr);
  lcd.print("%");
  lcd.setCursor(10,0);
  lcd.print("US:");
  lcd.print(umidadeDoSolo);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(tempC);
  lcd.print(F("C"));
  lcd.setCursor(9,1);
  lcd.print("B1");
  lcd.print(flagBomba1);
  lcd.setCursor(13,1);
  lcd.print("B2");
  lcd.print(flagBomba2);
}
