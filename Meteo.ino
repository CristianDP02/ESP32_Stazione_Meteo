/*
 * PROGETTO: Stazione Meteo v1
 * OBIETTIVO: Leggere BME280 (I2C) e MQ135 (Analogico).
 * HARDWARE: ESP32, BME280, MQ135
 * ALIMENTAZIONE (3.3V per entrambi i sensori)
 */

//LIBRERIE
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <cmath>
#include <WiFi.h>            
#include "ThingSpeak.h"
#include "credentials.h"

//COSTANTI
#define BME280_ADDRESS (0x76) // Indirizzo I2C BME280
#define MQ135_PIN 34          // Pin ADC per l'MQ135 (GPIO 34)

//COSTANTI CALIBRAZIONE MQ-135
#define VCC 3.3                 // Tensione di alimentazione (3.3V per ESP32)
#define RL_VALUE 5.0            // Resistenza di carico (RL) sul modulo (5 kOhm)
#define R0_CLEAN_AIR 76.625     // R0 (Rs in aria pulita/400ppm) stimato
#define PARA 116.6020682        // Parametro 'a' per la curva CO2
#define PARB -2.769034857       // Parametro 'b' per la curva CO2

//INIZIALIZZAZIONE OGGETTI
Adafruit_BME280 bme; // Oggetto per BME280
WiFiClient client;  

void setup() {
  Serial.begin(115200); 
  Serial.println("\nAvvio Stazione Meteo...");

  bool status = bme.begin(BME280_ADDRESS);  
  if (!status) {
    Serial.println("ERRORE: Sensore BME280 non trovato!");
    while (1); 
  }
  Serial.println("Sensore BME280 trovato.");

  /*
   * L'ESP32 ha un ADC a 12 bit (0-4095).
   * Possiamo impostare un'attenuazione per definire il range
   * di tensione che può leggere. 11dB ci dà il range
   * completo di 0-3.3V.
   */
   analogSetPinAttenuation(MQ135_PIN, ADC_11db);

  Serial.print("Connessione a WiFi: ");
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnesso al WiFi!");

  // Inizializza ThingSpeak
  ThingSpeak.begin(client); 
  Serial.println("Comunicazione con ThingSpeak avviata.");
}

void loop() {
  // --- Lettura BME280 ---
  float temperatura = bme.readTemperature();
  float umidita = bme.readHumidity();
  float pressione = bme.readPressure() / 100.0F;

  // --- Lettura MQ135 ---
  int mq_raw = analogRead(MQ135_PIN); //Valore grezzo tra 0 e 4095
  float v_rl = (float)mq_raw * (VCC / 4095.0); //Converte il valore grezzo in tensione (V_RL)
  float rs = RL_VALUE * (VCC / v_rl - 1.0); //Calcola la resistenza Rs (in kOhm) usando la formula del partitore
  float ratio = rs / R0_CLEAN_AIR; //Calcola il rapporto Rs/R0
  float ppm = PARA * pow(ratio, PARB); //Calcola i PPM usando la formula della curva

  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, umidita);
  ThingSpeak.setField(3, pressione);
  ThingSpeak.setField(4, ppm);

  // 2. Scrive i campi sul cloud
  int httpCode = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_API_KEY);

  if(httpCode == 200) {
    Serial.println("Dati inviati a ThingSpeak con successo.");
  } else {
    Serial.print("Errore invio dati a ThingSpeak. Codice HTTP: ");
    Serial.println(httpCode);
    Serial.println("Controlla API Key e Channel ID.");
  }

  // --- Stampa su Seriale ---
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.print(" °C | ");
  Serial.print("Umidità: "); Serial.print(umidita); Serial.print(" % | ");
  Serial.print("Pressione: "); Serial.print(pressione); Serial.print(" hPa | ");
  
  Serial.print("Resistenza Sensore (Rs): "); Serial.print(rs); Serial.print(" kOhm | ");
  Serial.print("Qualità Aria (CO2e): "); Serial.print(ppm); Serial.println(" PPM");

  Serial.println(); 
  
  delay(20000); 
}
