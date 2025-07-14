# 💧 IoT-Based Water Quality Monitoring with Firebase & OLED Display

This project is a real-time **water quality monitoring system** built on ESP32 using various sensors and Firebase. It reads water **temperature**, **turbidity (NTU)**, **TDS**, and **electrical conductivity (EC)**, then uploads the data to **Firebase Realtime Database**, and displays it on an **OLED screen**.

---

## 🔧 Features

- 📡 Uploads sensor data to **Firebase Realtime Database**
- 🌡️ Measures water **temperature** using DS18B20
- 💧 Measures **TDS** and **electrical conductivity**
- 🌫️ Measures **turbidity (NTU)** with calibration and standardization
- 🖥️ Displays real-time data on an **OLED 128x64 SH1106** screen
- 📶 Wi-Fi connected using ESP32

---

## 🧰 Hardware Components

| Component              | Description                          |
|------------------------|--------------------------------------|
| ESP32 Board            | Main controller with Wi-Fi           |
| DS18B20                | Digital temperature sensor           |
| TDS Sensor             | For total dissolved solids           |
| Turbidity Sensor       | Analog sensor for water cloudiness   |
| OLED SH1106 128x64     | I2C Display for live output          |
| Resistors/Wires        | For interfacing and calibration      |

---

## 🌐 Firebase Configuration

You must replace these macros with your own:

```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourPassword"
#define DATABASE_URL "https://your-project-id.firebaseio.com/"
#define API_KEY "YourFirebaseWebAPIKey"
📊 Firebase Data Paths
The data is stored in Firebase under the path /water/:

Firebase Key	Description
water_temp(C)	Water temperature in °C
turbidity(NTU)	Turbidity in NTU
TDS	Total Dissolved Solids
EC	Electrical Conductivity

📟 Display Output
The OLED screen displays:

makefile
Sao chép
Chỉnh sửa
water_temp: 26.3 C

turbidity: 425 NTU

TDS: 750 ppm

EC: 1.12
🔁 Main Loop Logic
Read temperature using DS18B20.

Read analog values from turbidity and TDS sensors.

Calculate volt, EC, TDS, and standardize NTU.

Upload to Firebase via Firebase.RTDB.setInt(...).

Display all values on OLED.

Delay 3 seconds and repeat.

🧪 Calibration Notes
The turbidity value is standardized from 0–1000 NTU.

EC is temperature-compensated using a coefficient.

TDS formula based on EC:

ini
Sao chép
Chỉnh sửa
tds = (133.42 * EC³ - 255.86 * EC² + 857.39 * EC) * 0.5
📚 Libraries Required
Firebase_ESP_Client

Adafruit_GFX

Adafruit_SH110X

OneWire, DallasTemperature

SPI, Wire

Install via Library Manager in Arduino IDE.

🧠 Function Highlight
cpp
Sao chép
Chỉnh sửa
float round_to_dp(float in_value, int decimal_place)
A helper to round any float to a desired number of decimal places.

📷 Preview
(Add your hardware photo or screenshot here)

📄 License
This project is licensed under the MIT License – feel free to use and modify for educational or research purposes.

✉️ Contact
Author: [Your Name]
📧 [Nminh59001@gmail.com]
🌐 [https://github.com/nguyenmihn]
