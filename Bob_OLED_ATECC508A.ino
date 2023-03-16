#include <ArduinoBLE.h>
#include <SparkFun_ATECCX08a_Arduino_Library.h>
#include <Wire.h>
#include <SFE_MicroOLED.h> 

//Inizializzazione 
#define PIN_RESET 9
#define DC_JUMPER 1
MicroOLED oled(PIN_RESET, DC_JUMPER);
const int buttonPin = 2;
const int ledPin =  13;
ATECCX08A atecc;

uint8_t message[32]; //Inizializzazione dati sensore BM280

uint8_t signature[64]; //Inizzializzazione Firma

//Inizializzazione chiave pubblica di alice da cambiare  con quella che è stat generata dal master
uint8_t AlicesPublicKey[64] = {
0x4D, 0xA2, 0x4C, 0x8A, 0xCB, 0x30, 0x00, 0xB1, 0xEF, 0x5E, 0x46, 0x9D, 0xB6, 0xB1, 0x85, 0x71, 
0x34, 0x4D, 0x23, 0xCF, 0x8D, 0xD8, 0xF0, 0xBE, 0x37, 0x49, 0x03, 0x10, 0x4F, 0xCA, 0x38, 0x3E, 
0x2A, 0xB4, 0xFE, 0xEB, 0x19, 0x0D, 0x9C, 0xE3, 0xCA, 0xE0, 0xE7, 0xFD, 0x6E, 0xEA, 0x3C, 0x21, 
0xD3, 0x66, 0x83, 0x97, 0x24, 0x85, 0x0A, 0xFD, 0x34, 0xDA, 0xB1, 0x9D, 0xB1, 0x98, 0xEA, 0xB4
};

//Inizializzazione costanti bluetooth
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
BLEService bleService(deviceServiceUuid); 
BLEByteCharacteristic bleCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);

