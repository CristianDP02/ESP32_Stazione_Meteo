Stazione Meteo IoT con ESP32, BME280 e MQ135
Questo progetto è una stazione di monitoraggio ambientale completa. Rileva dati ambientali (temperatura, umidità, pressione e qualità dell'aria) e li invia in tempo reale a una dashboard cloud sulla piattaforma ThingSpeak.

Dashboard in Tempo Reale
Questa è la dashboard pubblica del progetto, che mostra i dati aggiornati ogni 20 secondi. https://thingspeak.mathworks.com/channels/3151117

Caratteristiche Principali
Monitoraggio Ambientale: Lettura di 4 parametri ambientali chiave.
Connettività Cloud: Invio dei dati via WiFi alla piattaforma IoT ThingSpeak.
Stima della Qualità dell'Aria: Calibrazione del sensore MQ-135 per fornire una stima dei PPM (Parti Per Milione) di CO2 equivalente.
Basso Consumo: Il dispositivo utilizza un ESP32 in modalità delay (in un'evoluzione futura, deep sleep).

Tecnologie e Componenti
Hardware
Microcontrollore: ESP32 DevKitC V2
Sensore Digitale (I2C): Bosch BME280 (per temperatura, umidità, pressione)
Sensore Analogico: MQ-135 (per qualità dell'aria, CO2e)
Prototipazione: Breadboard e cavi Jumper
Software & Firmware
Linguaggio: C++ (su framework Arduino)
IDE: Arduino IDE
Protocolli: I2C, WiFi
Librerie Chiave: Adafruit_BME280, ThingSpeak
CloudPiattaforma: ThingSpeak

Come Eseguirlo
Clonare questo repository.
Installare le librerie necessarie nell'Arduino IDE (vedi include nel codice).
Creare un file credentials.h nella stessa cartella dello sketch. Questo file è ignorato da Git per proteggere i dati sensibili.
Incollare nel file credentials.h il seguente contenuto, sostituendo i valori:
#define SECRET_SSID "IL_NOME_DELLA_TUA_RETE_WIFI"
#define SECRET_PASS "LA_TUA_PASSWORD_WIFI"
#define SECRET_CH_ID 1234567 // Il tuo Channel ID ThingSpeak
#define SECRET_WRITE_API_KEY "LA_TUA_WRITE_API_KEY_THINGSPEAK"
Configurare la scheda, la porta e caricare lo sketch sull'ESP32.
Aprire il Monitor Seriale (a 115200 baud) per il debug.

Note sul Progetto
Calibrazione MQ-135
La stima dei PPM si basa sulle curve del datasheet. Per letture accurate, il sensore MQ-135 richiede un "burn-in" (rodaggio) di 12-24 ore per stabilizzarsi. I valori iniziali (come Rs e PPM) saranno molto alti e instabili. I valori di calibrazione (R0_CLEAN_AIR) sono stati derivati da dati standard e potrebbero essere affinati con una calibrazione su un campione di aria pulita (400 PPM).
