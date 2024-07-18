# Progetto di Laurea in Informatica

Questo è il **progetto** che ho realizzato per la mia laurea in informatica. Il progetto proposto è uno schema di autenticazione per l'IoT, che ci consente di prendere dati tramite un sensore **SparkFun Environmental Combo CCS811/BME280** e firmarli tramite il **co-processore crittografico ATECC508A**. I dati vengono inviati tramite **SparkFun RedBoard Artemis**, che ha un built-in Bluetooth a basso consumo energetico.

Sul lato ricevente ci sarà un altro **SparkFun RedBoard Artemis** che acquisirà i dati e li verificherà tramite il **co-processore ATECC508A**. Infine, se i dati non sono stati manomessi, verranno visualizzati su uno **SparkFun Micro OLED**.

## Dettagli Tecnici

Viene utilizzata un'autenticazione in cui i due processori si scambiano una **chiave privata** per firmare i file e garantire il **non ripudio** dei dati scambiati. Nella repository sono presenti due file, **alice** e **bob**, per lo scambio dei dati in modo sicuro.

Inoltre, nella repository sono presenti:
- La **tesi della mia laurea**
- La **presentazione** della discussione

## Componenti Utilizzati
- **Sensore:** SparkFun Environmental Combo CCS811/BME280
- **Co-processore crittografico:** ATECC508A
- **Scheda di sviluppo trasmittente e ricevente:** SparkFun RedBoard Artemis
- **Display:** SparkFun Micro OLED
