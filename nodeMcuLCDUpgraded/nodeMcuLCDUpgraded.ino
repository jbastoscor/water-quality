#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuração do LCD (endereço padrão 0x27, ajustar se necessário)
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Credenciais WiFi
const char* ssid = "";               // Substitua pelo nome da sua rede WiFi
const char* password = "";  // Substitua pela senha da sua rede WiFi

// Configuração do Broker MQTT
const char* mqtt_server = "393ef3f9806b4bafaa612c1071f4b8f5.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;             // Porta para conexão segura (TLS)
const char* mqtt_topic = "mqtt/agua";  // Substitua pelo tópico desejado

// Credenciais do Broker MQTT
const char* mqtt_user = "";  // Substitua pelo seu username
const char* mqtt_pass = "";      // Substitua pela sua senha

WiFiClientSecure espClient;  // Cliente seguro para conexão TLS
PubSubClient client(espClient);

// Variáveis para armazenar o título e a mensagem separada
String titulo = "";
String mensagem = "";

//  Tratar mensagens recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  // Limpar o LCD antes de mostrar a nova mensagem
  lcd.clear();

  // Converter payload em String
  String msgRecebida = "";
  for (int i = 0; i < length; i++) {
    msgRecebida += (char)payload[i];
  }

  // Separar título e mensagem com base no caractere ":"
  int posSeparador = msgRecebida.indexOf(':');
  if (posSeparador != -1) {
    titulo = msgRecebida.substring(0, posSeparador) + ":";
    mensagem = msgRecebida.substring(posSeparador + 1);
  }
}

// Função para conectar ao WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função para conectar ao broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao Broker MQTT...");
    if (client.connect("nodeMcu", mqtt_user, mqtt_pass)) {
      Serial.println("Conectado");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

// Inicialização
void setup() {
  Serial.begin(9600);
  setup_wifi();

  // Inicializar o LCD
  lcd.init();
  lcd.backlight();

  // Configuração do cliente MQTT
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// Env ia as mensagens recebidas para tela LCD
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Exibir título na primeira linha
  lcd.setCursor(0, 0);
  lcd.print(titulo);

  // Variáveis para controlar a rolagem e o atraso inicial
  static unsigned long lastScroll = 0;
  static int scrollPos = 0;
  static bool delayDone = false;  // Indica se o atraso inicial foi concluído

  // Tempo de atraso inicial antes de começar a rolagem (em milissegundos)
  unsigned long delayStart = 2000;

  // Realizar o atraso inicial
  if (!delayDone) {
    // Exibir a primeira parte da mensagem na segunda linha
    lcd.setCursor(0, 1);
    lcd.print(mensagem.substring(0, 16));

    // Verificar se o tempo de atraso inicial já passou
    if (millis() > delayStart) {
      delayDone = true;       // Indicar que o atraso inicial foi concluído
      lastScroll = millis();  // Reiniciar o temporizador de rolagem
    }
  } else {
    // Rolagem da mensagem após o atraso inicial
    if (millis() - lastScroll > 500) {  // Intervalo para rolar o texto
      lastScroll = millis();

      // Limpar a segunda linha
      lcd.setCursor(0, 1);

      // Verificar se a posição da rolagem está perto do fim da mensagem
      if (scrollPos + 16 > mensagem.length()) {
        scrollPos = 0;  // Reiniciar a rolagem
      }

      // Exibir uma "janela" de 16 caracteres da mensagem
      lcd.print(mensagem.substring(scrollPos, scrollPos + 16));

      // Avançar uma posição na rolagem
      scrollPos++;
    }
  }
}
