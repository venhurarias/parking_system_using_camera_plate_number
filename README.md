# Smart Parking and Vehicle Access System

This project is a recovered version of a smart parking and vehicle access system that combines license plate recognition, parking slot monitoring, and hardware-based security features. Since this file was recovered, it is **not the complete or latest version** of the project. Some parts may be outdated, incomplete, or no longer reflect the final implementation. The latest file is currently not available for review.

## Project Overview

The system was designed to automate vehicle entry and parking management through a combination of computer vision and embedded hardware. On the software side, a Python desktop application uses a camera feed and a license plate recognition engine to detect vehicle plate numbers in real time. On the hardware side, an Arduino-based controller manages parking slot sensors, LED indicators, RFID functions, and fingerprint authentication.

The recovered code shows the core structure of the system, including:
- real-time camera display using Tkinter and OpenCV
- license plate recognition using `simplelpr`
- local tenant/plate storage using SQLite
- serial communication setup between Python and Arduino
- parking slot monitoring using multiple sensors
- NeoPixel LED status indicators for parking slots
- fingerprint and RFID support for additional access control

## Important Note About This Recovered File

This repository contains a **recovered file only**. It should be treated as a partial snapshot of the project and not as the final or most updated source code.

Please note the following:
- this is **not the complete version**
- the **latest version is currently unavailable**
- some intended features may be missing or only partially implemented
- some code paths may reflect earlier development stages

Based on the recovered code and project behavior, the **latest version most likely used Arduino communication for fingerprint verification as an extra layer of security**. In other words, beyond license plate recognition, the newer implementation likely required fingerprint-based confirmation through the Arduino system before allowing or finalizing access.

## Python Application

The Python application provides the main user interface and camera processing logic. It:
- opens a fullscreen Tkinter window
- captures live video from the default camera
- displays the live feed on screen
- analyzes frames periodically for license plate detection
- shows recognized plate numbers on the interface
- initializes a local SQLite database for tenant records
- prepares serial communication with an Arduino device

The plate recognition logic uses repeated detection checks before accepting a plate result, which helps reduce false positives.

## Arduino Hardware Controller

The Arduino code acts as the hardware controller for the parking and access system. It includes:
- parking slot state detection using multiple digital sensors
- slot availability and assignment tracking
- LED visualization using a NeoPixel strip
- fingerprint enrollment and verification
- RFID reading and writing
- serial communication with the main application

The parking slots are managed using status values such as available, assigned, and occupied. LED colors are used to visually represent these states.

## Security Features

The recovered version already includes fingerprint and RFID modules in the Arduino code. However, based on the available notes and structure, the **latest implementation appears to have relied on fingerprint communication through Arduino for additional security**. This suggests that the final system may have required biometric confirmation in addition to plate recognition, especially for secure vehicle access or slot assignment validation.

## Database

The Python application uses SQLite to store tenant information. In the recovered version, the database includes a `tenant` table containing sample records with names and plate numbers. This indicates that recognized plates were intended to be matched against authorized vehicle records.

## Current Limitations

Because this is only a recovered version of the code:
- the full end-to-end logic may not be present
- the latest security flow cannot be confirmed completely
- some integrations between Python and Arduino may be unfinished in this copy
- the most recent communication protocol is not fully visible

## Summary

This recovered project represents a smart parking and vehicle access prototype that integrates:
- license plate recognition
- parking slot monitoring
- local vehicle record storage
- serial communication with Arduino
- fingerprint-based extra security
- RFID support

Although incomplete, the recovered code still shows the intended architecture of a multi-layer vehicle access and parking management system.


## Wiring Diagram

## Images
