# IoT_authentication
This is the project I did for my bachelor's degree in computer science. The proposed project is a scheme 
of authentication for the IoT, which allows us to take data via a sensor 
SparkFun Environmental Combo CCS811/BME280 and sign it via Cryptographic Co-Processor ATECC508A, and send it 
via SparkFun RedBoard Artemis which has a built-in Bluetooth Low Energy. On the receiving end, there will be another SparkFun RedBoard
Artemis that will acquire the data and verify it through the 
ATECC508A Co-Processor, finally if the data has not been tampered with it will be displayed on a SparkFun Micro OLED.
