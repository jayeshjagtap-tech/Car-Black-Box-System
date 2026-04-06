# 🚗 Car Black Box System (PIC16F877A)

## 📌 Overview
This project is a microcontroller-based Car Black Box system developed using PIC16F877A. It records vehicle events along with time and speed, providing features like secure login, log management, and real-time monitoring.

## ⚙️ Features
- 🔐 Password Protected Login
- 📊 Dashboard Display (Time, Event, Speed)
- 📁 View Logs (stored in EEPROM)
- 🗑️ Clear Logs
- 📥 Download Logs via UART
- ⏰ Set Time using DS1307 RTC
- 🔄 Change Password
- 🎛️ Menu-driven interface using CLCD & Keypad

## 🛠️ Technologies Used
- Embedded C
- MPLAB X IDE
- XC8 Compiler
- PIC16F877A
- EEPROM
- DS1307 RTC
- I2C Communication
- UART

## 🧠 How it Works
- Events are logged into EEPROM with timestamp and speed.
- User logs in using password.
- Menu allows viewing, clearing, downloading logs, and setting time.
- RTC maintains real-time clock.
- UART is used to download logs.

## 👨‍💻 Author
Jayesh Jagtap
