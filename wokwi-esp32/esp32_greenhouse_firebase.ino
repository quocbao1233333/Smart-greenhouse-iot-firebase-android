#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ================== WIFI WOKWI ==================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ================== FIREBASE ==================
// Không thêm /greenhouse ở cuối
const char* FIREBASE_HOST = "https://smart-city-iot-dashboard-default-rtdb.firebaseio.com";

// ================== DANH SACH KHU VUC ==================
const int AREA_COUNT = 4;

String areas[AREA_COUNT] = {
  "hanoi",
  "gialai",
  "hochiminh",
  "camau"
};

// Khu vực dùng để đọc điều khiển LED mô phỏng trong Wokwi
// Nếu bạn bấm nút Hà Nội trên dashboard thì giữ hanoi
String controlArea = "hanoi";

// ================== CHAN LED MO PHONG THIET BI ==================
#define PIN_GROW_LIGHT 2
#define PIN_FAN 4
#define PIN_WATER_PUMP 5
#define PIN_HEATER 18

// ================== THOI GIAN CAP NHAT ==================
unsigned long lastSendSensorTime = 0;
unsigned long lastReadDeviceTime = 0;

const unsigned long sendSensorInterval = 5000;  // 5 giay gui sensor
const unsigned long readDeviceInterval = 1000;  // 1 giay doc devices

// ================== HAM TAO SO NGAU NHIEN FLOAT ==================
float randomFloat(float minValue, float maxValue) {
  return minValue + ((float)random(0, 10000) / 10000.0) * (maxValue - minValue);
}

// ================== TAO JSON SENSOR DUNG KEY DASHBOARD ==================
String createSensorJsonForArea(String area) {
  float temperature;
  float humidity;
  float soilMoisture;
  float light;
  float co2;
  float waterLevel;

  // Tao du lieu moi khu vuc hoi khac nhau cho dashboard dep hon
  if (area == "hanoi") {
    temperature  = randomFloat(24.0, 32.0);
    humidity     = randomFloat(55.0, 85.0);
    soilMoisture = randomFloat(35.0, 80.0);
    light        = randomFloat(200.0, 900.0);
    co2          = randomFloat(400.0, 800.0);
    waterLevel   = randomFloat(40.0, 95.0);
  } 
  else if (area == "gialai") {
    temperature  = randomFloat(20.0, 30.0);
    humidity     = randomFloat(60.0, 90.0);
    soilMoisture = randomFloat(45.0, 85.0);
    light        = randomFloat(300.0, 1000.0);
    co2          = randomFloat(380.0, 750.0);
    waterLevel   = randomFloat(35.0, 90.0);
  } 
  else if (area == "hochiminh") {
    temperature  = randomFloat(28.0, 36.0);
    humidity     = randomFloat(60.0, 88.0);
    soilMoisture = randomFloat(30.0, 75.0);
    light        = randomFloat(400.0, 1200.0);
    co2          = randomFloat(450.0, 1000.0);
    waterLevel   = randomFloat(30.0, 85.0);
  } 
  else {
    // camau
    temperature  = randomFloat(26.0, 34.0);
    humidity     = randomFloat(70.0, 95.0);
    soilMoisture = randomFloat(50.0, 90.0);
    light        = randomFloat(250.0, 950.0);
    co2          = randomFloat(420.0, 900.0);
    waterLevel   = randomFloat(50.0, 100.0);
  }

  String json = "{";

  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"soilMoisture\":" + String(soilMoisture, 1) + ",";
  json += "\"light\":" + String(light, 1) + ",";
  json += "\"co2\":" + String(co2, 0) + ",";
  json += "\"waterLevel\":" + String(waterLevel, 1);

  json += "}";

  return json;
}

// ================== HTTP PUT LEN FIREBASE ==================
void putJsonToFirebase(String path, String jsonData) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi bi mat ket noi");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String url = String(FIREBASE_HOST) + path;

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.PUT(jsonData);

  Serial.println();
  Serial.println("========== PUT FIREBASE ==========");
  Serial.print("URL: ");
  Serial.println(url);

  Serial.print("JSON: ");
  Serial.println(jsonData);

  Serial.print("HTTP Response Code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Firebase Response: ");
    Serial.println(response);
  } else {
    Serial.println("Loi PUT Firebase");
  }

  Serial.println("==================================");

  http.end();
}

// ================== GUI SENSOR CHO 4 KHU VUC ==================
void sendSensorsForAllAreas() {
  for (int i = 0; i < AREA_COUNT; i++) {
    String area = areas[i];

    String jsonData = createSensorJsonForArea(area);

    String path = "/greenhouse/" + area + "/sensors.json";

    putJsonToFirebase(path, jsonData);
  }
}

