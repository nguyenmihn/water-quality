#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Redmi Note 10S"
#define WIFI_PASSWORD "123456789"

#define DATABASE_URL "https://iot2-37175-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyBRcKbTuTnJ0ljvkK4OQQFpiI_jXY8XOZI"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     
int TDS_Sensor = A0;
int sensorPin = 34;
float Aref = 3.3;
float ec_Val = 0;
unsigned int tds_value = 0;
float ecCal = 1;
double wTemp;
bool signupOK = false;
float volt;
float ntu;
long ntu_standardized;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  display.display();
  delay(2000);
  display.clearDisplay();
  // Start the DS18B20 sensor
  sensors.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; 
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
  volt = 0;
  wTemp = temperatureC;
  float V_level= Aref / 1024.0;
  float rawEc = analogRead(TDS_Sensor) * V_level;  // Raw  data of EC
  float T_Cof = 1.0 + 0.02 * (wTemp - 25.0);  // Temperature Coefficient

  for(int i = 0; i < 800; i++)
  {
      volt += ((float)analogRead(sensorPin) / 1023) * 5;
  }
  volt = volt / 800 / 667 * 140;
  volt = round_to_dp(volt, 2);

  if(volt < 1){
    ntu = 6000;
  }else{
    ntu = -1120.4 * volt * volt + 5742.3 * volt - 4353.8; 
  }
  // Quy chuẩn NTU từ 0 tới 1000
  ntu_standardized = map(ntu, 0, 10000, 0, 1000);
  ntu_standardized = constrain(ntu_standardized, 0, 1000); // Đảm bảo giá trị nằm trong khoảng từ 0 tới 1000

  delay(200);
  Serial.print(volt);
  Serial.println(" V");
  // Serial.print(ntu);
  // Serial.println(" NTU");
  Serial.print(ntu_standardized);
  Serial.println(" NTU ");

  ec_Val = (rawEc / T_Cof) * ecCal;// temperature and calibration compensation
  tds_value = (133.42 * pow(ec_Val, 3) - 255.86 * ec_Val * ec_Val + 857.39 * ec_Val) * 0.5; 

  tds_value = map(tds_value, 0, 50000, 0, 1000);
  tds_value = constrain(tds_value, 0, 1000); // Đảm bảo giá trị nằm trong khoảng từ 0 tới 1000

  double Far= (((wTemp *9)/5) + 32); // Temp in Far*
  Serial.print("TDS:"); Serial.println(tds_value);
  Serial.print("EC:"); Serial.println(ec_Val, 2);
  Firebase.RTDB.setInt(&fbdo, "water/water_temp(C)", temperatureC);
  Firebase.RTDB.setInt(&fbdo, "water/turbidity(NTU)", ntu_standardized);
  Firebase.RTDB.setInt(&fbdo, "water/TDS", tds_value);
  Firebase.RTDB.setInt(&fbdo, "water/EC", ec_Val);
  display.clearDisplay();

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SH110X_WHITE); // Draw white text
  display.setCursor(0, 0); // Start at top-left corner

  display.print("water_temp:");
  display.print(temperatureC);
  display.println(" C");
  display.println("");
  display.print("turbidity:");
  display.print(ntu_standardized);
  display.println(" NTU");
  display.println("");
  display.print("TDS: ");
  display.print(tds_value);
  display.println(" ppm");
  display.println("");
  display.print("EC:");
  display.println(ec_Val, 2);
  
  // display.println(" %");
  display.display();

  delay(3000);
}

float round_to_dp(float in_value, int decimal_place)
{
  float multiplier = powf(10.0f, decimal_place);
  in_value = roundf(in_value * multiplier) / multiplier;
  return in_value;
}