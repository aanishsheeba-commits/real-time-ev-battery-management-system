# Task 2 - Event-Driven Safety Protection Kernel

## Overview
Developed a non-blocking automotive-grade safety protection controller for a simulated 4-cell lithium battery pack.

## Features
- Weak cell voltage detection
- Overvoltage detection
- Sensor anomaly detection
- Rapid voltage fluctuation detection
- Relay safety cutoff
- Buzzer alerts
- 16x2 I2C LCD warnings
- millis()-based non-blocking timing
- Recovery logic
- Anti-relay chatter protection
- Stable state management

## Hardware
- ESP32
- 4 Potentiometers for simulated cell voltages
- Relay Module
- Buzzer
- 16x2 I2C LCD

## Pin Configuration
- Cell 1: GPIO 34
- Cell 2: GPIO 35
- Cell 3: GPIO 32
- Cell 4: GPIO 33
- Relay: GPIO 25
- Buzzer: GPIO 26
- LCD SDA: GPIO 21
- LCD SCL: GPIO 22

## Safety States
NORMAL → WARNING → FAULT → RECOVERY → NORMAL

## Safety Thresholds
- Below 3.0 V: Weak Cell
- Above 4.2 V: Overvoltage
- Sudden voltage variation: Rapid Change
- Critical faults: Relay OFF

## Technology
ESP32, Embedded C++, Arduino, Event-Driven Architecture, millis()-based Timing, Battery Management Systems.
