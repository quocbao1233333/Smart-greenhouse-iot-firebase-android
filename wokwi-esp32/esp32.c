#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHTesp.h>

// ================== WIFI WOKWI ==================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ================== FIREBASE ==================
// Không thêm /greenhouse ở cuối
const char* FIREBASE_HOST = "https://smart-city-iot-dashboard-default-rtdb.firebaseio.com";

// ================== KHU VUC DASHBOARD ==================
// Nếu true: gửi cùng dữ liệu đo được cho cả 4 khu vực
// Nếu false: chỉ gửi cho controlArea
#define SEND_TO_ALL_AREAS true

const int AREA_COUNT = 4;

String areas[AREA_COUNT] = {
  "hanoi",
  "gialai",
  "hochiminh",
  "camau"
};

// Khu vực chính để điều khiển / test
String controlArea = "hanoi";

// ================== PIN THEO diagram.json ==================
// DHT22
#define DHT_PIN 23

// Potentiometer mô phỏng độ ẩm đất
#define SOIL_PIN 32

// Photoresistor sensor AO mô phỏng ánh sáng
#define LIGHT_PIN 33

// Gas sensor AOUT mô phỏng CO2
#define GAS_PIN 34

// HC-SR04 mô phỏng mực nước
#define TRIG_PIN 25
#define ECHO_PIN 26

// ================== DEVICE OUTPUT OPTIONAL ==================
// Trong diagram.json hiện tại chưa có LED/relay.
// Nếu bạn thêm LED/relay thì có thể dùng các chân này.
#define PIN_GROW_LIGHT 2
#define PIN_FAN 4
#define PIN_WATER_PUMP 5
#define PIN_HEATER 18

// ================== DHT OBJECT ==================
DHTesp dht;

// ================== THOI GIAN CAP NHAT ==================
unsigned long lastSendSensorTime = 0;
unsigned long lastReadDeviceTime = 0;

const unsigned long sendSensorInterval = 3000;  // 3 giây gửi sensor
const unsigned long readDeviceInterval = 1000;  // 1 giây đọc devices

// ================== BIEN SENSOR ==================
float temperature = 0;
float humidity = 0;
float soilMoisture = 0;
float light = 0;
float co2 = 0;
float waterLevel = 0;

// ================== HAM GIOI HAN GIA TRI ==================
float clampValue(float value, float minValue, float maxValue) {
  if (value < minValue) return minValue;
  if (value > maxValue) return maxValue;
  return value;
}

// ================== DOC DHT22 ==================
void readDHT22() {
  TempAndHumidity data = dht.getTempAndHumidity();

  if (!isnan(data.temperature)) {
    temperature = data.temperature;
  }

  if (!isnan(data.humidity)) {
    humidity = data.humidity;
  }
}

// ================== DOC DO AM DAT TU POTENTIOMETER ==================
void readSoilMoisture() {
  int raw = analogRead(SOIL_PIN);

  // ESP32 ADC: 0 - 4095
  // Potentiometer xoay càng lớn thì phần trăm càng lớn
  soilMoisture = (raw / 4095.0) * 100.0;

  soilMoisture = clampValue(soilMoisture, 0, 100);
}

// ================== DOC ANH SANG TU PHOTORESISTOR ==================
void readLightSensor() {
  int raw = analogRead(LIGHT_PIN);

  // Quy đổi tương đối sang lux để dashboard hiển thị
  // Wokwi photoresistor sensor cho giá trị analog 0 - 4095
  light = (raw / 4095.0) * 1200.0;

  light = clampValue(light, 0, 1200);
}

// ================== DOC CO2 TU GAS SENSOR ==================
void readGasSensor() {
  int raw = analogRead(GAS_PIN);

  // Quy đổi tương đối sang ppm
  // 0   -> khoảng 380 ppm
  // 4095 -> khoảng 2000 ppm
  co2 = 380.0 + (raw / 4095.0) * (2000.0 - 380.0);

  co2 = clampValue(co2, 380, 2000);
}

// ================== DOC KHOANG CACH HC-SR04 ==================
float readUltrasonicDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2.0;

  return distance;
}

// ================== QUY DOI KHOANG CACH THANH MUC NUOC ==================
void readWaterLevel() {
  float distanceCm = readUltrasonicDistanceCm();

  // Giả sử chiều cao bồn là 100 cm
  // Cảm biến đặt ở phía trên nhìn xuống mặt nước
  // distance nhỏ => nước cao
  // distance lớn => nước thấp
  const float tankHeightCm = 100.0;

  if (distanceCm < 0) {
    return;
  }

  waterLevel = ((tankHeightCm - distanceCm) / tankHeightCm) * 100.0;

  waterLevel = clampValue(waterLevel, 0, 100);
}

// ================== DOC TAT CA SENSOR ==================
void readAllSensors() {
  readDHT22();
  readSoilMoisture();
  readLightSensor();
  readGasSensor();
  readWaterLevel();
}