void setup() 
 { 
                   Wire.begin();
                   Serial.begin(115200);
                 if (atecc.begin() == true)
                     {
                        Serial.println("La connessione I2C e corretta.");
                       Serial.println();
                     }else{
                           Serial.println("Dispositivo di autenticazione non configurato");
                           Serial.println();
                          }    
                     //inizializzazione e settaggio Schermo Oled 
                        Wire.begin();
                        delay(100);
                        oled.begin();
                        oled.clear(ALL);
                        oled.display();
                        delay(1000);
                        oled.clear(PAGE);
                        oled.display();
                        oled.setFontType(0);
                        oled.setCursor(0,0);
                        pinMode(ledPin, OUTPUT);
                        pinMode(buttonPin, INPUT_PULLUP);

                BLE.begin();//inizializzazione bluetooth
              if (!BLE.begin()) 
               { 
                Serial.println("Avvio del modulo Bluetooth non riuscito!");
               }
                BLE.setLocalName("Artemis Sparkfun Read Board (Dispositivo di periferia)");
                Serial.println();
                BLE.setAdvertisedService(bleService);
                bleService.addCharacteristic(bleCharacteristic);
                BLE.addService(bleService);
                BLE.advertise();
                Serial.println("Artemis Sparkfun Read Board (Dispositivo di periferia)");
                Serial.println();
                printInfo();
                if (!(atecc.configLockStatus && atecc.dataOTPLockStatus && atecc.slot0LockStatus))
                       {
                        Serial.print("Dispositivo di autenticazione non configurato si prega di configurarlo");
                        Serial.println();
                       }
                         Serial.println("Ciao sono Bob, sto ascoltando i messaggi in arrivo da Alice tramite bluetooth");
                         Serial.println();
 }

 void loop() 
 {
     BLEDevice central = BLE.central();
     delay(3000);
     Serial.println("Ricerca del dispositivo cetrale..");    
     if (central) 
   {
         Serial.println("* Sto cercando di collegarmi ad ALICE!");
         Serial.println();
         Serial.print("* MAC ADDRESS  del dispositivo: ");
         Serial.println(central.address());
         Serial.println();

         int i =0;
         int bytes=0;
       while (central.connected()) 
       {
          while (bleCharacteristic.written())
           {
              if (i<4)
              {
                   message[i]= bleCharacteristic.value();//ricezione
                   Serial.print("DATI ");//Debug
                   Serial.print(" ");//Debug
                   Serial.println(message[i]);//Debug
               }else if ( i< 68)
                   {
                     Serial.print(i);//Debug
                     Serial.print(" ");//Debug
                     Serial.print("FIRMA");//Debug
                     Serial.print(" ");//Debug
                     signature[bytes] = bleCharacteristic.value();//ricezione
                     Serial.println(signature[bytes]);//Debug
                     bytes++;
                   }
                     if (i == 67)
                       {  
                                      central.disconnect();
                                      i=0;
                                      printMessage();
                                      printSignature();
                                if (atecc.verifySignature(message, signature, AlicesPublicKey))
                                     {
                                          Serial.println("Successo! Firma Verificata.");
                                          //visualizzazione su schermo OLED
                                          delay(1000);
                                          oled.clear(PAGE);
                                          oled.display();
                                          oled.setCursor(0,0);
                                          oled.print("Umi: ");
                                          oled.println(message[0]);
                                          oled.print("Pres: ");
                                          oled.println(message[1]);
                                          oled.print("Alt: ");
                                          oled.println(message[2]);
                                          oled.print("Temp: ");
                                          oled.println(message[3]);
                                          oled.display();
                                          delay(1000); 
                                      }
                                   else Serial.println("Verifica fallita "); 
    
                        }else{
                             i++;
                             }  
            }
       }if(!(central.connected()))
        Serial.println("disconnesso ");
   }

 }

 void printInfo()
 {
    atecc.readConfigZone(false); 
    Serial.println();
    Serial.print("Serial Number: \t");
      for (int i = 0 ; i < 9 ; i++)
    {
       if ((atecc.serialNumber[i] >> 4) == 0) Serial.print("0"); 
      Serial.print(atecc.serialNumber[i], HEX);
    }
   Serial.println();

   Serial.print("Rev Number: \t");
   for (int i = 0 ; i < 4 ; i++)
    { 
      if ((atecc.revisionNumber[i] >> 4) == 0) Serial.print("0");
      Serial.print(atecc.revisionNumber[i], HEX);
    }
     Serial.println();
     Serial.print("Config Zone: \t");
     if (atecc.configLockStatus) Serial.println("Locked");
        else Serial.println("NOT Locked");
          Serial.print("Data/OTP Zone: \t");
      if (atecc.dataOTPLockStatus) Serial.println("Locked");
        else Serial.println("NOT Locked");
         Serial.print("Data Slot 0: \t");
        if (atecc.slot0LockStatus) Serial.println("Locked");
          else Serial.println("NOT Locked"); 
           Serial.println();
    if (atecc.configLockStatus && atecc.dataOTPLockStatus && atecc.slot0LockStatus)
    {
        if (atecc.generatePublicKey(0, false) == false)
        {
         Serial.println("Failure to generate Public Key");
          Serial.println();
        }
    }
 }

 void printAlicesPublicKey()
 {
     Serial.println();
     Serial.println("uint8_t AlicesPublicKey[64] = {");
     for (int i = 0; i < sizeof(AlicesPublicKey) ; i++)
    {
       Serial.print("0x");
      if ((AlicesPublicKey[i] >> 4) == 0) Serial.print("0");
        Serial.print(AlicesPublicKey[i], HEX);
      if (i != 63) Serial.print(", ");
      if ((63 - i) % 16 == 0) Serial.println();
    }
      Serial.println("};");
      Serial.println();
  }

 void printMessage()
 {
    Serial.println("uint8_t message[32] = {");
    for (int i = 0; i < sizeof(message) ; i++)
   {
      Serial.print("0x");
      if ((message[i] >> 4) == 0) Serial.print("0");
      Serial.print(message[i], HEX);
      if (i != 31) Serial.print(", ");
      if ((31 - i) % 16 == 0) Serial.println();
   }
    Serial.println("};");
    Serial.println();
 }
 
 void printSignature()
 {
     Serial.println("uint8_t signature[64] = {");
     for (int i = 0; i < sizeof(signature) ; i++)
    {
      Serial.print("0x");
      if ((signature[i] >> 4) == 0) Serial.print("0"); 
      Serial.print(signature[i], HEX);
      if (i != 63) Serial.print(", ");
      if ((63 - i) % 16 == 0) Serial.println();
    }
      Serial.println("};");
      Serial.println();
 }