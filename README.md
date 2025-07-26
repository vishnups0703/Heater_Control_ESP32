# Heater Control System with FreeRTOS
This project demonstrates a **Heater Control System** using **ESP32** with **FreeRTOS**. It uses a **DHT22** sensor to monitor temperature and humidity, an **OLED screen** to display real-time data, and a **relay** (mimicking a heater) along with an **LED** to indicate the heater's status.

## 🧰 Components Used

- ESP32
- DHT22 (Temperature & Humidity Sensor)
- OLED Display (SSD1306)
- Relay Module (acts as Heater)
- LED (indicates heater status)
- FreeRTOS (Real-Time Operating System)

## 📋 Features

- Periodic reading of temperature and humidity via DHT22
- Displays data on OLED screen
- Turns on relay and LED when temperature is below a set threshold
- Implements FreeRTOS tasks for multitasking

## 🖥️ Simulation

**Simulation Image:**  
![Simulation Screenshot](simulation_image.png)

**Simulation Link:**  
[Click here to view the simulation](https://wokwi.com/projects/437156056135982081)  
