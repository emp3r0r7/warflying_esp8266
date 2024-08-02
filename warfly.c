#include <SdFat.h> //deve essere la prima (tu fallo!)
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266TrueRandom.h>

#define SD_SS D8
#define LED_PIN 2

// Inizializza il display OLED con i pin specificati per SDA e SCL
SdFat32 SD;
File32 myFile;
String fileName;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/D5, /* data=*/D6, U8X8_PIN_NONE);

void writeMsg(const String& message) {
  digitalWrite(LED_PIN, HIGH);

  // Tentativo di reinizializzare la scheda SD ogni volta
  if (!SD.begin(SD_SS, SPI_HALF_SPEED)) {
    Serial.println("Reinizializzazione della SD fallita!");
    return;  // Esce se la SD non può essere inizializzata
  } 

  if (!myFile.open(fileName.c_str(), O_RDWR | O_CREAT)) {
      Serial.println("Impossibile aprire o creare il file");
  } else {
      if (myFile.fileSize() == 0) {  // Se il file è nuovo, scrivi l'intestazione
        myFile.println("Nome,MAC Address,Segnale,Encryption,Canale,Visibility,Tempo Trascorso");
        Serial.println("Intestazione aggiunta");
      }
      myFile.close();
    }
  if (!myFile.open(fileName.c_str(), O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println(F("Apertura file fallita!"));
  } else {
    myFile.println(message);
    myFile.close();  // Chiude il file subito dopo la scrittura
    Serial.println(F("Messaggio scritto sulla SD"));
    //flashNTimesLed(2);
  }
  digitalWrite(LED_PIN, LOW);
}


void setup() {
  Serial.begin(115200);

  if (!SD.begin(SD_SS, SPI_HALF_SPEED)) {
    Serial.println("Reinizializzazione della SD fallita!");
    return;  // Esce se la SD non può essere inizializzata
  } 
  
  pinMode(LED_PIN, OUTPUT);  // Imposta il pin del LED come output
  u8g2.begin(); // Inizializza il display
  Serial.println("Delaying..."); 
  textToScreen("INITIALIZING");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  fileName = "wifi_data_" + generateUUID() + ".txt"; // Genera un nuovo UUID ad ogni avvio
  Serial.println("UUID Generated: " + fileName); 
}

void loop() {
  static unsigned long startTime = millis();
  int n = WiFi.scanNetworks();
  flashNTimesLed(4);
  Serial.println("Scansione area.. completato");
  if (n == 0) {
    Serial.println("Nessuna rete trovata");
    textToScreen("Nessuna rete trovata");
  } else {
    int loadingAnimation = 0;

    for (int i = 0; i < n; ++i) {
      if(SD.begin(SD_SS, SPI_HALF_SPEED)){
        if (isNewNetwork(WiFi.SSID(i))) {
            unsigned long currentTime = (millis() - startTime) / 1000;  // Tempo in secondi
             String data = WiFi.SSID(i) + "," 
                        + WiFi.BSSIDstr(i) + ","
                        + String(WiFi.RSSI(i)) + ","
                        + encryptionTypeDesc(WiFi.encryptionType(i)) + "," 
                        + String(WiFi.channel(i)) + "," 
                        + String(WiFi.isHidden(i) ? "Hidden" : "Visible") + "," 
                        + String(currentTime);
            writeMsg(data); //sd write
            Serial.println(data); //debug
            textToScreenNewWifi(data);
        } else {
            textToScreenScanning(loadingAnimation);
        }
      }
      delay(10);
    }
  }
  WiFi.scanDelete();
  delay(1000);  // Aspetta 5 secondi prima del prossimo scan
}

bool isNewNetwork(String ssid) {

  if (!myFile.open(fileName.c_str(), O_READ)) {
    Serial.print("Errore nell'apertura del file " + fileName + " per la lettura: ");
    Serial.println(SD.card()->errorCode(), HEX);  // Mostra il codice di errore della scheda SD
    return true;  // Presumi che la rete sia nuova se il file non può essere aperto
  }

  String content;
  while (myFile.available()) {
    content = myFile.readStringUntil('\n');
    if (content.indexOf(ssid) >= 0) {
      myFile.close();
      return false;
    }
  }

  myFile.close();
  return true;
}

void textToScreen(String message){
    u8g2.clearBuffer();          // pulisce il buffer
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 10, message.c_str());    // disegna la stringa
    u8g2.sendBuffer();
}


void textToScreenNewWifi(String message){
    u8g2.clearBuffer();          // pulisce il buffer
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 10, "New SSID Found!");    // disegna la stringa
    u8g2.drawStr(0, 30, message.c_str());    // disegna la stringa
    u8g2.sendBuffer();
}

void textToScreenScanning(int &loadingAnimation){
    u8g2.setFont(u8g2_font_6x10_tf);
    String text = "Scanning Area";

    if(loadingAnimation % 2 == 0){
        text += ".";   // Aggiunge un punto
    } else {
        text += "..";  // Aggiunge due punti
    }

    // Incrementa il contatore di animazione
    loadingAnimation = (loadingAnimation + 1) % 4;  

    // Calcola la larghezza del testo per posizionarlo correttamente o per cancellare solo quella parte
    int width = u8g2.getStrWidth(text.c_str());
    int height = u8g2.getMaxCharHeight();

    // Calcola la posizione in base alla larghezza della stringa e altezza del carattere
    int x = 0; // Imposta la posizione X iniziale del testo
    int y = 45; // Imposta la posizione Y dove viene disegnato il testo

    // Sovrascrivi solo l'area di interesse senza pulire tutto il display
    u8g2.setDrawColor(0); // Imposta il colore di disegno su nero (per cancellare)
    u8g2.drawBox(x, y - height, 128, height); // Cancella l'area dove verrà il testo (128 è la larghezza tipica di molti display OLED, adatta se necessario)
    u8g2.setDrawColor(1); // Imposta il colore di disegno su bianco (per scrivere)
    u8g2.drawStr(x, y, text.c_str()); // Disegna la nuova stringa
    u8g2.sendBuffer(); // Aggiorna il display
}



String generateUUID() {
  String uuid = "";
  for (int i = 0; i < 16; i++) {
    byte randomValue = ESP8266TrueRandom.random(0, 256); // Usa TrueRandom per generare un byte casuale
    if (i == 4 || i == 6 || i == 8 || i == 10) {
      uuid += "-";
    }
    uuid += String(randomValue < 16 ? "0" : "") + String(randomValue, HEX);
  }
  uuid.toUpperCase(); // Converti in maiuscolo se preferisci
  return uuid;
}

String encryptionTypeDesc(uint8_t encryptionType) {
  switch (encryptionType) {
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA";
    case ENC_TYPE_CCMP:
      return "WPA2";
    case ENC_TYPE_NONE:
      return "None";
    case ENC_TYPE_AUTO:
      return "Auto";
    default:
      return "Unknown";
  }
}

void flashNTimesLed(int number){
  for(int i = 0; i < number; i++ ){
      digitalWrite(LED_PIN, LOW);
      delay(20);
      digitalWrite(LED_PIN, HIGH);
      delay(20);
  }
}
