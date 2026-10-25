# Arduino-Based Li-Fi Text Communication System

An Arduino-based **bidirectional text communication system using visible light**. The project uses an LED to transmit text data as light pulses and a light-dependent resistor (LDR) to receive the transmitted data.

The system encodes characters into binary data, transmits them through light, and reconstructs the original message at the receiving end.

## Features

- **Light-based communication:** Transmits data using LED light pulses.
- **Text encoding:** Supports uppercase letters, lowercase letters, spaces, and digits.
- **6-bit communication protocol:** Each character is represented by a 6-bit value.
- **Configurable transmission speed:** Adjust the bit delay using a single variable.
- **Serial Monitor interface:** Send text messages directly from the Arduino IDE.
- **Progressive reception output:** Displays received characters as they arrive.
- **Low-cost hardware:** Built using Arduino boards and basic electronic components.

## Hardware Requirements

- 2 × Arduino Uno
- 1 × LED for the transmitting end
- 1 × Light-dependent resistor (LDR) for the receiving end
- 1 × LM358 operational amplifier, if used in the receiver circuit
- Resistors appropriate for the LED and receiver circuit
- Breadboards and jumper wires
- USB cables
- A computer with the Arduino IDE

> **Note:** The exact components and circuit connections may vary depending on the receiver circuit used. An LM358 is not required if the LDR signal is connected directly to the Arduino's analog input and produces sufficiently reliable readings.

## System Architecture

The communication system consists of two main parts:

### 1. Transmitter

The transmitter:
1. Reads a text message from the Serial Monitor.
2. Converts each supported character into a 6-bit value.
3. Adds delimiter sequences to identify the message boundaries.
4. Converts the binary data into LED ON/OFF pulses.
5. Transmits the message through visible light.

### 2. Receiver

The receiver:
1. Monitors light intensity using the LDR.
2. Detects the beginning of a transmission.
3. Samples the incoming light signal at fixed intervals.
4. Groups the received bits into 6-bit values.
5. Decodes the values into characters.
6. Displays the reconstructed message in the Serial Monitor.

## Communication Protocol

The project uses a custom 6-bit character encoding scheme.

| Character | Encoded Value |
|---|---:|
| Space | 0 |
| `a`–`z` | 1–26 |
| `A`–`Z` | 27–52 |
| `0`–`9` | 53–62 |
| Delimiter | 63 |

Each encoded value is transmitted from the most significant bit to the least significant bit.

The delimiter value `63`, represented by `111111` in binary, marks the boundaries of a message.

### Example

The message:

```text
hello
```

is converted into individual 6-bit values and transmitted as a sequence of light pulses.

The receiver samples these pulses, reconstructs the binary values, and decodes them back into text.

**Important:** The current encoding scheme supports letters, spaces, and digits. Punctuation and other unsupported characters are not encoded reliably.

## Configuration

The primary communication settings are defined near the beginning of the Arduino sketch.

```cpp
const int LED_PIN = 13;
const int LDR_PIN = A0;

const int LIGHT_THRESHOLD = 600;
const unsigned long BIT_DELAY = 40;
const unsigned long SECOND_INTERVAL = 1000;

const byte DELIMITER = 63;
const int MAX_MESSAGE = 100;
```

| Parameter | Description |
|---|---|
| `LED_PIN` | Digital pin connected to the transmitting LED |
| `LDR_PIN` | Analog pin connected to the receiver's LDR circuit |
| `LIGHT_THRESHOLD` | Analog reading threshold used to distinguish light from darkness |
| `BIT_DELAY` | Duration of each transmitted bit in milliseconds |
| `SECOND_INTERVAL` | Scheduling interval for transmission |
| `DELIMITER` | Special 6-bit value used to mark message boundaries |
| `MAX_MESSAGE` | Maximum message buffer size |

### Adjusting the Transmission Speed

The current bit delay is **40 milliseconds**.

```cpp
const unsigned long BIT_DELAY = 40;
```

Both communicating devices must use compatible timing settings. Changing the bit delay may require adjustments to the receiver's sampling and start-detection logic.

The light threshold should also be calibrated for the ambient lighting conditions and the distance between the LED and LDR.

## Circuit Connections

### Transmitter

| Arduino Pin | Connection |
|---|---|
| Digital Pin 13 | LED transmitter circuit |
| GND | Circuit ground |

Use an appropriate current-limiting resistor with the LED. If the LED requires more current than an Arduino GPIO pin can safely provide, use a suitable transistor driver.

### Receiver

