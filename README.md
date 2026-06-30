# Smart Greenhouse IoT Dashboard

## 1. Giới thiệu dự án

**Smart Greenhouse IoT Dashboard** là hệ thống giám sát và điều khiển nhà kính thông minh sử dụng:

- **ESP32 trên Wokwi** để mô phỏng thiết bị IoT.
- **Firebase Realtime Database** để lưu dữ liệu cảm biến và trạng thái thiết bị theo thời gian thực.
- **HTML, CSS, JavaScript** để xây dựng dashboard web.
- **MIT App Inventor WebViewer** để đóng gói dashboard thành ứng dụng Android APK.
- **Firebase REST API** để ESP32 gửi dữ liệu cảm biến và đọc lệnh điều khiển thiết bị.

Dự án cho phép người dùng giám sát các thông số môi trường như nhiệt độ, độ ẩm không khí, độ ẩm đất, ánh sáng, CO₂ và mực nước. Ngoài ra, người dùng có thể điều khiển các thiết bị như đèn trồng cây, quạt thông gió, máy bơm nước và máy sưởi trực tiếp từ web dashboard hoặc app Android.

---

## 2. Tên repository GitHub đề xuất

Tên khuyến nghị:

```text
smart-greenhouse-iot-firebase-android
```

Một số tên thay thế:

```text
esp32-smart-greenhouse-firebase
smart-greenhouse-dashboard-esp32
iot-greenhouse-control-system
firebase-esp32-greenhouse-dashboard
smart-greenhouse-webview-android-app
```

Nên dùng tên:

```text
smart-greenhouse-iot-firebase-android
```

Tên này thể hiện đầy đủ các phần chính của dự án:

- `smart-greenhouse`: nhà kính thông minh
- `iot`: hệ thống Internet of Things
- `firebase`: cơ sở dữ liệu thời gian thực
- `android`: có app điện thoại Android

---

## 3. Chức năng chính

### 3.1. Giám sát cảm biến

Dashboard hiển thị 6 thông số cảm biến:

| STT | Thông số | Firebase key | Đơn vị |
|---:|---|---|---|
| 1 | Nhiệt độ | `temperature` | °C |
| 2 | Độ ẩm không khí | `humidity` | % |
| 3 | Độ ẩm đất | `soilMoisture` | % |
| 4 | Ánh sáng | `light` | lux |
| 5 | CO₂ | `co2` | ppm |
| 6 | Mực nước | `waterLevel` | % |

### 3.2. Điều khiển thiết bị

Dashboard và app Android điều khiển 4 thiết bị:

| STT | Thiết bị | Firebase key | Giá trị |
|---:|---|---|---|
| 1 | Đèn trồng cây | `growLight` | `0` hoặc `1` |
| 2 | Quạt thông gió | `fan` | `0` hoặc `1` |
| 3 | Máy bơm nước | `waterPump` | `0` hoặc `1` |
| 4 | Máy sưởi | `heater` | `0` hoặc `1` |

Trong đó:

```text
0 = OFF
1 = ON
```

### 3.3. Hỗ trợ nhiều khu vực

Hệ thống hỗ trợ 4 khu vực:

| STT | Khu vực hiển thị | Firebase area key |
|---:|---|---|
| 1 | Hà Nội | `hanoi` |
| 2 | Gia Lai | `gialai` |
| 3 | Hồ Chí Minh | `hochiminh` |
| 4 | Cà Mau | `camau` |

---

## 4. Kiến trúc hệ thống

```text
+-------------------+        Internet        +-----------------------------+
|                   |  ghi/đọc dữ liệu       |                             |
|  Android App      | <---------------------> |  Firebase Realtime Database |
|  MIT WebViewer    |                         |                             |
+-------------------+                         +-----------------------------+
          ^                                                   ^
          |                                                   |
          | mở dashboard HTML                                 |
          |                                                   |
+-------------------+                         +-----------------------------+
|                   |  ghi cảm biến           |                             |
|  Web Dashboard    | <---------------------> |  ESP32 Wokwi Simulation     |
|  HTML/CSS/JS      |  đọc trạng thái         |  Firebase REST API          |
|                   |                         |                             |
+-------------------+                         +-----------------------------+
```

Nguyên lý hoạt động:

