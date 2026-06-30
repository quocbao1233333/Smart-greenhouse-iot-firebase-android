// ==============================
// Firebase Realtime Database config
// ==============================
const firebaseConfig = {
  apiKey: "AIzaSyAE7wuyRHBgZGm5kjzMk8V3j5ewgjii4nY",
  authDomain: "mirageuz-1610lma.firebaseapp.com",
  databaseURL: "https://smart-city-iot-dashboard-default-rtdb.firebaseio.com",
  projectId: "mirageuz-1610lma",
  storageBucket: "mirageuz-1610lma.firebasestorage.app",
  messagingSenderId: "115800000806",
  appId: "1:115800000806:web:6cad2593811a399e6559e7",
  measurementId: "G-RT8PYTRSTF"
};

// ==============================
// Area and data definitions
// ==============================
const areaNames = {
  hanoi: "Hà Nội",
  gialai: "Gia Lai",
  hochiminh: "Hồ Chí Minh",
  camau: "Cà Mau"
};

const sensorConfig = [
  {
    key: "temperature",
    label: "Nhiệt độ",
    unit: "°C",
    elementId: "temperatureValue"
  },
  {
    key: "humidity",
    label: "Độ ẩm",
    unit: "%",
    elementId: "humidityValue"
  },
  {
    key: "soilMoisture",
    label: "Độ ẩm đất",
    unit: "%",
    elementId: "soilMoistureValue"
  },
  {
    key: "light",
    label: "Ánh sáng",
    unit: "lux",
    elementId: "lightValue"
  },
  {
    key: "co2",
    label: "CO₂",
    unit: "ppm",
    elementId: "co2Value"
  },
  {
    key: "waterLevel",
    label: "Mực nước",
    unit: "%",
    elementId: "waterLevelValue"
  }
];

// ==============================
// Device definitions
// ==============================
// Dùng cho MIT App Inventor WebViewer.
// Ảnh upload vào Media nên để cùng cấp với index.html.
// Vì vậy dùng "light_on.png", không dùng "img/light_on.png".
const deviceConfig = {
  growLight: {
    statusId: "growLightStatus",
    iconId: "growLightIcon",
    onIcon: "light_on.png",
    offIcon: "light_off.png"
  },
  fan: {
    statusId: "fanStatus",
    iconId: "fanIcon",
    onIcon: "fan_on.png",
    offIcon: "fan_off.png"
  },
  waterPump: {
    statusId: "waterPumpStatus",
    iconId: "waterPumpIcon",
    onIcon: "waterpump_on.png",
    offIcon: "waterpump_off.png"
  },
  heater: {
    statusId: "heaterStatus",
    iconId: "heaterIcon",
    onIcon: "heating_on.png",
    offIcon: "heating_off.png"
  }
};

// ==============================
// Global variables
// ==============================
let currentArea = "hanoi";
let sensorListenerRef = null;
let deviceListenerRef = null;
let sensorBarChart = null;
let database = null;

// ==============================
// Safe DOM helper
// ==============================
function setText(elementId, text) {
  const element = document.getElementById(elementId);

  if (element) {
    element.innerText = text;
  }
}

// ==============================
// Clock
// ==============================
function updateClock() {
  const now = new Date();

  const timeString = now.toLocaleTimeString("vi-VN", {
    hour12: false
  });

  setText("clock", timeString);
}

// ==============================
// Chart.js bar chart
// ==============================
function createBarChart() {
  const canvas = document.getElementById("sensorBarChart");

  if (!canvas) {
    console.log("Không tìm thấy sensorBarChart.");
    return;
  }

  if (typeof Chart === "undefined") {
    console.log("Chart.js chưa tải xong.");
    return;
  }

  const ctx = canvas.getContext("2d");

  sensorBarChart = new Chart(ctx, {
    type: "bar",
    data: {
      labels: sensorConfig.map(function (sensor) {
        return sensor.label;
      }),
      datasets: [
        {
          label: "Giá trị cảm biến",
          data: [0, 0, 0, 0, 0, 0],
          borderWidth: 1
        }
      ]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: {
          display: true
        },
        tooltip: {
          callbacks: {
            label: function (context) {
              const sensor = sensorConfig[context.dataIndex];
              return sensor.label + ": " + context.raw + " " + sensor.unit;
            }
          }
        }
      },
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });
}

function updateBarChart(sensorData) {
  if (!sensorBarChart) {
    return;
  }

  const chartValues = sensorConfig.map(function (sensor) {
    let value = 0;

    if (sensorData && sensorData[sensor.key] !== undefined) {
      value = sensorData[sensor.key];
    }

    return Number(value);
  });

  sensorBarChart.data.datasets[0].data = chartValues;
  sensorBarChart.update();
}

