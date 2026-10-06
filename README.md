# Real-Time EV Battery Management System

## Project Title
Adaptive Multi-Cell Battery Intelligence Engine

## 1. Project Overview

This project implements a simulated real-time Battery Management System (BMS) for a 4-cell lithium battery pack.

The system continuously monitors individual cell voltages and processes the measurements to determine the condition of the battery pack.

## 2. Objectives

- Monitor individual battery cell voltages.
- Calculate total battery pack voltage.
- Calculate average cell voltage.
- Identify the strongest and weakest cells.
- Calculate cell voltage imbalance.
- Classify battery health.
- Transmit monitoring data to ThingSpeak.
- Provide real-time monitoring through serial output and a cloud dashboard.

## 3. System Implementation

The system was developed using an ESP32-based simulation in Wokwi.

Four analog inputs are used to represent the voltage conditions of four individual battery cells.

The measured values are processed by the ESP32 to calculate battery-level parameters and determine the overall battery condition.

## 4. Battery Parameters

The system calculates:

- Cell 1 voltage
- Cell 2 voltage
- Cell 3 voltage
- Cell 4 voltage
- Pack voltage
- Average cell voltage
- Strongest cell
- Weakest cell
- Cell voltage imbalance
- Battery health state

## 5. Battery Health Classification

The battery intelligence engine evaluates the cell measurements and classifies the battery into different operating states:

- Healthy
- Minor Imbalance
- Critical Imbalance
- Pack Failure

The classification is based on the configured voltage and imbalance conditions in the source code.

## 6. IoT Integration

ThingSpeak is used for cloud-based monitoring.

The ESP32 sends battery monitoring information to ThingSpeak using HTTP communication.

Successful communication was verified during testing with an HTTP 200 response.

## 7. Testing

A healthy battery test produced the following result:

| Parameter | Result |
|---|---:|
| Cell 1 | 3.609 V |
| Cell 2 | 3.591 V |
| Cell 3 | 3.604 V |
| Cell 4 | 3.609 V |
| Pack Voltage | 14.413 V |
| Average Cell Voltage | 3.603 V |
| Strongest Cell | Cell 1 |
| Weakest Cell | Cell 2 |
| Imbalance | 0.50% |
| Battery State | HEALTHY |
| ThingSpeak Response | HTTP 200 |

Additional tests were performed for the configured battery health states, including minor imbalance, critical imbalance and pack failure conditions.

## 8. Technologies Used

- ESP32
- Arduino/C++
- Wokwi
- ThingSpeak
- Analog voltage monitoring
- HTTP communication
- GitHub
- GitHub Pages
- HTML/CSS

## 9. Conclusion

The implemented battery intelligence engine demonstrates real-time monitoring and analysis of a simulated 4-cell EV battery pack.

The system can identify cell-level voltage conditions, determine the strongest and weakest cells, calculate imbalance, classify battery health and transmit monitoring information to a cloud dashboard.

The implementation provides a foundation for further development toward a hardware-based EV Battery Management System.