1. ESP32 Wokwi tạo dữ liệu cảm biến ngẫu nhiên.
2. ESP32 gửi dữ liệu lên Firebase theo từng khu vực.
3. Dashboard web đọc dữ liệu cảm biến từ Firebase và hiển thị lên giao diện.
4. Người dùng bấm ON/OFF trên dashboard hoặc app Android.
5. Dashboard ghi trạng thái thiết bị lên Firebase.
6. ESP32 đọc trạng thái thiết bị từ Firebase.
7. ESP32 bật/tắt LED mô phỏng thiết bị trên Wokwi.

---

## 5. Công nghệ sử dụng

| Thành phần | Công nghệ |
|---|---|
| Vi điều khiển mô phỏng | ESP32 |
| Môi trường mô phỏng | Wokwi |
| Cơ sở dữ liệu | Firebase Realtime Database |
| Giao diện web | HTML, CSS, JavaScript |
| Biểu đồ | Chart.js |
| App Android | MIT App Inventor WebViewer |
| Giao tiếp ESP32 - Firebase | Firebase REST API qua HTTP/HTTPS |
| Giao tiếp dashboard - Firebase | Firebase JavaScript SDK |

---

## 6. Cấu trúc thư mục dự án

```text
smart-greenhouse-iot-firebase-android/
│
├── web-dashboard/
│   ├── index.html
│   ├── style.css
│   ├── function.js
│   └── assets/
│       ├── greenhouse.png
│       ├── temperature.png
│       ├── humidity.png
│       ├── soil.png
│       ├── sun.png
│       ├── co2.png
│       ├── waterlevel.png
│       ├── light_on.png
│       ├── light_off.png
│       ├── fan_on.png
│       ├── fan_off.png
│       ├── waterpump_on.png
│       ├── waterpump_off.png
│       ├── heating_on.png
│       └── heating_off.png
│
├── wokwi-esp32/
│   ├── esp32_greenhouse_firebase.ino
│   ├── diagram.json
│   └── libraries.txt
│
├── android-app/
│   ├── MIT_App_Inventor_Guide.md
│   └── SmartGreenhouseApp.apk
│
├── firebase/
│   ├── database_structure.json
│   └── firebase_rules.json
│
├── docs/
│   ├── system_architecture.png
│   ├── firebase_structure.png
│   ├── dashboard_screenshot.png
│   └── android_app_screenshot.png
│
└── README.md
```

---

## 7. Cấu trúc Firebase Realtime Database

Firebase sử dụng cấu trúc dữ liệu như sau:

```json
{
  "greenhouse": {
    "hanoi": {
      "sensors": {
        "temperature": 30.1,
        "humidity": 70.5,
        "soilMoisture": 55.2,
        "light": 800.4,
        "co2": 450,
        "waterLevel": 81.3
      },
      "devices": {
        "growLight": 0,
        "fan": 0,
        "waterPump": 0,
        "heater": 0
      }
    },
    "gialai": {
      "sensors": {
        "temperature": 26.5,
        "humidity": 78.2,
        "soilMoisture": 63.4,
        "light": 720.5,
        "co2": 430,
        "waterLevel": 76.1
      },
      "devices": {
        "growLight": 0,
        "fan": 0,
        "waterPump": 0,
        "heater": 0
      }
    },
    "hochiminh": {
      "sensors": {
        "temperature": 32.4,
        "humidity": 68.9,
        "soilMoisture": 52.7,
        "light": 1050.0,
        "co2": 620,
        "waterLevel": 69.3
      },
      "devices": {
        "growLight": 0,
        "fan": 0,
        "waterPump": 0,
        "heater": 0
      }
    },
    "camau": {
      "sensors": {
        "temperature": 29.8,
        "humidity": 88.5,
        "soilMoisture": 75.6,
        "light": 640.2,
        "co2": 500,
        "waterLevel": 90.4
      },
      "devices": {
        "growLight": 0,
        "fan": 0,
        "waterPump": 0,
        "heater": 0
      }
    }
  }
}
```

---

## 8. Firebase Rules dùng để demo

Trong quá trình demo, có thể dùng rules mở để dễ kiểm thử:

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

Lưu ý: rules này chỉ dùng cho demo hoặc học tập. Khi triển khai thực tế cần cấu hình xác thực người dùng bằng Firebase Authentication.

---

## 9. Web Dashboard

### 9.1. Chức năng của dashboard

Dashboard web có các thành phần:

- Header hiển thị tên hệ thống.
- Thanh chọn khu vực gồm Hà Nội, Gia Lai, Hồ Chí Minh, Cà Mau.
- Khu vực hiển thị dữ liệu cảm biến.
- Khu vực điều khiển thiết bị.
- Biểu đồ cột hiển thị giá trị cảm biến.
- Footer hiển thị thông tin sinh viên.

