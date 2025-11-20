# WorkStation 4.0 - IoT para o Futuro do Trabalho

## 1. Descrição do Projeto
Este projeto consiste numa estação de trabalho inteligente baseada em **ESP32**, desenvolvida para alinhar produtividade e bem-estar no contexto da Indústria 4.0.

O dispositivo atua como um terminal de ponta (Edge Device) que permite ao operador registar a sua produção (Gamificação) e, simultaneamente, monitoriza as condições ambientais (Temperatura/Humidade) para prevenir insalubridade e fadiga.

## 2. Hardware Utilizado (Simulação Wokwi)
* **ESP32 (DevKit v1):** Microcontrolador com Wi-Fi integrado.
* **DHT22:** Sensor de Temperatura e Humidade.
* **LCD 16x2 (I2C):** Interface visual para feedback imediato ao trabalhador.
* **Push Button:** Gatilho para registo de produção.
* **LEDs (Verde/Vermelho):** Indicadores visuais de status e alerta.

* <img width="761" height="458" alt="image" src="https://github.com/user-attachments/assets/f0188966-cfd1-4413-b0f3-b92b62eb02cc" />


## 3. Comunicação e Protocolos
O sistema utiliza o protocolo **MQTT** para transmissão de dados em tempo real, garantindo baixo consumo de banda e alta confiabilidade.

### Tópicos MQTT Configurados:
1.  **Produção (`fabrica/setorA/prod`):**
    * Envia JSON a cada acionamento do botão.
    * Ex: `{"matricula": "123456", "quantidade": 1, "total": 5}`
2.  **Ambiente (`fabrica/setorA/sensor`):**
    * Envia telemetria a cada 2 segundos.
    * Ex: `{"temp": 32.5, "umid": 60.0}`

## 4. Como Executar
1.  Aceda ao projeto no Wokwi através deste link: https://wokwi.com/projects/448148989030622209
2.  Inicie a simulação.
3.  Utilize um cliente MQTT (ex: HiveMQ Client) e subscreva o tópico `fabrica/setorA/#` para visualizar os dados.

## 5. Funcionalidades de Bem-Estar
* **Alerta de Calor:** Se T > 30°C, aciona LED Vermelho e exibe alerta no LCD.
* **Alerta de Frio:** Se T < 5°C, aciona LED Vermelho e exibe alerta no LCD.
* **Feedback de Produção:** Confirmação visual (LED Verde) a cada registo.

## 6. Vídeo explicativo
**Link: ** https://www.awesomescreenshot.com/video/46631982?key=84bba1649ae32a03cb3f81947fe97a7d
