#include <OneWire.h>
#include <DallasTemperature.h>

// TDS - Conductividade - Total dissolved solids(TDS), é uma medida da quantidade de substâncias inorgânicas e orgânicas na água e é normalmente medido em partes por milhão (ppm).
#define TdsSensorPin A5
#define VREF 5.0           // analog reference voltage(Volt) of the ADC
#define SCOUNT 30          // sum of sample point
int analogBuffer[SCOUNT];  // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

// NTU - A turbidez pode ser entendida como a medida do espalhamento
//  de luz produzido pela presença de partículas em suspensão ou coloidais,
//  sendo expressa como Unidade Nefelométrica de Turbidez
//  (NTU – Nephelometric Turbidity Unity).
#define NtuSensorPin A4
double NTU = 0.0;

// Temperatura
#define ONE_WIRE_BUS 10
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer = { 0x28, 0x32, 0x8E, 0x49, 0xF6, 0x60, 0x3C, 0x9C };

// PH
// Pino analógico onde o sensor de pH está conectado
#define PhSensorPin A3
// Variáveis para armazenar os valores de tensão e pH
float voltage;
float pH;
// Valores conhecidos de referência (tensão e pH)
const float voltageNeutral = 2.70;   // pH 7
const float voltageAcidic = 3.37;    // pH 3
const float voltageAlkaline = 1.85;  // pH 12

// Nível da água
#define LevelSensorPin A2       // Pino onde o sensor está conectado
int sensorValue = 0;      // Variável para armazenar o valor lido do sensor
float sensorVoltage = 0;  // Para armazenar a tensão calculada
float maxVoltage = 3.5;   // Tensão máxima de saída do sensor
float minVoltage = 0;     // Tensão mínima de saída do sensor

// Inicialização 
void setup() {
  Serial.begin(115200);

  // TDS
  pinMode(TdsSensorPin, INPUT);

  // Temperatura
  sensors.begin();                               //inicializa sensores
  sensors.setResolution(insideThermometer, 10);  //configura para resolução de 10 bits
}

// Controla o envio dos dados para o ESP-01 de cada sensor. 
void loop() {
  getTDS();
  delay(3500);
  getNTU();
  delay(3500);
  getTemperature();
  delay(3500);
  getPH();
  delay(3500);
  getLevel();
  delay(3500);
}

// TDS - Conductividade
void getTDS() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
  averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;                                                                                                   // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                                //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  float compensationVolatge = averageVoltage / compensationCoefficient;                                                                                                             //temperature compensation
  tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;  //convert voltage value to tds value
  // Serial.print("voltage:");
  // Serial.print(averageVoltage, 2);
  // Serial.print("V ");
  Serial.print("TDS-Condutivi.: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");
}

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

// NTU - Turbidez
void getNTU() {
  int sensorValue = analogRead(NtuSensorPin);
  float voltage = sensorValue / 1024.0 * 5.0;

  NTU = calc_NTU(voltage);

  // Serial.print(voltage);
  // Serial.print(" | ");
  Serial.print("NTU-Turbidez: ");
  Serial.println(NTU, 0);
}

double calc_NTU(double volt) {
  double NTU_val;
  NTU_val = -(1120.4 * volt * volt) + (5742.3 * volt) - 4352.9;
  return NTU_val;
}

// Temperatura
void getTemperature() {
  sensors.requestTemperatures();
  printTemperature(insideThermometer);
}

void printTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Erro de leitura");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(tempC);
    Serial.print(" graus C");
    // Serial.print(" | ");
    // Serial.print(DallasTemperature::toFahrenheit(tempC));
    // Serial.print(" graus F");
  }

  Serial.print("\n\r");
}

// PH
void getPH() {
  int sensorValue = analogRead(PhSensorPin);

  // Converte o valor para tensão (considerando alimentação de 5V)
  voltage = sensorValue * (5.0 / 1023.0);

  // Calcula o pH com base na tensão
  pH = mapVoltageToPH(voltage);

  // Exibe os valores no monitor serial
  // Serial.print("Tensao (V): ");
  // Serial.print(voltage);
  // Serial.print(" | pH: ");
  Serial.print("PH: ");
  Serial.println(pH);

  // Aguarda um tempo antes de ler novamente
}

float mapVoltageToPH(float voltage) {
  // Fórmula para relação linear entre tensão e pH
  if (voltage > voltageNeutral) {
    // Para pH ácido
    return 7 - ((voltage - voltageNeutral) * (7 - 3) / (voltageAcidic - voltageNeutral));
  } else {
    // Para pH alcalino
    return 7 + ((voltageNeutral - voltage) * (12 - 7) / (voltageNeutral - voltageAlkaline));
  }
}

//Nível da água
void getLevel() {
  sensorValue = analogRead(LevelSensorPin);

  // Converter o valor lido para uma tensão
  sensorVoltage = (sensorValue * 5.0) / 1023;  // Mapeia a leitura (0-1023) para uma tensão de 0 a 5V

  // Exibir a tensão lida
  // Serial.print("Tensão do sensor: ");
  // Serial.print(sensorVoltage);
  // Serial.println(" V");

  // Exibir o nível de água baseado na tensão lida
  if (sensorVoltage == 0) {
    Serial.println("Nivel: Sem agua detectada.");
  } else if (sensorVoltage > 0 && sensorVoltage <= 2) {
    Serial.println("Nivel: Nivel baixo.");       
  } else if (sensorVoltage > 2 && sensorVoltage <= maxVoltage) {
    Serial.println("Nivel: Nivel normal.");
  } else if (sensorVoltage > maxVoltage) {
    Serial.println("Nivel: Nivel maximo.");
  }
}
