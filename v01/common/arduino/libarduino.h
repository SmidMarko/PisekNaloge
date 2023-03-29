#ifndef LIBARDUINO
#define LIBARDUINO

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <string>

// *** Constants ***
// Pin modes
#define INPUT 1
#define OUTPUT 2
#define INPUT_PULLUP 3

// Values for digital write
#define HIGH 2
#define LOW 1

// Analog pins
#define A0 -1
#define A1 -2
#define A2 -3
#define A3 -4
#define A4 -5
#define A5 -6

#define LED_BUILTIN 13

using namespace std;

// Global variables
int __currentPinModes[14] = {};
int __currentPinValues[14] = {};
int __currentAnalogPinValues[6] = {};

string __inputBuffer = "";


void fillInputBuffer() {
  // (Re)fill the inputBuffer if needed
  if(__inputBuffer.length() > 64 || feof(stdin)) { return; }

  char readBuffer[64];
  if(fgets(readBuffer, 64, stdin) != NULL) {
    __inputBuffer.append(readBuffer);
  }
}


void pinMode(int pin, int mode) {
  // Mode should be one of the pin modes defined above
  if(mode < 1 || mode > 3) { return; }
  if(__currentPinModes[pin] != mode) {
    __currentPinModes[pin] = mode;
    printf("S %d %d\n", pin, mode);
  }
}

void digitalWrite(int pin, int value) {
  if(value < 1 || value > 2) { return; }
  if(__currentPinValues[pin] != value) {
    __currentPinValues[pin] = value;
    printf("P %d %d\n", pin, value);
  }
}

int digitalRead(int pin) {
  if(pin >= 0) {
    return __currentPinValues[pin];
  } else {
    if(__currentAnalogPinValues[1-pin] > 127) {
      return HIGH;
    } else {
      return LOW;
    }
  }
}

void delay(int ms) {
  printf("W %d\n", ms);
}

void tone(int pin, unsigned int frequency) {
  printf("T %d %u\n", pin, frequency);
}

void noTone(int pin) {
  printf("N %d\n", pin);
}

int analogRead(int pin) {
  return __currentAnalogPinValues[pin];
}

class SerialClass {
  public:
    static void begin(int n);
    static int available();
    static char read();
    static void write(char c);
    static long print(char* txt);
    static long print(char const* txt);
    static long print(int n);
    static long println(char* txt);
    static long println(char const* txt);
    static long println(int n);
    static long println();
};

void SerialClass::begin(int n) {
  // TODO
  return;
}

int SerialClass::available() {
  // The Arduino serial port buffer can only hold 64 bytes, we emulate that by
  // returning an available fo maximum 64 bytes
  fillInputBuffer();
  return min(static_cast<int>(__inputBuffer.length()), 64);
}

char SerialClass::read() {
  fillInputBuffer();
  if(__inputBuffer.length() > 0) {
    char c = __inputBuffer[0];
    __inputBuffer.erase(0, 1);
    return c;
  } else {
    return -1;
  }
}

void SerialClass::write(char c) {
  printf("O `%c`\n", c);
}

long SerialClass::print(char* txt) {
  printf("O `%s`\n", txt);
  return static_cast<long>(strlen(txt));
}

long SerialClass::print(char const* txt) {
  return SerialClass::print(strdup(txt));
}

long SerialClass::print(int n) {
  printf("O `%d`\n", n);
  if(n > 0) {
    return static_cast<long>(floor(log10(n)))+1;
  } else if(n < 0) {
    return static_cast<long>(floor(log10(-n)))+1;
  } else {
    return 1;
  }
}

long SerialClass::println(char* txt) {
  printf("O `%s\n`\n", txt);
  return static_cast<long>(strlen(txt))+1;
}

long SerialClass::println(char const* txt) {
  return SerialClass::println(strdup(txt));
}

long SerialClass::println(int n) {
  printf("O `%d\n`\n", n);
  if(n > 0) {
    return static_cast<long>(floor(log10(n)))+2;
  } else if(n < 0) {
    return static_cast<long>(floor(log10(-n)))+2;
  } else {
    return 2;
  }
};

long SerialClass::println() {
  printf("O `\n`\n");
  return 1;
}

SerialClass Serial;


#endif
