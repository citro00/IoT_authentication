#include <SparkFun_ATECCX08a_Arduino_Library.h>
#include "SparkFunBME280.h"
#include <ArduinoBLE.h>
#include <Wire.h> 
//dichiarazione costanti connessione bluetooth
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
BME280 mySensor; //Variabile sensore 
uint8_t Environment[32]; //Array dati Proveniente dal sensore
ATECCX08A atecc; //Variabile  

void setup()
{
     Wire.begin();
     Serial.begin(115000);
       if (mySensor.beginI2C() == false)
        {
           Serial.println("il Sensore BME280 non risponde si prega di controllare il cablaggio");
           Serial.println("");
        }
          Wire.begin(); 
          Serial.begin(115000);
          if(atecc.begin() == true)
          {
             Serial.println("Connessione I2C corretta.");
             Serial.println("");
          }
}

void loop() 
{ 
     Serial.println("Informazione di configurazione ATECC508A");
      printInfo();
      if (!(atecc.configLockStatus && atecc.dataOTPLockStatus && atecc.slot0LockStatus))
           {
             Serial.println("Dispositivo di autenticazione non configurato oppure camblaggio non corretto ");
           }
             Serial.println();
             Serial.println("Ciao sono Alice, vuoi che invii i  dati ambientali firmati a Bob tramite bluetooth?  Digita (y/n)");
             Serial.println();
 
          while (Serial.available() == 0); 
            if (Serial.read() == 'y')
          {
            Serial.println();
            Serial.println("Okay. Adesso li invio.");
            Serial.println();
            connectToPeripheral(); //Provo a connettermi al dispositivo centrale
          }
  else Serial.print("Hai sbagliato a cliccare, oppure hai digitato No");
} 


void connectToPeripheral()
{
      BLEDevice peripheral;
   
        if (!BLE.begin()) 
          {
            Serial.println("* Avvio del modulo Bluetooth non riuscito!");
          }else{
                   BLE.setLocalName("Spark fun Artemis(Central Device)"); 
                   BLE.advertise();
                   Serial.println("Spark fun Artemis (Dispositivo Centrale(ALICE))");
                   Serial.println();
               }
 
                 Serial.println("- Sto Cercando di collegarmi a BOB");
                 Serial.println();
                  do
                  {
                    BLE.scanForUuid(deviceServiceUuid);
                    peripheral = BLE.available();
                  } while (!peripheral);
     
    if (peripheral) 
    {
        Serial.println("* Ho rilevato BOB");
        Serial.print("*Questo è il suo MAC ADDRESS: ");
        Serial.println(peripheral.address());
         Serial.println();
        Serial.print(" Codice UUID: ");
        Serial.println(peripheral.advertisedServiceUuid());
        Serial.println();
        BLE.stopScan();
        controlPeripheral(peripheral);
    } else 
       Serial.println("Errore Dispositivo(BOB) non Trovato");
}

void controlPeripheral(BLEDevice peripheral) 
 {
       Wire.begin();
       Serial.begin(115000);
  if (peripheral.connect()) 
      {
        Serial.println(" Sono collegato BOB!");
        Serial.println();
      }else{
                Serial.println(" Connessione al dispositivo periferico(BOB) fallita!");
                Serial.println();
                return;
           }
               Serial.println("Rilevamento degli attributi del dispositivo periferico.....");
           if (peripheral.discoverAttributes())
               {
                 Serial.println(" Attributi del dispositivo periferico rilevati!");
                 Serial.println();
               }else{
                      Serial.println("* Rilevamento degli attributi del dispositivo periferico non riuscito!");
                      Serial.println();
                      peripheral.disconnect();
                      return;
                    }
                 BLECharacteristic Blesend = peripheral.characteristic(deviceServiceCharacteristicUuid);
    
                  if (!Blesend)
                          {
                           Serial.println("* il dispositivo non puo inviare dati!");
                           peripheral.disconnect();
                           return;
                          } else if (!Blesend.canWrite())
                                   {
                                    Serial.println("*Il dispositivo non ha dati da inviare!");
                                    peripheral.disconnect();
                                    return;
                                   }
                               while (peripheral.connected())
                                    {
                                      EnvironmentalData();//Chiamo  la funzione per Prendere i dati dal sensore BME280
                                      atecc.createSignature(Environment); //firmo i dati
                                      for(int i=0;i<=3;i++)
                                         {
                                            Blesend.writeValue(Environment[i]);//invio dati ambientali
                                            Serial.print(i);//debug
                                            Serial.print(" "); //debug
                                            Serial.print("DATI");//debug
                                            Serial.print(" ");//debug
                                            Serial.println(Environment[i]);//debug
                                         }
                                           for (int j = 0; j < sizeof(atecc.signature) ; j++) 
                                                {
                                                   Blesend.writeValue(atecc.signature[j]);//invio firma
                                                   Serial.print(j);//debug
                                                   Serial.print(" ");  //debug
                                                   Serial.print("FIRMA");//debug
                                                   Serial.print(" ");//debug
                                                   Serial.println(atecc.signature[j]);//debug
                                                    if (j==64)
                                                       {
                                                         peripheral.disconnect();//(disconnettiamo appena invia  dati e firma in modo di non mandare
                                                                                 //dati che comprometterebbero la firma)
                                                         Serial.println("Avvio e Verifica avvenuti Correttamente,Disconnessione in Corso");
                                                         Serial.println();
                                                       }
                                                }   
                                          break;
 
                                    }Serial.println("Disconnessione");
                  
 }
//FUNZIONI UTILIZZATE
void EnvironmentalData() 
{
   Environment[0]=(uint8_t)mySensor.readFloatHumidity();
   Environment[1]=(uint8_t)mySensor.readFloatPressure();
   Environment[2]=(uint8_t)mySensor.readFloatAltitudeFeet();
   Environment[3]=(uint8_t)mySensor.readTempC();
}

void printAlicesPublicKey()
{ 
  Serial.println("**Copia/incolla la seguente chiave pubblica (di alice) nella parte iniziale di (bob).**");
  Serial.println("Bob ne ha bisogno per verificare la sua firma");
  Serial.println();
  Serial.println("uint8_t AlicesPublicKey[64] = {");
  for (int i = 0; i < sizeof(atecc.publicKey64Bytes) ; i++)
  {
    Serial.print("0x");
    if ((atecc.publicKey64Bytes[i] >> 4) == 0) Serial.print("0"); 
    Serial.print(atecc.publicKey64Bytes[i], HEX);
    if (i != 63) Serial.print(", ");
    if ((63 - i) % 16 == 0) Serial.println();
  }
  Serial.println("};");
  Serial.println();
}
void printInfo()
{

  atecc.readConfigZone(false); 

  Serial.println();

  Serial.print("Serial Number: \t");
  for (int i = 0 ; i < 9 ; i++)
  {
    if ((atecc.serialNumber[i] >> 4) == 0) Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(atecc.serialNumber[i], HEX);
  }
  Serial.println();

  Serial.print("Rev Number: \t");
  for (int i = 0 ; i < 4 ; i++)
  {
    if ((atecc.revisionNumber[i] >> 4) == 0) Serial.print("0"); // print preceeding high nibble if it's zero
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
      Serial.println("Errore la chiave pubblica di alice non è stata generata correttamente");
      Serial.println();
    }
    printAlicesPublicKey();
  }
}