### 9.2. Kết nối Firebase trong JavaScript

Dashboard sử dụng Firebase JavaScript SDK:

```html
<script src="https://www.gstatic.com/firebasejs/8.2.10/firebase-app.js"></script>
<script src="https://www.gstatic.com/firebasejs/8.2.10/firebase-database.js"></script>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script src="function.js"></script>
```

File `function.js` đọc dữ liệu tại:

```text
greenhouse/{area}/sensors
```

và ghi điều khiển tại:

```text
greenhouse/{area}/devices/{deviceKey}
```

### 9.3. Luồng đọc cảm biến

```text
Firebase Realtime Database
greenhouse/hanoi/sensors
        ↓
function.js
        ↓
Cập nhật các thẻ span trên index.html
        ↓
Hiển thị lên dashboard
```

### 9.4. Luồng điều khiển thiết bị

```text
Người dùng bấm ON/OFF
        ↓
onclick="setDevice('growLight', 1)"
        ↓
function.js ghi lên Firebase
        ↓
greenhouse/hanoi/devices/growLight = 1
        ↓
ESP32 đọc và bật LED mô phỏng
```

---

## 10. Ứng dụng Android bằng MIT App Inventor

### 10.1. Lý do dùng WebViewer

Vì dashboard đã được thiết kế bằng HTML/CSS/JS nên không cần kéo lại toàn bộ giao diện trong MIT App Inventor. MIT App Inventor chỉ cần dùng một thành phần:

```text
WebViewer
```

để mở file:

```text
index.html
```

Như vậy giao diện trên app Android giống với dashboard web.

### 10.2. Các file cần upload vào Media

Trong MIT App Inventor, upload các file sau vào Media:

```text
index.html
style.css
function.js
greenhouse.png
temperature.png
humidity.png
soil.png
sun.png
co2.png
waterlevel.png
light_on.png
light_off.png
fan_on.png
fan_off.png
waterpump_on.png
waterpump_off.png
heating_on.png
heating_off.png
```

### 10.3. Cấu hình WebViewer

Trong MIT App Inventor:

```text
WebViewer1.Width = Fill parent
WebViewer1.Height = Fill parent
WebViewer1.HomeUrl = http://localhost/index.html
```

### 10.4. Build APK

Sau khi dashboard hiển thị thành công trong AI Companion:

```text
Build → Android App (.apk)
```

Sau đó cài file APK lên điện thoại Android và kiểm tra điều khiển Firebase.

---

## 11. ESP32 Wokwi Simulation

### 11.1. Vai trò của ESP32

ESP32 trong Wokwi thực hiện:

1. Kết nối WiFi Wokwi.
2. Tạo dữ liệu cảm biến ngẫu nhiên.
3. Gửi dữ liệu cảm biến lên Firebase bằng HTTP PUT.
4. Đọc trạng thái thiết bị từ Firebase bằng HTTP GET.
5. Bật/tắt LED mô phỏng thiết bị.

### 11.2. Kết nối LED mô phỏng

| Thiết bị | GPIO ESP32 | LED |
|---|---:|---|
| Đèn trồng cây | GPIO 2 | LED 1 |
| Quạt thông gió | GPIO 4 | LED 2 |
| Máy bơm nước | GPIO 5 | LED 3 |
| Máy sưởi | GPIO 18 | LED 4 |

Mỗi LED nối:

```text
GPIO ESP32 → điện trở 220Ω → chân dương LED
chân âm LED → GND
```

### 11.3. Điều khiển theo khu vực

Mặc định code Wokwi đọc lệnh điều khiển của khu vực:

```cpp
String controlArea = "hanoi";
```

Nếu muốn LED Wokwi nhận lệnh của khu vực khác, sửa thành:

```cpp
String controlArea = "gialai";
```

hoặc:

```cpp
String controlArea = "hochiminh";
```

hoặc:

```cpp
String controlArea = "camau";
```

---

## 12. Hướng dẫn chạy dự án

### 12.1. Chạy web dashboard trên máy tính

1. Mở thư mục `web-dashboard` bằng VS Code.
2. Cài extension Live Server.
3. Chuột phải vào `index.html`.
4. Chọn `Open with Live Server`.
5. Dashboard sẽ mở trên trình duyệt.

### 12.2. Chạy ESP32 trên Wokwi

