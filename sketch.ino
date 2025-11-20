#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Configurações de Hardware ---
#define PIN_DHT 15
#define PIN_BTN_PROD 12
#define PIN_LED_OK 4
#define PIN_LED_ALERT 2
#define DHTTYPE DHT22

// --- Configurações de Rede (Wokwi) ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com"; 

// --- Tópicos MQTT ---
const char* topico_producao = "fabrica/setorA/prod";
const char* topico_ambiente = "fabrica/setorA/sensor";

// --- Objetos ---
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(PIN_DHT, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Variáveis Globais ---
int contadorProducao = 0;
unsigned long lastMsg = 0;
bool btnPressionado = false; // Controle para evitar cliques duplos

void setup_wifi() {
  delay(10);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("A ligar WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  lcd.setCursor(0, 0);
  lcd.print("WiFi OK!       ");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-Fabrica-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      lcd.setCursor(0, 1);
      lcd.print("MQTT OK!       ");
      delay(1000);
      lcd.clear();
      atualizarDisplay();
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configura o botão com resistor interno (PULLUP)
  // Isso significa que SOLTO ele vale 1 (HIGH), APERTADO ele vale 0 (LOW)
  pinMode(PIN_BTN_PROD, INPUT_PULLUP);
  
  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_ALERT, OUTPUT);
  
  dht.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void atualizarDisplay() {
  // Limpa apenas a linha 0 para evitar piscar a tela toda
  lcd.setCursor(0, 0);
  lcd.print("Prod: ");
  lcd.print(contadorProducao);
  lcd.print(" Latas   "); // Espaços extras para limpar caracteres antigos
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // --- 1. Lógica de Produção (Botão) ---
  // LOW significa que o botão foi apertado (conectado ao GND)
  if (digitalRead(PIN_BTN_PROD) == LOW) {
    
    // Verifica se o botão já não estava pressionado antes (Debounce)
    if (!btnPressionado) { 
      btnPressionado = true; // Trava o botão
      contadorProducao++;
      
      // Feedback Visual
      digitalWrite(PIN_LED_OK, HIGH);
      
      // Monta o JSON
      char msg[100];
      snprintf(msg, 75, "{\"matricula\": \"123456\", \"quantidade\": 1, \"total\": %d}", contadorProducao);
      
      // Envia MQTT
      client.publish(topico_producao, msg);
      Serial.println(msg);
      
      atualizarDisplay();
      delay(200); // O LED fica aceso por 200ms
      digitalWrite(PIN_LED_OK, LOW);
    }
  } else {
    // Quando soltar o botão, destrava a variável
    btnPressionado = false;
  }

  // --- 2. Monitorização Ambiental (A cada 2 segundos para teste rápido) ---
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      return;
    }

    // --- Lógica de Alerta (Calor OU Frio) ---
    lcd.setCursor(0, 1); // Escreve na linha de baixo
    
    if (t > 30.0) {
      // Muito Quente
      digitalWrite(PIN_LED_ALERT, HIGH);
      lcd.print("ALERTA: CALOR!  "); // Espaços para limpar
    } 
    else if (t < 5.0) {
      // Muito Frio (NOVO)
      digitalWrite(PIN_LED_ALERT, HIGH);
      lcd.print("ALERTA: FRIO!   "); // Espaços para limpar
    } 
    else {
      // Temperatura Normal
      digitalWrite(PIN_LED_ALERT, LOW);
      lcd.print("Temp: ");
      lcd.print(t, 1);
      lcd.print("C      "); // Espaços para limpar msg de alerta anterior
    }

    // Envia MQTT
    char msg[100];
    char tempStr[8];
    dtostrf(t, 1, 2, tempStr);
    snprintf(msg, 75, "{\"temp\": %s, \"umid\": %.1f}", tempStr, h);
    client.publish(topico_ambiente, msg);
  }
}