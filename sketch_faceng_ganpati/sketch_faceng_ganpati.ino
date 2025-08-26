#include <Arduino.h>
#include <WiFiEspAT.h>
#include "FacengGanpatiPinConstants.h"
#include "FacengGanpatiUtilities.h"
#include "FacengGanpatiWiFiUtils.h"
#include "FacengGanpatiModulesControl.h"
#include "FacengGanpatiSevenSegDisplay.h"

/*****************************************************************************************/
//
// Globals for mode selection
//
/*****************************************************************************************/
#define NOOP_MODULE 0xFF
#define BTN_STATE_UNKNOWN 0xFF
uint8_t currentModule = NOOP_MODULE;
volatile uint8_t lastModule = NOOP_MODULE;
volatile uint8_t selectedModule = NOOP_MODULE;
volatile unsigned long lastInterruptTime = 0;
const unsigned long intrDebounceDelay = 100; // milliseconds
// Interrupt pin map for Megsa 2560
// pin	  INTx	digitalPinToInterrupt(pin)
// 3  	  INT5	  1
// 2  	  INT4    0
// 18 TX1	INT3	  5
// 19 RX1	INT2	  4
// 20 SDA	INT1	  3
// 21 SCL	INT0	  2

/*****************************************************************************************/
//
// Globals miscellaneous
//
/*****************************************************************************************/
const unsigned int baudRateBluetoothHC05ATMode = 38400;
const unsigned int baudRate = 9600;
const unsigned long pushButtonDebounceDelayMs = 100;
uint8_t prevBellPinState = HIGH;
uint8_t prevSpinWheelPinState = HIGH;
uint8_t prevAutomationPinState = HIGH;
bool wifiSetupSuccessful = false;

/*****************************************************************************************/
//
// Globals helper functions
//
/*****************************************************************************************/
void handleModuleIntr() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > intrDebounceDelay) {
    lastModule = selectedModule;
    selectedModule = (++selectedModule % Modules::MODULES_COUNT);
    // User can't interface directly with Music module, so skip that
    if (selectedModule == Modules::Music) {
      selectedModule = (++selectedModule % Modules::MODULES_COUNT);
    }
    lastInterruptTime = currentTime; // Update last interrupt time
  }
}

int debouncedDigitalRead(uint8_t bellBtnPin, unsigned long debounceDelayMs) {
  int toggle = digitalRead(bellBtnPin);
  int toggleNext = 0;
  int startTime = millis();

  do {
    toggleNext = digitalRead(bellBtnPin);
    if (toggleNext != toggle) {
      startTime = millis();
      toggle = toggleNext;
    }
  } while (millis() - startTime < debounceDelayMs);

  return toggle;
}

void setup() {
  // Default Serial
  Serial.begin(baudRate);

  // // Serial with Bluetooth Module
  // Serial1.begin(baudRate);
  // Serial with ESP8266
  ESP_SERIAL.begin(115200);

  // Setup interrupt for mode-switch button
  pinMode(intrPin, INPUT_PULLUP);

  // Attach the ISR for mode-switch
  attachInterrupt(digitalPinToInterrupt(intrPin), handleModuleIntr, LOW);

  // Setup 7-seg display o/p pins
  for (int i = 0; i < numSegmentsForDigits; i++) {
    pinMode(arduinoPinsFor7segDisp[i], OUTPUT);
  }

  // Setup relay o/p pins
  for (int i = 0; i < numRelayControlPins; i++) {
    // Since relay input is active low, by default
    // our pins should be high. Hence first set to INPUT_PULLUP and then to OUTPUT
    // This ensures that default output remains HIGH
    pinMode(relayControlPins[i], INPUT_PULLUP);
    pinMode(relayControlPins[i], OUTPUT);
  }

  // Setup limit switch and module start/stop i/p pins
  for (int i = 0; i < numInputControlPins; i++) {
    pinMode(inputControlPins[i], INPUT_PULLUP);
  }

  DEBUG_PRINT("Current module %d\n", currentModule);
  writeNumberTo7SegDisplay(currentModule);

  DEBUG_PRINT("Starting Wi-Fi setup on ESP8266...\n");
  if (wifiInit()) {
    DEBUG_PRINT("Wi-Fi setup complete. Ready to send commands.\n");
    wifiSetupSuccessful = true;
    digitalWrite(wifiOkLEDPin, HIGH);
  } else {
    DEBUG_PRINT("Wi-Fi setup failed.\n");
    digitalWrite(wifiOkLEDPin, LOW);
  }

  // Turn on Boot-OK LED
  digitalWrite(bootedOkLEDPin, HIGH);
  // Show on 7-seg display that we are ready to start
  writeNumberTo7SegDisplay(10);
  DEBUG_PRINT("Chip booted and setup done\n");
}


