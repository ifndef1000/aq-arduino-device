// Libraries included

#include <Wire.h> // ??
#include <RTClib.h> // real time clock
#include <SPI.h> // SD card?
#include <SD.h> // SD card
#include <sps30.h> // Sensirion SPS30
#include <Adafruit_Sensor.h> // bme680
#include "Adafruit_BME680.h" // bme680
#include <Multichannel_Gas_GMXXX.h> // Grove gas sensor (CO and NO2)
static uint8_t recv_cmd[8] = {}; // Integer for Grove sensor

// Definitions

File dataFilePM; // define file to save PM data to
File dataFile; // define file to save data to
File dataFileGAS; // define file to save gas data to
RTC_DS1307 rtc; // define timeclock

#define SEALEVELPRESSURE_HPA (1013.25) // definitions for bme 680 for sea level

Adafruit_BME680 bme; // I2C

#define BME_SCK 13 // definitions for bme 680
#define BME_MISO 12 // definitions for bme 680
#define BME_MOSI 11 // definitions for bme 680
#define BME_CS 10 // definitions for bme 680

#define BME_ON // you can comment this out if you dont want the BME680 measuring
#define PM_ON // you can comment this out if you dont want the Sensiron SPS30 measuring
#define GAS_ON // you can comment this out if you dont want the Grove sensor mreasuring

// Grove Sensor defining gas
#ifdef SOFTWAREWIRE
    #include <SoftwareWire.h>
    SoftwareWire myWire(3, 2);
    GAS_GMXXX<SoftwareWire> gas;
#else
    #include <Wire.h>
    GAS_GMXXX<TwoWire> gas;
#endif

// defines variables ret, auto_clean_days and auto_clean
void setup() {

  if (!bme.begin(0x76)) 
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }
 
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  Serial.begin(9600);
  gas.begin(Wire, 0x08); // use the hardware I2C for this
  delay(1000);

  if (!SD.begin(10)) {
  Serial.println("initialization failed!");

}


}

void loop() {
 
  DateTime now = rtc.now();
  delay(1000);

#ifdef BME_ON

    if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
    }
    
    dataFile = SD.open("BME.CSV", FILE_WRITE);
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(",");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(',');
    dataFile.print(bme.temperature);
    dataFile.print(',');
    dataFile.print(bme.pressure / 100.0);
    dataFile.print(',');
    dataFile.print(bme.humidity);
    dataFile.print(',');
    dataFile.print(bme.gas_resistance / 1000.0);
    dataFile.print(',');
    dataFile.println();
    dataFile.close();

#endif

#ifdef PM_ON

    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;
    int16_t ret;

    ret = sps30_start_measurement();
    ret = sps30_read_data_ready(&data_ready);
    ret = sps30_read_measurement(&m);
    
    dataFilePM = SD.open("PM.CSV", FILE_WRITE);
    dataFilePM.print(now.year(), DEC);
    dataFilePM.print('/');
    dataFilePM.print(now.month(), DEC);
    dataFilePM.print('/');
    dataFilePM.print(now.day(), DEC);
    dataFilePM.print(",");
    dataFilePM.print(now.hour(), DEC);
    dataFilePM.print(':');
    dataFilePM.print(now.minute(), DEC);
    dataFilePM.print(':');
    dataFilePM.print(now.second(), DEC);
    dataFilePM.print(',');
    dataFilePM.print(m.mc_2p5);
    dataFilePM.print(",");
    dataFilePM.print(m.mc_10p0);
    dataFilePM.print(",");
    dataFilePM.print(m.nc_2p5);
    dataFilePM.print(",");
    dataFilePM.print(m.nc_10p0);
    dataFilePM.print(",");
    dataFilePM.print(m.typical_particle_size);
    dataFilePM.print(",");
    dataFilePM.println();
    dataFilePM.close();

#endif

#ifdef GAS_ON

    uint8_t len = 0;
    uint8_t addr = 0;
    uint8_t i;
    uint32_t val = 0;

    dataFileGAS = SD.open("GAS.CSV", FILE_WRITE);
    dataFileGAS.print(now.year(), DEC);
    dataFileGAS.print('/');
    dataFileGAS.print(now.month(), DEC);
    dataFileGAS.print('/');
    dataFileGAS.print(now.day(), DEC);
    dataFileGAS.print(",");
    dataFileGAS.print(now.hour(), DEC);
    dataFileGAS.print(':');
    dataFileGAS.print(now.minute(), DEC);
    dataFileGAS.print(':');
    dataFileGAS.print(now.second(), DEC);
    dataFileGAS.print(','); 
    val = gas.measure_NO2(); dataFileGAS.print(val); dataFileGAS.print(","); //measure NO2
    val = gas.measure_C2H5OH(); dataFileGAS.print(val); dataFileGAS.print(","); //measure ethanol
    val = gas.measure_VOC(); dataFileGAS.print(val); dataFileGAS.print(","); //measure VOC
    val = gas.measure_CO(); dataFileGAS.print(val); dataFileGAS.println(","); //measure CO
    dataFileGAS.close();

#endif


}