// ================== TAO JSON SENSOR DUNG DASHBOARD ==================
String createSensorJson() {
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

// ================== GUI SENSOR LEN FIREBASE ==================
void sendSensorData() {
  readAllSensors();

  String jsonData = createSensorJson();

#if SEND_TO_ALL_AREAS
  for (int i = 0; i < AREA_COUNT; i++) {
    String area = areas[i];
    String path = "/greenhouse/" + area + "/sensors.json";
    putJsonToFirebase(path, jsonData);
  }
#else
  String path = "/greenhouse/" + controlArea + "/sensors.json";
  putJsonToFirebase(path, jsonData);
#endif
}

// ================== DOC 1 DEVICE TU FIREBASE ==================
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
    Serial.print("Loi doc device: ");
    Serial.println(deviceName);
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    http.end();
    return 0;
  }
}

// ================== DOC DEVICE VA DIEU KHIEN OUTPUT ==================
void readDevicesAndControlOutput() {
  int growLightState = readDeviceFromFirebase(controlArea, "growLight");
  int fanState       = readDeviceFromFirebase(controlArea, "fan");
  int pumpState      = readDeviceFromFirebase(controlArea, "waterPump");
  int heaterState    = readDeviceFromFirebase(controlArea, "heater");

  // Nếu diagram chưa có LED/relay thì các lệnh này không hiển thị ra mạch.
  // Khi thêm LED/relay vào các chân 2,4,5,18 thì sẽ thấy bật/tắt.
  digitalWrite(PIN_GROW_LIGHT, growLightState);
  digitalWrite(PIN_FAN, fanState);
  digitalWrite(PIN_WATER_PUMP, pumpState);
  digitalWrite(PIN_HEATER, heaterState);

  Serial.println();
  Serial.println("========== TRANG THAI DEVICE ==========");
  Serial.print("Khu vuc dieu khien: ");
  Serial.println(controlArea);

  Serial.print("growLight: ");
  Serial.println(growLightState ? "ON" : "OFF");

  Serial.print("fan: ");
  Serial.println(fanState ? "ON" : "OFF");

  Serial.print("waterPump: ");
  Serial.println(pumpState ? "ON" : "OFF");

  Serial.print("heater: ");
  Serial.println(heaterState ? "ON" : "OFF");

  Serial.println("=======================================");
}

// ================== IN SENSOR RA SERIAL ==================
void printSensorData() {
  Serial.println();
  Serial.println("========== SENSOR DOC TU WOKWI ==========");

  Serial.print("Nhiet do DHT22: ");
  Serial.print(temperature, 1);
  Serial.println(" *C");

  Serial.print("Do am khong khi DHT22: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  Serial.print("Do am dat Potentiometer: ");
  Serial.print(soilMoisture, 1);
  Serial.println(" %");

  Serial.print("Anh sang LDR: ");
  Serial.print(light, 1);
  Serial.println(" lux");

  Serial.print("CO2 Gas Sensor: ");
  Serial.print(co2, 0);
  Serial.println(" ppm");

  Serial.print("Muc nuoc HC-SR04: ");
  Serial.print(waterLevel, 1);
  Serial.println(" %");

  Serial.println("========================================");
}

// ================== KHOI TAO DEVICE MAC DINH NEU CHUA CO ==================
void initDevicesIfNeeded() {
  String json = "{";
  json += "\"growLight\":0,";
  json += "\"fan\":0,";
  json += "\"waterPump\":0,";
  json += "\"heater\":0";
  json += "}";

#if SEND_TO_ALL_AREAS
  for (int i = 0; i < AREA_COUNT; i++) {
    String area = areas[i];
    String path = "/greenhouse/" + area + "/devices.json";
    putJsonToFirebase(path, json);
  }
#else
  String path = "/greenhouse/" + controlArea + "/devices.json";
  putJsonToFirebase(path, json);
#endif
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);

  // ADC ESP32
  analogReadResolution(12);

  // DHT22
  dht.setup(DHT_PIN, DHTesp::DHT22);

  pinMode(SOIL_PIN, INPUT);
  pinMode(LIGHT_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(PIN_GROW_LIGHT, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_WATER_PUMP, OUTPUT);
  pinMode(PIN_HEATER, OUTPUT);

  digitalWrite(PIN_GROW_LIGHT, LOW);
  digitalWrite(PIN_FAN, LOW);
  digitalWrite(PIN_WATER_PUMP, LOW);
  digitalWrite(PIN_HEATER, LOW);

  Serial.println("ESP32 Wokwi - Smart Greenhouse Real Sensors");
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

  // Khoi tao devices mac dinh
  // Neu khong muon ghi de device khi khoi dong, co the comment dong nay.
  initDevicesIfNeeded();

  // Gui sensor lan dau
  sendSensorData();
  printSensorData();
}

// ================== LOOP ==================
void loop() {
  unsigned long currentTime = millis();

  // Gui sensor theo chu ky
  if (currentTime - lastSendSensorTime >= sendSensorInterval) {
    lastSendSensorTime = currentTime;

    sendSensorData();
    printSensorData();
  }

  // Doc device theo chu ky
  if (currentTime - lastReadDeviceTime >= readDeviceInterval) {
    lastReadDeviceTime = currentTime;

    readDevicesAndControlOutput();
  }
}
