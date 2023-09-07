# JARVIS-Bot-Assistant-OpenAI-Embedded-ESP32-Solutions


## Overview

JARVIS is an innovative IoT system that combines voice recognition, speech synthesis, and machine learning capabilities to create a voice-controlled smart home system. This project integrates the GPT-3.5-Turbo-16k language model from OpenAI, Microsoft Azure Speech Recognition, and various hardware components, including an ESP32 microcontroller, relay modules, and a motion sensor. With JARVIS, you can control lights, windows, and other IoT-enabled devices using natural language commands.

## Features

- **Voice-Controlled IoT**: Control your smart home devices using voice commands.
- **Real-time Speech Recognition and Synthesis**: Communicate with JARVIS using spoken language.
- **GPT-3 Integration**: Utilize OpenAI's GPT-3.5-Turbo-16k for natural language understanding and responses.
- **Hardware Control**: Manage lights and windows & custom IoT devices with ease.

## Hardware Requirements

- ESP32 microcontroller
- 2-channel relay module
- Motion sensor (PIR sensor)
- Additional Custom IoT-enabled devices (lights, windows, doors, etc.)

## Software Requirements

- Python (for GPT-3 integration)
- PlatformIO (for ESP32 firmware development)
- Firestore credentials JSON file (not included in this repository)
- GPT-3 API KEY TXT file (not included in this repository)

## Fine-Tuning Note

This project utilizes the power of the GPT-3.5-Turbo-16k model from OpenAI to provide intelligent responses to user queries and commands. It's important to note that the model used in this project was initiated with a conversation prompt to guide its behavior, but it has not undergone manual fine-tuning. Manual fine-tuning is a more in-depth process performed by OpenAI, which allows for customization of the model's behavior.

Due to subscription constraints, manual fine-tuning was not performed in this project. However, if you have an OpenAI subscription, you have the option to fine-tune the model to better suit your specific use case and requirements. Fine-tuning allows you to have more control over the model's responses and adapt it to your unique needs.

If you choose to fine-tune the model, you can achieve even more tailored and specialized results for your application. OpenAI provides detailed documentation on the fine-tuning process, enabling you to take full advantage of the model's capabilities.


## Installation and Setup

### GPT-3 Integration

1. Clone this repository to your local machine.

   ```bash
   git clone https://github.com/bnina-ayoub/J.A.R.V.I.S-Bot-Assistant.git
   cd your-project
   ```

2. Create a virtual environment (optional but recommended).

   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows, use: venv\Scripts\activate
   ```

3. Install Python dependencies.

   ```bash
   pip install -r requirements.txt
   ```

4. Obtain GPT-3 API credentials and Firestore credentials.

   - For GPT-3 API credentials, follow the OpenAI [API Reference](https://platform.openai.com/docs/api-reference)
   - For Firestore credentials, follow the Firebase [documentation](https://platform.openai.com/docs/api-reference)

5. Add the API credentials JSON files to the project directory.

6. Run those Python scripts.

   ```bash
   python Authentication.py
   python Firestore.py
   python main.py
   ```

### ESP32 Firmware

1. Open the `esp32/main.cpp` file in your PlatformIO-compatible IDE.

2. Install the required libraries specified in the `platformio.ini` file.

3. Configure your ESP32 settings in `platformio.ini`, such as board type and COM port.

4. Upload the firmware to your ESP32 device.

### ESP32 Firebase Client Library

To establish a connection between your ESP32 device and Firestore, please refer to the documentation in this [repository](https://github.com/mobizt/Firebase-ESP-Client#about-firebasedata-object). 
This resource provides comprehensive information on integrating your ESP32 with Firestore for seamless communication and data management.

## Usage

1. Power on your ESP32 device.

2. Connect the ESP32 to your local Wi-Fi network.

3. Trigger the motion sensor or send voice commands to JARVIS.

4. JARVIS will process the commands, control the relay modules, and respond using the GPT-3 language model.

## Contributors

- Bnina Ayoub [E-mail](bninayoub.pro@gmail.com)

## License

This project is licensed under the [MIT License](LICENSE.md).

## Acknowledgments

- OpenAI for the GPT-3.5-Turbo-16k model.
- Microsoft Azure for speech recognition.
- Firebase for Firestore.
- PlatformIO for embedded development.

## Support

If you encounter any issues or have questions, please open an issue on this repository or contact [your@email.com].

---

Feel free to customize the README further to include additional information specific to your project, such as troubleshooting tips, project structure, or advanced usage instructions. Make sure to keep it well-organized and user-friendly to help others understand and use your project effectively.
