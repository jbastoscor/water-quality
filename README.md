# Projeto de Monitoramento de Água

Este projeto utiliza Arduino Uno, ESP-01 e NodeMCU para monitorar a qualidade da água, medindo parâmetros como TDS (Total de Sólidos Dissolvidos), NTU (Turbidez), pH, temperatura e nível da água.

## Componentes Utilizados

- Arduino Uno
- ESP-01
- NodeMCU
- Sensores de TDS, NTU, pH, temperatura e nível da água
- Display LCD I2C

## Estrutura do Projeto

- `arduino-esp/arduino-esp.ino`: Código para o Arduino Uno que lê os sensores e envia os dados para o ESP-01.
- `esp01-arduino/esp01-arduino.ino`: Código para o ESP-01 que conecta ao WiFi e envia os dados recebidos do Arduino para o broker MQTT.
- `nodeMcuLCDUpgraded/nodeMcuLCDUpgraded.ino`: Código para o NodeMCU que recebe os dados do broker MQTT e exibe no display LCD.

## Funcionalidades

### Arduino Uno

- **TDS (Total de Sólidos Dissolvidos)**: Medido pelo sensor de TDS e calculado no método `getTDS()`.
- **NTU (Turbidez)**: Medido pelo sensor de NTU e calculado no método `getNTU()`.
- **pH**: Medido pelo sensor de pH e calculado no método `getPH()`.
- **Temperatura**: Medida pelo sensor de temperatura e exibida no método `getTemperature()`.
- **Nível da Água**: Medido pelo sensor de nível e exibido no método `getLevel()`.

### ESP-01

- Conecta ao WiFi e ao broker MQTT.
- Recebe dados do Arduino via comunicação serial e publica no broker MQTT.

### NodeMCU

- Conecta ao WiFi e ao broker MQTT.
- Recebe dados do broker MQTT e exibe no display LCD.

## Configuração

### Arduino Uno

1. Conecte os sensores aos pinos apropriados do Arduino Uno.
2. Carregue o código `arduino-esp/arduino-esp.ino` no Arduino Uno.

### ESP-01

1. Configure as credenciais WiFi e MQTT no código `esp01-arduino/esp01-arduino.ino`.
2. Carregue o código no ESP-01.

### NodeMCU

1. Configure as credenciais WiFi e MQTT no código `nodeMcuLCDUpgraded/nodeMcuLCDUpgraded.ino`.
2. Carregue o código no NodeMCU.

## Execução

1. Ligue o Arduino Uno, ESP-01 e NodeMCU.
2. O Arduino Uno começará a ler os sensores e enviar os dados para o ESP-01.
3. O ESP-01 publicará os dados no broker MQTT.
4. O NodeMCU receberá os dados do broker MQTT e exibirá no display LCD.

## Contato

- Jacqueline Bastos Correa
- Universidade Presbiteriana Mackenzie
- Análise e Desenvolvimento de Sistemas
- Quinto Semestre
