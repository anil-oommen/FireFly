# Firefly ESP Tank

This project implements a remote-controlled tank using an ESP8266 microcontroller, an MPU6050 gyroscope/accelerometer, and a web-based interface for control. The communication between the tank and the web interface is handled via the MQTT protocol.

## Features

*   **Remote Control:** Control the tank's movement through a web interface.
*   **Web-Based UI:** The web interface is built with Angular and provides:
    *   A virtual joystick for intuitive control.
    *   Sliders for direct PWM signal manipulation of the motors.
*   **MQTT Communication:** The ESP8266 and the web app communicate over an MQTT broker, allowing for real-time control.
*   **Gyro/Accelerometer:** An MPU6050 is used for motion sensing, although the current implementation focuses on remote control.

## Architecture

The project is divided into two main components:

1.  **The Tank (ESP8266 Firmware):** The Arduino code running on the ESP8266.
2.  **The Controller (Angular Web App):** The web interface for controlling the tank.

### Tank (FireflyDCTank.ino)

*   **Hardware:**
    *   ESP8266 (NodeMCU)
    *   MPU6050 Gyro/Accelerometer
    *   L298N Motor Driver (or similar)
*   **Functionality:**
    *   Connects to a Wi-Fi network.
    *   Connects to an MQTT broker.
    *   Subscribes to the `firefly/tank/control` topic to receive commands from the web interface.
    *   Controls the motors based on the received commands.

### Controller (FireflyNgWeb)

*   **Framework:** Angular
*   **Functionality:**
    *   Connects to the MQTT broker via WebSockets.
    *   Provides a user interface with a virtual joystick and sliders.
    *   Publishes control commands to the `firefly/tank/control` MQTT topic.

## Installation and Setup

### 1. MQTT Broker

You need an MQTT broker that is accessible to both the ESP8266 and the device running the web interface. For testing, you can use a public broker like `broker.hivemq.com`.

### 2. Tank Firmware (FireflyDCTank)

**Prerequisites:**

*   [Arduino IDE](https://www.arduino.cc/en/software)
*   ESP8266 Board Support for the Arduino IDE. You can add it by going to `File > Preferences` and adding the following URL to the "Additional Boards Manager URLs":
    ```
    http://arduino.esp8266.com/stable/package_esp8266com_index.json
    ```

**Libraries:**

Install the following libraries through the Arduino IDE's Library Manager (`Sketch > Include Library > Manage Libraries...`):

*   `PubSubClient` by Nick O'Leary
*   `Adafruit MPU6050` by Adafruit
*   `Adafruit Unified Sensor Driver` by Adafruit
*   `Adafruit BusIO` by Adafruit

**Configuration:**

1.  Open `FireflyDCTank/FireflyDCTank.ino` in the Arduino IDE.
2.  Modify the following lines to set your Wi-Fi credentials and MQTT broker address:

    ```cpp
    const char* ssid = "YOUR_WIFI_SSID";
    const char* password = "YOUR_WIFI_PASSWORD";
    const char* mqtt_server = "YOUR_MQTT_BROKER_ADDRESS"; // e.g., "broker.hivemq.com"
    ```

**Flashing:**

1.  Connect your ESP8266 to your computer.
2.  In the Arduino IDE, select your ESP8266 board from the `Tools > Board` menu (e.g., "NodeMCU 1.0 (ESP-12E Module)").
3.  Select the correct COM port from the `Tools > Port` menu.
4.  Click the "Upload" button.

### 3. Web Interface (FireflyNgWeb)

**Prerequisites:**

*   [Node.js and npm](https://nodejs.org/)

**Installation:**

1.  Open a terminal and navigate to the `FireflyNgWeb` directory.
2.  Install the dependencies:
    ```bash
    npm install
    ```

**Configuration:**

1.  Open `FireflyNgWeb/src/environments/environment.ts`.
2.  Modify the `mqtt` configuration to match your MQTT broker's WebSocket connection details:

    ```typescript
    export const environment = {
      production: false,
      mqtt: {
        server: 'YOUR_MQTT_BROKER_ADDRESS', // e.g., 'broker.hivemq.com'
        protocol: 'wss', // or 'ws'
        port: 8084 // or your broker's WebSocket port
      }
    };
    ```
    *Note: For a local broker, you might use `ws` protocol and a different port.*

**Running the Web App:**

1.  In the `FireflyNgWeb` directory, run the following command:
    ```bash
    ng serve
    ```
2.  Open your web browser and navigate to `http://localhost:4200/`.

## How to Use

1.  **Power on** the ESP8266 tank. It should connect to your Wi-Fi and the MQTT broker.
2.  **Start the web interface** by running `ng serve` in the `FireflyNgWeb` directory.
3.  **Open the web interface** in your browser (`http://localhost:4200/`).
4.  You should now be able to **control the tank** using the virtual joystick or the PWM sliders.

## Project Structure

```
.
├── FireflyDCTank/      # Arduino firmware for the ESP8266
│   └── FireflyDCTank.ino
├── FireflyNgWeb/       # Angular web application for control
│   ├── src/
│   └── package.json
└── README.md           # This file
```

## Future Development (from @OLD_DONT_USE)

The following are notes and resources for future development of the project, specifically regarding the integration of the MPU6050 for more advanced features like auto-correction and PID control.

### MPU-6050 and Digital Motion Processor (DMP)

*   **MPU-6050 Interfacing with NodeMCU:** [https://www.electronicwings.com/nodemcu/mpu6050-interfacing-with-nodemcu](https://www.electronicwings.com/nodemcu/mpu6050-interfacing-with-nodemcu)
*   **i2cdevlib for MPU6050:** The MPU6050 has a powerful Digital Motion Processor (DMP) that can offload sensor fusion and filtering from the main microcontroller. The `i2cdevlib` library by Jeff Rowberg is essential for using the DMP.
    *   **Repository:** [https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050)
    *   **DMP Example for ESP8266:** [https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6_ESPWiFi/MPU6050_DMP6_ESPWiFi.ino](https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6_ESPWiFi/MPU6050_DMP6_ESPWiFi.ino)

    **Installation of i2cdevlib:**

    1.  Download the repository: [https://github.com/jrowberg/i2cdevlib/archive/master.zip](https://github.com/jrowberg/i2cdevlib/archive/master.zip)
    2.  Unzip the downloaded file.
    3.  In the Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library`.
    4.  Select the `i2cdevlib-master/Arduino/MPU6050` folder.

### PID Control for Auto-Correction

A PID (Proportional-Integral-Derivative) controller can be used with the gyroscope data to provide auto-correction for the tank's movement, helping it to drive in a straight line.

*   **PID Library for Arduino:** [http://playground.arduino.cc/Code/PIDLibrary](http://playground.arduino.cc/Code/PIDLibrary)
*   **PID Autotune Library:** [https://playground.arduino.cc/Code/PIDAutotuneLibrary](https://playground.arduino.cc/Code/PIDAutotuneLibrary)
*   **Introduction to PID Controllers:** [http://wired.chillibasket.com/2015/03/pid-controller/](http://wired.chillibasket.com/2015/03/pid-controller/)