// ==============================
// Firebase RTDB read functions
// Path: greenhouse/{area}/sensors
// Path: greenhouse/{area}/devices
// ==============================
function listenSensorData(area) {
  if (!database) {
    setConnectionStatus("Chưa khởi tạo Firebase", true);
    return;
  }

  if (sensorListenerRef) {
    sensorListenerRef.off();
  }

  sensorListenerRef = database.ref("greenhouse/" + area + "/sensors");

  sensorListenerRef.on(
    "value",
    function (snapshot) {
      const data = snapshot.val() || {};

      sensorConfig.forEach(function (sensor) {
        let value = 0;

        if (data[sensor.key] !== undefined) {
          value = data[sensor.key];
        }

        setText(sensor.elementId, value);
      });

      updateBarChart(data);
      setConnectionStatus("Đã kết nối RTDB", false);
    },
    function (error) {
      console.error(error);
      setConnectionStatus("Lỗi đọc dữ liệu RTDB", true);
    }
  );
}

function listenDeviceData(area) {
  if (!database) {
    setConnectionStatus("Chưa khởi tạo Firebase", true);
    return;
  }

  if (deviceListenerRef) {
    deviceListenerRef.off();
  }

  deviceListenerRef = database.ref("greenhouse/" + area + "/devices");

  deviceListenerRef.on(
    "value",
    function (snapshot) {
      const data = snapshot.val() || {};

      Object.keys(deviceConfig).forEach(function (deviceKey) {
        const isOn = normalizeStatus(data[deviceKey]);
        updateDeviceUI(deviceKey, isOn);
      });
    },
    function (error) {
      console.error(error);
      setConnectionStatus("Lỗi đọc trạng thái thiết bị", true);
    }
  );
}

// ==============================
// Firebase RTDB write function
// ==============================
function setDevice(deviceKey, status) {
  if (!database) {
    setConnectionStatus("Chưa khởi tạo Firebase", true);
    return;
  }

  database
    .ref("greenhouse/" + currentArea + "/devices/" + deviceKey)
    .set(status)
    .then(function () {
      updateDeviceUI(deviceKey, status === 1);
      setConnectionStatus("Đã gửi lệnh điều khiển", false);
    })
    .catch(function (error) {
      console.error(error);
      setConnectionStatus("Lỗi gửi lệnh điều khiển", true);
    });
}

// Cho phép gọi setDevice() trực tiếp từ onclick trong HTML
window.setDevice = setDevice;

// ==============================
// UI helpers
// ==============================
function normalizeStatus(value) {
  return (
    value === 1 ||
    value === true ||
    value === "ON" ||
    value === "on" ||
    value === "1"
  );
}

function updateDeviceUI(deviceKey, isOn) {
  const device = deviceConfig[deviceKey];

  if (!device) {
    return;
  }

  const statusElement = document.getElementById(device.statusId);
  const iconElement = document.getElementById(device.iconId);

  if (statusElement) {
    statusElement.innerText = isOn ? "ON" : "OFF";
    statusElement.classList.toggle("is-on", isOn);
  }

  if (iconElement) {
    iconElement.src = isOn ? device.onIcon : device.offIcon;
  }
}

function setConnectionStatus(message, isError) {
  const status = document.getElementById("connectionStatus");

  if (!status) {
    return;
  }

  status.innerText = message;
  status.classList.toggle("error", isError);
}

function selectArea(area) {
  currentArea = area;

  setText("areaTitle", "Khu vực: " + areaNames[area]);
  setText("chartAreaName", areaNames[area]);

  document.querySelectorAll(".area-btn").forEach(function (button) {
    button.classList.toggle("active", button.dataset.area === area);
  });

  listenSensorData(area);
  listenDeviceData(area);
}

function setupAreaButtons() {
  document.querySelectorAll(".area-btn").forEach(function (button) {
    button.addEventListener("click", function () {
      selectArea(button.dataset.area);
    });
  });
}

// ==============================
// Init
// ==============================
document.addEventListener("DOMContentLoaded", function () {
  try {
    if (typeof firebase === "undefined") {
      setConnectionStatus("Lỗi: chưa tải thư viện Firebase", true);
      return;
    }

    firebase.initializeApp(firebaseConfig);
    database = firebase.database();

    updateClock();
    setInterval(updateClock, 1000);

    createBarChart();
    setupAreaButtons();
    selectArea(currentArea);
  } catch (error) {
    console.error(error);
    setConnectionStatus("Lỗi khởi tạo hệ thống", true);
  }
});