| Arduino Pin | Connection |
|---|---|
| Analog Pin A0 | LDR sensing circuit output |
| 5V | Receiver circuit supply, as appropriate |
| GND | Receiver circuit ground |

The LDR should be connected in a suitable sensing circuit, such as a voltage divider, so that changes in light intensity produce measurable changes at A0.

**Safety and reliability:** Do not connect an LED or other component in a way that exceeds the Arduino's electrical limits. Ensure the receiver output stays within the permitted analog-input voltage range.

These connections are a general guide; follow the actual circuit schematic when assembling the hardware.

## Getting Started

### 1. Install the Arduino IDE

Download and install the Arduino IDE from the official website:

https://www.arduino.cc/en/software

### 2. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
```

Replace `YOUR_USERNAME` and `YOUR_REPOSITORY` with your GitHub username and repository name.

Alternatively, download the repository as a ZIP file and extract it.

### 3. Open the Arduino Sketch

Open the `.ino` file in the Arduino IDE.

### 4. Connect the Arduino

Connect the Arduino board to your computer using USB.

Select the appropriate board and port from the Arduino IDE.

### 5. Upload the Code

Compile and upload the sketch to the Arduino board.

For a two-board setup, upload the appropriate transmitter and receiver sketches to their respective boards. If using a combined sketch, verify that its transmit and receive logic matches your hardware arrangement.

### 6. Open the Serial Monitor

Open the Serial Monitor and configure it as follows:

- **Baud rate:** `9600`
- **Line ending:** `Newline` or `Both NL & CR`

Enter a message and press Enter to send it.

## Serial Monitor Output

The intended output format is:

```text
[send] hello
[received] hello
```

- `[send]` indicates that the message has been queued for transmission.
- `[received]` indicates the text being reconstructed by the receiver.
- Received characters are printed progressively on the same line as they arrive.

**Note:** The `[send]` message confirms that the software accepted the message for transmission. It does not confirm successful reception. A separate acknowledgement mechanism would be needed to verify delivery.

## Troubleshooting

### The Receiver Displays Incorrect Characters

- Verify that both devices use compatible bit-delay settings.
- Adjust `LIGHT_THRESHOLD` to match the ambient light conditions.
- Check the LDR wiring and receiver circuit.
- Keep the LED and LDR aligned.
- Reduce interference from other light sources.

### The Receiver Does Not Detect Messages

- Confirm that the LED is transmitting visible light pulses.
- Verify that the LDR readings change when the LED is switched ON and OFF.
- Check the analog input connection.
- Ensure the transmitter and receiver use compatible framing and sampling logic.

### Messages Are Incomplete

- Check the distance and alignment between the LED and LDR.
- Reduce ambient-light interference.
- Verify that the delimiter is detected correctly.
- Test with short messages before attempting longer ones.

### The Serial Monitor Shows No Output

- Select the correct COM port or serial device.
- Set the baud rate to `9600`.
- Select a suitable line-ending option.
- Check that the correct sketch has been uploaded.

## Current Limitations

- Communication reliability depends on ambient light, distance, alignment, and receiver sensitivity.
- The protocol supports only a limited set of characters.
- The receiver depends on accurate timing and delimiter detection.
- The current implementation does not include error correction, checksums, or confirmed message delivery.
- Reliable simultaneous full-duplex communication is not guaranteed; transmitting and receiving at the same time may require additional hardware and protocol changes.

## Future Improvements

Potential improvements include:

- [ ] Improve start-bit detection and synchronization.
- [ ] Add checksums or error detection.
- [ ] Add acknowledgement messages for delivery confirmation.
- [ ] Expand character encoding to support punctuation and additional symbols.
- [ ] Improve receiver sensitivity and ambient-light rejection.
- [ ] Add automatic bit-delay configuration.
- [ ] Optimize communication for longer messages.
- [ ] Improve bidirectional communication and collision handling.
- [ ] Develop a more robust physical-layer protocol.

## Applications

Potential applications of the project include:

- Short-range optical communication demonstrations
- Educational experiments involving visible light communication
- Arduino-based communication systems
- Learning about binary encoding, digital signaling, and optical data transmission

## Contributing

Contributions, suggestions, and improvements are welcome.

1. Fork the repository.
2. Create a branch for your changes.
3. Commit your changes.
4. Open a pull request describing your improvements.

For major changes, consider opening an issue first to discuss the proposed design.

## License

This project is available under the MIT License. Add a `LICENSE` file to the repository containing the MIT License text if you intend to distribute the project under that license.

## Acknowledgements

Built using Arduino hardware and the principles of visible light communication (VLC).

---

**Project status:** Experimental prototype under development.
