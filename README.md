---

# WiFi Network Scanner with OLED Display and SD Logging

This project is a WiFi network scanner built using an ESP8266 microcontroller, which logs WiFi network data to an SD card and displays status messages on an OLED display. The primary goal is to scan the area for WiFi networks, log the detected SSIDs and related information to an SD card, and provide visual feedback on an OLED screen.

## Features

- **WiFi Scanning**: The ESP8266 scans for available WiFi networks and logs their details such as SSID, MAC address, signal strength, encryption type, channel, visibility, and the elapsed time since the scan started.
- **OLED Display**: The current status of the scan, including new SSIDs found, is displayed on an OLED screen.
- **SD Card Logging**: All scanned WiFi networks are logged into a file on an SD card for further analysis.
- **LED Feedback**: The onboard LED provides visual feedback during the scan and SD card operations.

## Hardware Required

- ESP8266 microcontroller
- OLED Display (SSD1306, 128x64 pixels)
- SD card module with SD card
- LED (optional, usually built-in on the ESP8266)
- Connecting wires

## Software Libraries

The project utilizes the following libraries:

- [SdFat](https://github.com/greiman/SdFat) for SD card operations
- [U8g2](https://github.com/olikraus/u8g2) for OLED display control
- [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi) for WiFi operations
- [ESP8266TrueRandom](https://github.com/plieningerweb/ESP8266TrueRandom) for UUID generation

## Installation

1. **Clone the Repository**:
    ```bash
    git clone https://github.com/yourusername/wifi-network-scanner.git
    cd wifi-network-scanner
    ```

2. **Install Required Libraries**:
   Ensure the required libraries are installed in your Arduino IDE:
   - SdFat
   - U8g2
   - ESP8266WiFi
   - ESP8266TrueRandom

   You can install these libraries using the Arduino Library Manager or manually.

3. **Upload the Sketch**:
   - Open the `wifi_network_scanner.ino` file in the Arduino IDE.
   - Connect your ESP8266 to your computer.
   - Select the correct board and port in the Arduino IDE.
   - Upload the sketch to the ESP8266.

## Usage

1. **Power On**: Once powered on, the device initializes the SD card and OLED display. It generates a unique file name using a UUID for each session.
2. **Scanning**: The device scans for available WiFi networks. If a new SSID is found, it logs the details to the SD card and displays a message on the OLED screen.
3. **Log File**: The data is saved in the SD card under a file with a name like `wifi_data_<UUID>.txt`. The file contains:
   ```
   Nome,MAC Address,Segnale,Encryption,Canale,Visibility,Tempo Trascorso
   ```
4. **LED Feedback**: The LED blinks during scanning and data writing operations.

## Functions Overview

- **`writeMsg(const String& message)`**: Writes a message to the SD card.
- **`generateUUID()`**: Generates a unique identifier for the session.
- **`textToScreen(String message)`**: Displays a message on the OLED screen.
- **`textToScreenNewWifi(String message)`**: Displays a message when a new SSID is found.
- **`textToScreenScanning(int &loadingAnimation)`**: Displays a scanning animation on the OLED screen.
- **`isNewNetwork(String ssid)`**: Checks if the SSID has already been logged.

## Notes

- The SD card initialization and logging process is retried each time to ensure robustness.
- The OLED display shows a simple message and scanning animation during the WiFi scan.

## Contributing

If you want to contribute to this project, feel free to fork the repository and submit a pull request with your improvements.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

Thanks to the developers of the libraries used in this project for providing the building blocks necessary to create this WiFi scanner.

---
