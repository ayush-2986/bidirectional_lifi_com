const int LED_PIN = 13;
const int LDR_PIN = A0;

const int LIGHT_THRESHOLD = 600;
const unsigned long BIT_DELAY = 20;
const unsigned long SECOND_INTERVAL = 1000;

const byte DELIMITER = 63;
const int MAX_MESSAGE = 100;

char txMessage[MAX_MESSAGE];
char rxMessage[MAX_MESSAGE];

int txLength = 0;
int rxLength = 0;

bool txPending = false;
bool transmitting = false;
bool receiving = false;

unsigned long nextTxTime = 0;
unsigned long txStartTime = 0;
unsigned long rxStartTime = 0;
unsigned long lastSampleTime = 0;

int txBitIndex = 0;
int txFrameLength = 0;

byte txFrame[MAX_MESSAGE * 6 + 12];

byte rxBits[6];
int rxBitCount = 0;

bool lastLightState = false;
unsigned long lightStartTime = 0;

byte encodeChar(char c) {
  if (c == ' ') return 0;
  if (c >= 'a' && c <= 'z') return c - 'a' + 1;
  if (c >= 'A' && c <= 'Z') return c - 'A' + 27;
  if (c >= '0' && c <= '9') return c - '0' + 53;
  return 0;
}

char decodeChar(byte value) {
  if (value == 0) return ' ';
  if (value >= 1 && value <= 26) return 'a' + value - 1;
  if (value >= 27 && value <= 52) return 'A' + value - 27;
  if (value >= 53 && value <= 62) return '0' + value - 53;
  return '\0';
}

void addBits(byte value) {
  for (int i = 5; i >= 0; i--) {
    txFrame[txFrameLength++] = (value >> i) & 1;
  }
}

void queueMessage(const char *message) {
  txLength = 0;

  while (message[txLength] != '\0' && txLength < MAX_MESSAGE - 1) {
    txMessage[txLength] = message[txLength];
    txLength++;
  }

  txMessage[txLength] = '\0';

  if (txLength == 0) return;

  txFrameLength = 0;

  addBits(DELIMITER);

  for (int i = 0; i < txLength; i++) {
    addBits(encodeChar(txMessage[i]));
  }

  addBits(DELIMITER);

  txBitIndex = 0;
  txPending = true;

  Serial.print("[send] ");
  Serial.println(txMessage);
}

void startTransmission() {
  transmitting = true;
  txPending = false;
  txStartTime = millis();
  txBitIndex = 0;
}

void updateTransmission() {
  if (txPending && !transmitting && !receiving) {
    unsigned long now = millis();

    if (now >= nextTxTime) {
      startTransmission();
    }
  }

  if (!transmitting) return;

  unsigned long now = millis();

  if (now - txStartTime < BIT_DELAY) return;

  txStartTime = now;

  if (txBitIndex < txFrameLength) {
    digitalWrite(LED_PIN, txFrame[txBitIndex] ? HIGH : LOW);
    txBitIndex++;
  } else {
    digitalWrite(LED_PIN, LOW);
    transmitting = false;
    nextTxTime = ((millis() / SECOND_INTERVAL) + 1) * SECOND_INTERVAL;
  }
}

bool readLight() {
  return analogRead(LDR_PIN) > LIGHT_THRESHOLD;
}

void resetReceiver() {
  receiving = false;
  rxLength = 0;
  rxBitCount = 0;
  lastLightState = false;
  lightStartTime = 0;
}

void finishMessage() {
  if (rxLength > 0) {
    rxMessage[rxLength] = '\0';
    Serial.println();
  }

  resetReceiver();
}

void processBitGroup() {
  byte value = 0;

  for (int i = 0; i < 6; i++) {
    value = (value << 1) | rxBits[i];
  }

  if (value == DELIMITER) {
    if (rxLength > 0) {
      finishMessage();
    } else {
      rxLength = 0;
      rxBitCount = 0;
    }

    return;
  }

  char decoded = decodeChar(value);

  if (decoded != '\0' && rxLength < MAX_MESSAGE - 1) {
    if (rxLength == 0) {
      Serial.print("[received] ");
    }

    rxMessage[rxLength++] = decoded;
    Serial.print(decoded);
  }

  rxBitCount = 0;
}

void updateReceiver() {
  if (transmitting) return;

  unsigned long now = millis();
  bool lightState = readLight();

  if (!receiving) {
    if (lightState) {
      if (!lastLightState) {
        lightStartTime = now;
      }

      if (now - lightStartTime >= BIT_DELAY * 11 / 2) {
        receiving = true;
        rxLength = 0;
        rxBitCount = 0;
        rxStartTime = now;
        lastSampleTime = now;
      }
    } else {
      lightStartTime = 0;
    }

    lastLightState = lightState;
    return;
  }

  if (now - lastSampleTime >= BIT_DELAY) {
    lastSampleTime += BIT_DELAY;

    rxBits[rxBitCount++] = lightState ? 1 : 0;

    if (rxBitCount == 6) {
      processBitGroup();

      if (!receiving) return;
    }
  }

  if (now - rxStartTime > BIT_DELAY * (MAX_MESSAGE * 6 + 20)) {
    finishMessage();
  }
}

void readSerialInput() {
  static char inputBuffer[MAX_MESSAGE];
  static int inputLength = 0;

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputLength > 0) {
        inputBuffer[inputLength] = '\0';
        queueMessage(inputBuffer);
        inputLength = 0;
      }
    } else if (inputLength < MAX_MESSAGE - 1) {
      inputBuffer[inputLength++] = c;
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  nextTxTime = ((millis() / SECOND_INTERVAL) + 1) * SECOND_INTERVAL;
}

void loop() {
  readSerialInput();
  updateTransmission();
  updateReceiver();
}