// Put your main code here, to run repeatedly:
void loop() {
  // Track if we need to switch off prev module
  bool moduleSwitched = (selectedModule != currentModule);
  if (moduleSwitched) {
    // If we are moving in to or out of Automation, stop any other running Modules
    if (selectedModule == Modules::Automation || currentModule == Modules::Automation) {
      stopAllPhysicalModules();
      // If we are moving out of automation, change its state
      if (currentModule != selectedModule) {
        ModuleAutomationState.state = false;
        prevAutomationPinState = HIGH;
      }
    } else {
      // Stop the last module that the loop() knew before the switch
      stopModule(static_cast<Modules>(currentModule));
    }
    DEBUG_PRINT("Module switched to %u\n", selectedModule);

    // Now make the user-selected module as the current module that the loop() sees.
    currentModule = selectedModule;
    writeNumberTo7SegDisplay(currentModule);
  }

  if (currentModule == NOOP_MODULE)
    return;

  //
  // TODO: Add code to stop last module if module switch occurs
  //
  switch (currentModule) {
    case Modules::Bell: {
      //
      // Bell module toggled on-off if Module::Bell is selected as current module and pushbutton is active
      // The prevBellPinState helps track long human time scale presses in fast loop() iterations.
      // This allows implementing sticky behavior of Module state.
      // NOTE: Pins Active Low due to INPUT_PULLUP
      //
      int currBellPinState = debouncedDigitalRead(startModuleControlPin, pushButtonDebounceDelayMs);
      bool startBell = (LOW == currBellPinState && HIGH == prevBellPinState) && !ModuleBellState.state;
      bool stopBell  = (LOW == currBellPinState && HIGH == prevBellPinState) && ModuleBellState.state;
      if (startBell) {
        startModule(Modules::Bell);
      } else if (stopBell) {
        stopModule(Modules::Bell);
      }
      prevBellPinState = currBellPinState;
      if (startBell || stopBell)
        DEBUG_PRINT("Btn push recorded: CurrentModule %u, currPinState %d, ModuleStateChangedTo %u \n",currentModule, currBellPinState, ModuleBellState.state);
      break;
    }
    case Modules::SpinWheel: {
      //
      // Spinwheel module toggled on-off if Module::SpinWheel is selected as current module and pushbutton is active
      // The prevSpinWheelPinState helps track long human time scale presses in fast loop() iterations.
      // This allows implementing sticky behavior of Module state.
      // NOTE: Pins are Active Low due to INPUT_PULLUP
      //
      int currSpinWheelPinState = debouncedDigitalRead(startModuleControlPin, pushButtonDebounceDelayMs);
      bool startSpinWheel = (LOW == currSpinWheelPinState && HIGH == prevSpinWheelPinState) && !ModuleSpinWheelState.state;
      bool stopSpinWheel  = (LOW == currSpinWheelPinState && HIGH == prevSpinWheelPinState) && ModuleSpinWheelState.state;
      if (startSpinWheel) {
        startModule(Modules::SpinWheel);
      } else if (stopSpinWheel) {
        stopModule(Modules::SpinWheel);
      }
      prevSpinWheelPinState = currSpinWheelPinState;
      if (startSpinWheel || stopSpinWheel)
        DEBUG_PRINT("Btn push recorded: CurrentModule %u, currPinState %d, ModuleStateChangedTo %u \n", currentModule, currSpinWheelPinState, ModuleSpinWheelState.state);
      break;
    }
    case Modules::Cart: {
      //
      // Cart module can be in one of 3 states { forward, reverse, stopped }.
      // When it module is selected:
      //   The @ref startModuleControlPin will move the cart forward as long as it is pressed.
      //   Releasing will stop the cart.
      //   The @ref stopModuleControlPin will move the cart reversed as long as it is pressed.
      //   Releasing will stop the cart.
      // Note: Input pins are ACTIVE LOW due to INPUT_PULLUP. Output pins are ACTIVE HIGH
      //
      int currCartForwardPinState = debouncedDigitalRead(startModuleControlPin, pushButtonDebounceDelayMs);
      int currCartReversePinState = debouncedDigitalRead(stopModuleControlPin, pushButtonDebounceDelayMs);
      int limitSwitchPinState     = debouncedDigitalRead(cartLimitSwitchInputPin, pushButtonDebounceDelayMs);

      // User triggered, when not constrained by limit switch
      bool startMovingForward = (ModuleCartState.state == ModuleCartState.Stopped) &&
                                (LOW == currCartForwardPinState) &&
                                (HIGH == limitSwitchPinState);
      // Either user triggered or triggered by limit switch
      bool stopMovingForward  = (ModuleCartState.state == ModuleCartState.Forward) &&
                                ((HIGH == currCartForwardPinState) || (LOW == limitSwitchPinState));
      // No limit switch here per design for reverse
      bool startMovingReverse = (ModuleCartState.state == ModuleCartState.Stopped) &&
                                (LOW == currCartReversePinState);
      // No limit switch here per design for reverse
      bool stopMovingReverse  = (ModuleCartState.state == ModuleCartState.Reverse) &&
                                (HIGH == currCartReversePinState);

      if (startMovingForward) {
        startModule(Modules::Cart, true);
      } else if (startMovingReverse) {
        startModule(Modules::Cart, false);
      } else if (stopMovingForward || stopMovingReverse) {
        stopModule(Modules::Cart);
      }

      break;
    }
    case Modules::Balloon: {
      int currBalloonInflatePinState = debouncedDigitalRead(startModuleControlPin, pushButtonDebounceDelayMs);
      int currBalloonDeflatePinState = debouncedDigitalRead(stopModuleControlPin, pushButtonDebounceDelayMs);
      int limitSwitchPinState = debouncedDigitalRead(balloonTopLimitSwitchInputPin, pushButtonDebounceDelayMs);

      // User triggered, when not constrained by limit switch
      bool startInflation = (ModuleBalloonState.state == ModuleBalloonState.PressureHold) &&
                            (LOW == currBalloonInflatePinState) &&
                            (HIGH == limitSwitchPinState);
      // User triggered or by limit switch
      bool stopInflation  = (ModuleBalloonState.state == ModuleBalloonState.Inflate) &&
                            ((HIGH == currBalloonInflatePinState) || (LOW == limitSwitchPinState));
      // No limit switch here per design for reverse
      bool startDeflation = (ModuleBalloonState.state == ModuleBalloonState.PressureHold) &&
                            (LOW == currBalloonDeflatePinState);
      // No limit switch here per design for reverse
      bool stopDeflation  = (ModuleBalloonState.state == ModuleBalloonState.Deflate) &&
                            (HIGH == currBalloonDeflatePinState);

      if (startInflation) {
        startModule(Modules::Balloon, true);
      } else if (startDeflation) {
        startModule(Modules::Balloon, false);
      } else if (stopInflation || stopDeflation) {
        stopModule(Modules::Balloon);
      }
      break;
    }
    case Modules::Automation: {
     /*
      *.  The prevAutomationPinState helps us continue the Automation sequence in back to back loop() calls.
      *   InputPin  AutomationState Action
      *   0         False/Off       Turn-on
      *   1         True/On         Continue turn-on sequence
      *   0         True/On         Turn-off
      *   1         False/Off       Do nothing
      *
      */
      int  currAutomationPinState = debouncedDigitalRead(startModuleControlPin, pushButtonDebounceDelayMs);
      bool turnOnSequence   = (LOW == currAutomationPinState && HIGH == prevAutomationPinState && !ModuleAutomationState.state);
      bool continueSequence = (HIGH == currAutomationPinState && ModuleAutomationState.state);
      bool turnOffSequence  = (LOW == currAutomationPinState && HIGH == prevAutomationPinState && ModuleAutomationState.state);
      bool doNothing        = (HIGH == currAutomationPinState && !ModuleAutomationState.state);

      if (turnOnSequence || continueSequence) {
        if (turnOnSequence) {
          // Ensure all modules are off
          stopAllPhysicalModules();
          DEBUG_PRINT("AutomationState before %u CurrPinState %u", ModuleAutomationState.state, currAutomationPinState);
          ModuleAutomationState.state = true;
          DEBUG_PRINT("Starting modules in automation\n");
        }

        // 0. Bell
        if (!ModuleBellState.state) {
          startModule(Modules::Bell);
          DEBUG_PRINT("Bell module started. Current state of module %u\n", ModuleBellState.state);
        }

        // 1. Balloon
        bool isLimitSwitchBalloonHit = (LOW == debouncedDigitalRead(balloonTopLimitSwitchInputPin, pushButtonDebounceDelayMs));
        if (ModuleBalloonState.state == ModuleBalloonState.PressureHold && !isLimitSwitchBalloonHit) {
          startModule(Modules::Balloon, true);
          DEBUG_PRINT("Balloon module started. Current state of module %u\n", ModuleBalloonState.state);
        }
        // We can proceed only if balloon has hit limit switch
        if (isLimitSwitchBalloonHit) {
          // Stop the balloon inflation, i.e. hold its pressure
          if (ModuleBalloonState.state != ModuleBalloonState.PressureHold) {
            stopModule(Modules::Balloon);
          }

          // 2. SpinWheel
          if (!ModuleSpinWheelState.state) {
            startModule(Modules::SpinWheel);
            DEBUG_PRINT("Spinwheel module started. Current state of module %u\n", ModuleSpinWheelState.state);
          }

          // 3. Cart
          bool isLimitSwitchCartHit = (LOW == debouncedDigitalRead(cartLimitSwitchInputPin, pushButtonDebounceDelayMs));
          if (ModuleCartState.state == ModuleCartState.Stopped && !isLimitSwitchCartHit) {
            startModule(Modules::Cart, true);
            DEBUG_PRINT("Cart module started. Current state of module %u\n", ModuleCartState.state);
          }
          // If limit switch is hit, cart is in position. Start music then.
          if (isLimitSwitchCartHit) {
            // Stop cart
            if (ModuleCartState.state != ModuleCartState.Stopped) {
              stopModule(Modules::Cart);
            }

            // 4. Start Music
            if (!ModuleMusicState.state) {
              startModule(Modules::Music);
              DEBUG_PRINT("Music module started. Current state of module %u\n", ModuleMusicState.state);
            }
          }
        }
      } else if (turnOffSequence) {
        DEBUG_PRINT("AutomationState before %u CurrPinState %u\n", ModuleAutomationState.state, currAutomationPinState);
        DEBUG_PRINT("Stoping modules in automation\n");
        ModuleAutomationState.state = false;
        stopAllPhysicalModules();
        DEBUG_PRINT("Automation stopped\n");
      } else {
        // Do nothing
      }
      prevAutomationPinState = currAutomationPinState;
      break;
    }
    default:
      break;
  }
}