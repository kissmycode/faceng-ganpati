# FacEng Ganpati - Interactive Automation Controller

This project is an Arduino-based controller for an interactive Ganpati festival decoration. It manages several modules including a bell, a spinning wheel, a moving cart, and an inflating balloon. It features both manual control for each module and a fully automated sequence that runs all modules in a synchronized order.

The controller can communicate via **Wi-Fi (UDP)** to a companion Python script running on a macOS computer to play a devotional song (Aarti) from the Apple Music app.

## ✨ Features

* **Modular Control:** Manually select and operate individual modules.
* [cite_start]**7-Segment Display:** Shows the currently selected module number for clear user feedback[cite: 5].
* [cite_start]**Automation Mode:** A one-touch automated sequence that controls all physical modules in a predefined order[cite: 2].
* [cite_start]**Communication:** Uses an ESP8266 for Wi-Fi (UDP) commands to a remote computer[cite: 2, 3].
* **Remote Music Control:** Triggers playback of a specific track in Apple Music on a Mac.
* [cite_start]**Debounced Inputs:** All user inputs (buttons and limit switches) are debounced for reliable operation[cite: 2].

## ⚙️ Hardware Requirements

* **Microcontroller:** An Arduino Mega 2560 (or a compatible board with enough I/O pins).
* **Communication Module:**
    * [cite_start]**For Wi-Fi:** An ESP8266 module (like ESP-01) connected to `Serial3`[cite: 2].
* [cite_start]**Display:** One 7-segment display[cite: 5].
* **Actuators & Relays:** Relay modules to control the bell, spinning wheel motor, cart motor, and balloon pump/valve.
* [cite_start]**User Input:** 3 x Push Buttons (Mode Select, Start/Forward, Stop/Reverse)[cite: 1].
* [cite_start]**Sensors:** 2 x Limit Switches (for cart and balloon)[cite: 1].
* **Computer:** A Mac running macOS to execute the companion script for music playback.

## 📚 Software Dependencies

### Arduino
* **Arduino IDE:** Version 1.8.x or newer.
* **WiFiEspAT Library:** Required for Wi-Fi communication. Install from the Arduino IDE's Library Manager.

### Python (for macOS Companion Scripts)
* **Python 3**
* **appscript:** For controlling Apple Music.

## 🖥️ Companion Scripts (macOS Music Control)

The project includes a Python script to receive commands from the Arduino and control Apple Music on a Mac.

* [cite_start]`wifi-reader.py`: Listens for UDP packets over Wi-Fi on port `8888`[cite: 4]. This is the default method used by the automation sequence.

[cite_start]The script listens for the commands `play_aarti` and `stop_aarti` to control a specific track in the Music app[cite: 3, 4].

## 🚀 Installation & Running

### Arduino Setup

1.  **Wiring:** Connect all components to the Arduino Mega according to `FacengGanpatiPinConstants.h`.
2.  **Wi-Fi Credentials Setup:** This project uses a separate, untracked file to keep your Wi-Fi credentials secure.
    * Create a new file named `credentials.h` by copying the `credentials.h.example` template.
    * Open your new `credentials.h` file and replace the placeholder values with your actual Wi-Fi SSID and password.
    * The `credentials.h` file is intentionally ignored by Git (via `.gitignore`) and should never be committed.
3.  [cite_start]**Network Target:** In `FacengGanpatiWiFiUtils.cpp`, update the target IP address to match the IP of the Mac running the `wifi-reader.py` script[cite: 6].
    ```cpp
    const char* macbook_ip = "192.168.86.249"; // <-- CHANGE THIS
    ```
4.  **Upload:** Upload the `sketch_faceng_ganpati.ino` sketch to your Arduino.

### Python Script Setup (on macOS)

1.  **Install Dependencies:** Open a terminal, navigate to your project directory, and install the required Python libraries using the `requirements.txt` file:
    ```shell
    pip install -r requirements.txt
    ```
2.  **Configure the Script:**
    * [cite_start]**Apple Music:** Ensure you have a track named exactly **"Sukhkarta_aarti"** in your Apple Music library, as this name is hardcoded in both Python scripts[cite: 3, 4].
3.  **Run the Script:**
    * In your terminal, run the script that matches your hardware setup.
    * **For Wi-Fi:**
        ```shell
        python3 wifi-reader.py
        ```
    * [cite_start]The script will print a confirmation message once it's running and connected[cite: 3, 4]. Keep the terminal window open while you use the Arduino controller.

## 🕹️ How to Use

1.  [cite_start]**Power On:** The system will boot, connect to Wi-Fi (if configured), and display `A` on the 7-segment display to indicate it's ready[cite: 2].
2.  **Start Companion Script:** Run the Wi-Fi script on your Mac.
3.  [cite_start]**Select a Module:** Press the "Mode Select" button (connected to pin 2) to cycle through the modules: `0` (Bell), `1` (Spin Wheel), `2` (Cart), `3` (Balloon), `4` (Automation)[cite: 2].
4.  **Control the Module:** Use the "Start" and "Stop" buttons to operate the selected module. [cite_start]When **Automation** is selected and started, the Arduino will send the `"play_aarti"` command at the appropriate step in the sequence[cite: 2].