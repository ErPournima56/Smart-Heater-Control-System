# Smart Heater Control System

This project is a simulation of a smart embedded heater controller using the **ESP32**, **DS18B20 temperature sensor**, **OLED display**, and a **relay module**. It demonstrates a temperature-based state machine to control heating intelligently based on ambient conditions.

---

##  Features

- Monitors temperature using DS18B20 sensor
- Finite State Machine with 4 key states:
  - **IDLE**
  - **HEATING**
  - **STABILIZING**
  - **OVERHEAT**
- Displays system state and temperature on OLED
- Controls a heater using a relay module
- Modular logic designed for power optimization and future expansion
- Fully simulated in [Wokwi](https://wokwi.com/projects/438353317941401601)

---

## Hardware Components (Simulated)

- ESP32 DevKit
- DS18B20 Digital Temperature Sensor
- SSD1306 OLED Display (I2C)
- 1-Channel Relay Module
- 4.7kΩ Pull-up resistor (for DS18B20)

---

## Project Links

-  **Wokwi Simulation:** [Open in Wokwi](https://wokwi.com/projects/438353317941401601)

---

##  State Machine Logic

| State        | When It Activates                         | What It Does                                |
|--------------|--------------------------------------------|----------------------------------------------|
| `IDLE`       | Temp ≥ comfort range / timeout / user off | Heater OFF, OLED shows "IDLE"                |
| `HEATING`    | Temp < comfort range                      | Relay ON, OLED shows "HEATING"               |
| `STABILIZING`| Temp ≥ target - 0.5°C                     | Maintain temp, OLED shows "STABILIZING"      |
| `OVERHEAT`   | Temp ≥ MAX_SAFE_TEMP                      | Heater OFF, OLED shows "OVERHEAT"            |

---

##  Power Notes

In a real-world application:
- A **battery** would be required (not simulated here).
- To conserve power, the ESP32 can enter **deep sleep** and wake up periodically (e.g., every 1–2 hours) to read temperature.
- This version keeps all components active for simplicity.

---

## Future Enhancements

- Add deep sleep & timed wake-up logic
- Sync time via NTP for scheduled heating
- Add MQTT or BLE support for remote control
- Support custom heating profiles and schedules

---

##  Author

**[Pournima Choulwar]**  
Embedded Engineer
