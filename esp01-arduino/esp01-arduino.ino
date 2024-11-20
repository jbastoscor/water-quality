#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Credenciais WiFi
const char *ssid = "";     // Substitua pelo nome da sua rede WiFi
const char *password = ""; // Substitua pela senha da sua rede WiFi

// Configuração do Broker MQTT
const char *mqtt_server = "393ef3f9806b4bafaa612c1071f4b8f5.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_topic = "mqtt/agua";

// Credenciais do Broker MQTT
const char *mqtt_user = ""; // Substitua pelo seu username
const char *mqtt_pass = ""; // Substitua pela sua senha

WiFiClientSecure espClient; // Cliente seguro para conexão TLS
PubSubClient client(espClient);

// Conecta ao WIFI
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Conecta ao broker MQTT
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Tentando conectar ao Broker MQTT...");
    // Conectando com autenticação (username e password)
    if (client.connect("Esp01", mqtt_user, mqtt_pass))
    {
      Serial.println("Conectado");
    }
    else
    {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

// Inicialização
void setup()
{
  Serial.begin(115200); // Comunicação com o monitor serial
  // Serial.swap();              // Troca para os pinos GPIO1 (TX) e GPIO3 (RX) para comunicação serial com o Arduino
  setup_wifi();

  // Configuração do cliente TLS
  espClient.setInsecure(); // Ignorar verificação de certificado (para testes apenas!)
  client.setServer(mqtt_server, mqtt_port);
}

// Recupera os dados do arduino e pulica no broker
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Verifica se há dados recebidos pela serial do Arduino
  if (Serial.available() > 0)
  {
    String mensagem = Serial.readStringUntil('\n'); // Lê a mensagem até nova linha
    Serial.println("Mensagem recebida do Arduino: " + mensagem);

    // Envia a mensagem recebida para o Broker MQTT
    if (client.publish(mqtt_topic, mensagem.c_str()))
    {
      Serial.println("Mensagem enviada para o MQTT com sucesso!");
    }
    else
    {
      Serial.println("Falha ao enviar a mensagem para o MQTT.");
    }
  }
}