// ================== KHOI TAO DEVICES CHO 4 KHU VUC ==================
// Dung PATCH de khong ghi de neu app dang dieu khien
void initDevicesForAllAreas() {
  String json = "{";
  json += "\"growLight\":0,";
  json += "\"fan\":0,";
  json += "\"waterPump\":0,";
  json += "\"heater\":0";
  json += "}";

  for (int i = 0; i < AREA_COUNT; i++) {
    String area = areas[i];
    String path = "/greenhouse/" + area + "/devices.json";

    putJsonToFirebase(path, json);
  }
}

// ================== DOC 1 THIET BI TU FIREBASE ==================
int readDeviceFromFirebase(String area, String deviceName) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi bi mat ket noi");
    return 0;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String url = String(FIREBASE_HOST)
             + "/greenhouse/"
             + area
             + "/devices/"
             + deviceName
             + ".json";

  http.begin(client, url);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();
    payload.trim();

    http.end();

    if (payload == "1" ||
        payload == "true" ||
        payload == "\"ON\"" ||
        payload == "\"on\"" ||
        payload == "\"1\"") {
      return 1;
    } else {
      return 0;
    }
  } else {
    Serial.print("Loi doc thiet bi: ");
    Serial.println(deviceName);
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    http.end();
    return 0;
  }
}

// ================== DOC THIET BI VA DIEU KHIEN LED ==================
void readDevicesAndControlLed() {
  int growLightState = readDeviceFromFirebase(controlArea, "growLight");
  int fanState       = readDeviceFromFirebase(controlArea, "fan");
  int pumpState      = readDeviceFromFirebase(controlArea, "waterPump");
  int heaterState    = readDeviceFromFirebase(controlArea, "heater");

  digitalWrite(PIN_GROW_LIGHT, growLightState);
  digitalWrite(PIN_FAN, fanState);
  digitalWrite(PIN_WATER_PUMP, pumpState);
  digitalWrite(PIN_HEATER, heaterState);

  Serial.println();
  Serial.println("========== TRANG THAI THIET BI ==========");
  Serial.print("Khu vuc dieu khien LED: ");
  Serial.println(controlArea);

  Serial.print("Den trong cay: ");
  Serial.println(growLightState ? "ON" : "OFF");

  Serial.print("Quat thong gio: ");
  Serial.println(fanState ? "ON" : "OFF");

  Serial.print("May bom nuoc: ");
  Serial.println(pumpState ? "ON" : "OFF");

  Serial.print("May suoi: ");
  Serial.println(heaterState ? "ON" : "OFF");

  Serial.println("=========================================");
}

// ================== XOA DU LIEU SAI CU NEU CAN ==================
// Ham nay chi goi 1 lan neu ban muon xoa cac key sai o greenhouse:
// airHumidity, co2ppm, lightLux, soilHumidity, temperature, time, waterLevel
void deleteWrongOldData() {
  String wrongKeys[7] = {
    "airHumidity",
    "co2ppm",
    "lightLux",
    "soilHumidity",
    "temperature",
    "time",
    "waterLevel"
  };

  for (int i = 0; i < 7; i++) {
    if (WiFi.status() != WL_CONNECTED) {
      return;
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    String url = String(FIREBASE_HOST)
               + "/greenhouse/"
               + wrongKeys[i]
               + ".json";

    http.begin(client, url);

    int httpResponseCode = http.sendRequest("DELETE");

    Serial.print("DELETE ");
    Serial.print(wrongKeys[i]);
    Serial.print(" -> HTTP: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  randomSeed(esp_random());

  pinMode(PIN_GROW_LIGHT, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_WATER_PUMP, OUTPUT);
  pinMode(PIN_HEATER, OUTPUT);

  digitalWrite(PIN_GROW_LIGHT, LOW);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_WATER_PUMP, LOW);
  digitalWrite(PIN_HEATER, LOW);

  Serial.println("ESP32 Wokwi - Smart Greenhouse Firebase 4 Areas");
  Serial.println("Dang ket noi WiFi Wokwi...");

  WiFi.begin(ssid, password, 6);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Da ket noi WiFi");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());

  // Neu muon xoa du lieu sai cu nhu trong anh Firebase, bo comment dong nay:
  // deleteWrongOldData();

  // Tao devices cho du 4 khu vuc
  initDevicesForAllAreas();

  // Gui sensor lan dau cho du 4 khu vuc
  sendSensorsForAllAreas();
}

// ================== LOOP ==================
void loop() {
  unsigned long currentTime = millis();

  // Gui sensor cho ca 4 khu vuc moi 5 giay
  if (currentTime - lastSendSensorTime >= sendSensorInterval) {
    lastSendSensorTime = currentTime;

    sendSensorsForAllAreas();
  }

  // Doc trang thai devices cua khu vuc controlArea moi 1 giay
  if (currentTime - lastReadDeviceTime >= readDeviceInterval) {
    lastReadDeviceTime = currentTime;

    readDevicesAndControlLed();
  }
}