1. Tạo project ESP32 trên Wokwi.
2. Dán code vào file `.ino`.
3. Nối LED theo bảng chân GPIO.
4. Chạy mô phỏng.
5. Mở Serial Monitor để kiểm tra HTTP Response Code.
6. Nếu trả về `200`, ESP32 đã gửi/đọc Firebase thành công.

### 12.3. Chạy app Android

1. Mở project MIT App Inventor.
2. Upload đầy đủ file vào Media.
3. Cấu hình WebViewer.
4. Test bằng AI Companion.
5. Build APK.
6. Cài APK lên điện thoại Android.
7. Bấm ON/OFF và kiểm tra Firebase.

---

## 13. Cách kiểm tra hoạt động

### Kiểm tra cảm biến

Khi Wokwi chạy, Firebase phải cập nhật:

```text
greenhouse/hanoi/sensors/temperature
greenhouse/hanoi/sensors/humidity
greenhouse/hanoi/sensors/soilMoisture
greenhouse/hanoi/sensors/light
greenhouse/hanoi/sensors/co2
greenhouse/hanoi/sensors/waterLevel
```

Dashboard sẽ tự động hiển thị giá trị mới.

### Kiểm tra điều khiển

Bấm nút trên dashboard:

```text
Đèn ON
```

Firebase phải đổi:

```text
greenhouse/hanoi/devices/growLight = 1
```

LED nối GPIO 2 trên Wokwi sẽ sáng.

Bấm:

```text
Đèn OFF
```

Firebase đổi:

```text
greenhouse/hanoi/devices/growLight = 0
```

LED GPIO 2 sẽ tắt.

---

## 14. Một số lỗi thường gặp

### 14.1. Dashboard không hiện cảm biến

Nguyên nhân thường gặp:

- ESP32 gửi sai đường dẫn Firebase.
- Tên key cảm biến không đúng.
- Dashboard đang chọn khu vực khác với khu vực ESP32 gửi.
- Chưa upload lại `function.js` vào MIT App Inventor.
- Firebase Rules chưa cho phép đọc/ghi.

Đường dẫn đúng:

```text
greenhouse/{area}/sensors
```

Tên key đúng:

```text
temperature
humidity
soilMoisture
light
co2
waterLevel
```

### 14.2. Bấm ON/OFF nhưng LED Wokwi không sáng

Kiểm tra:

- Dashboard có ghi `devices` lên Firebase không.
- ESP32 đang đọc đúng `controlArea` không.
- LED có nối đúng GPIO không.
- Serial Monitor có báo HTTP Response Code `200` không.

### 14.3. App Android hiện trắng

Kiểm tra:

- `HomeUrl` của WebViewer có đúng `http://localhost/index.html` không.
- File `index.html`, `style.css`, `function.js` đã upload vào Media chưa.
- Tên ảnh có bị sai không.
- Trong HTML/JS còn đường dẫn `img/` không.

---

## 15. Kết quả đạt được

Dự án đã hoàn thành các phần:

- Xây dựng dashboard web bằng HTML/CSS/JavaScript.
- Kết nối dashboard với Firebase Realtime Database.
- Hiển thị dữ liệu cảm biến theo thời gian thực.
- Điều khiển thiết bị ON/OFF từ dashboard.
- Mô phỏng ESP32 trên Wokwi gửi dữ liệu cảm biến.
- ESP32 đọc lệnh thiết bị từ Firebase và điều khiển LED.
- Đóng gói dashboard thành app Android bằng MIT App Inventor WebViewer.
- Hỗ trợ nhiều khu vực: Hà Nội, Gia Lai, Hồ Chí Minh, Cà Mau.

---

## 16. Hướng phát triển

Có thể phát triển thêm:

- Đăng nhập người dùng bằng Firebase Authentication.
- Lưu lịch sử cảm biến theo thời gian.
- Vẽ biểu đồ đường theo thời gian thực.
- Cảnh báo khi nhiệt độ, CO₂ hoặc mực nước vượt ngưỡng.
- Tự động bật/tắt thiết bị theo điều kiện cảm biến.
- Điều khiển thiết bị thật bằng relay thay vì LED mô phỏng.
- Thiết kế app Android native bằng Flutter hoặc React Native.
- Triển khai dashboard lên Netlify, Vercel hoặc Firebase Hosting.

---

## 17. Thông tin tác giả

```text
Sinh viên: NGUYEN PHI QUOC BAO
MSSV: 
Email: quocbaodeptrai@gmail.com
```

---

## 18. License

Dự án phục vụ mục đích học tập, mô phỏng và nghiên cứu hệ thống IoT cơ bản.

```text
MIT License
```
