#include <Arduino.h>
#include "FacengGanpatiPinConstants.h"
#include "FacengGanpatiUtilities.h"
#include "FacengGanpatiWiFiUtils.h"
#include "FacengGanpatiModulesControl.h"

// Definitions for extern'd global variables
_ModuleBellState ModuleBellState;
_ModuleSpinWheelState ModuleSpinWheelState;
_ModuleCartState ModuleCartState;
_ModuleBalloonState ModuleBalloonState;
_ModuleMusicState ModuleMusicState;
_ModuleAutomationState ModuleAutomationState;
_ModuleColumnLEDsState ModuleColumnLEDsState;

void startModule(Modules mod, bool moveInPositiveDir) {
  // All relay pins are active low.
  switch (mod) {
    case Modules::Bell: {
      DEBUG_PRINT("Bell module started\n");
      ModuleBellState.state = true;
      digitalWrite(bellRelayControlPin, LOW);
      break;
    }
    case Modules::SpinWheel: {
      DEBUG_PRINT("SpinWheel module started\n");
      ModuleSpinWheelState.state = true;
      digitalWrite(spinWheenRelayControlPin, LOW);
      break;
    }
    case Modules::Cart: {
      DEBUG_PRINT("Cart module started");
      if (moveInPositiveDir) {
        ModuleCartState.state = ModuleCartState.Forward;
        digitalWrite(cartForwardRelayControlPin, LOW);
        DEBUG_PRINT(" foward\n");
      } else {
        ModuleCartState.state = ModuleCartState.Reverse;
        digitalWrite(cartReverseRelayControlPin, LOW);
        DEBUG_PRINT(" reverse\n");
      }
      break;
    }
    case Modules::Balloon: {
      DEBUG_PRINT("Balloon module started");
      if (moveInPositiveDir) {
        ModuleBalloonState.state = ModuleBalloonState.Inflate;
        digitalWrite(balloonInflateRelayControlPin, LOW);
        DEBUG_PRINT(" inflation\n");
      } else {
        ModuleBalloonState.state = ModuleBalloonState.Deflate;
        digitalWrite(balloonDeflateRelayControlPin, LOW);
        DEBUG_PRINT(" deflation\n");
      }
      break;
    }
    case Modules::Music: {
      if (!ModuleMusicState.state) {
        ModuleMusicState.state = true;
        playAarti();
        DEBUG_PRINT("Music module started\n");
      } else {
        DEBUG_PRINT("Music module was already started. Nothing to do.\n");
      }
      break;
    }
    case Modules::Automation: {
      break;
    }
    case Modules::ColumnLEDs:
    {
      ModuleColumnLEDsState.state = true;
      digitalWrite(templeLightsLEDRelayControlPin, LOW);
      DEBUG_PRINT("ColumnLEDs module started\n");
      break;
    }
    default: {
      DEBUG_PRINT("No matching module number %u to start\n", mod);
      break;
    }
  }
}

void stopModule(Modules mod) {
  // All relay pins are active low.
  switch (mod) {
    case Modules::Bell: {
      ModuleBellState.state = false;
      digitalWrite(bellRelayControlPin, HIGH);
      DEBUG_PRINT("Bell module stopped\n");
      break;
    }
    case Modules::SpinWheel: {
      ModuleSpinWheelState.state = false;
      digitalWrite(spinWheenRelayControlPin, HIGH);
      DEBUG_PRINT("SpinWheel module stopped\n");
      break;
    }
    case Modules::Cart: {
      ModuleCartState.state = ModuleCartState.Stopped;
      digitalWrite(cartForwardRelayControlPin, HIGH);
      digitalWrite(cartReverseRelayControlPin, HIGH);
      DEBUG_PRINT("Cart module stopped\n");
      break;
    }
    case Modules::Balloon: {
      ModuleBalloonState.state = ModuleBalloonState.PressureHold;
      digitalWrite(balloonInflateRelayControlPin, HIGH);
      digitalWrite(balloonDeflateRelayControlPin, HIGH);
      DEBUG_PRINT("Balloon module stopped\n");
      break;
    }
    case Modules::Music: {
      if (ModuleMusicState.state) {
        ModuleMusicState.state = false;
        stopAarti();
        DEBUG_PRINT("Music module stopped\n");
      } else {
        DEBUG_PRINT("Music module was already stopped. Nothing to do.\n");
      }
      break;
    }
    case Modules::Automation: {
      break;
    }
    case Modules::ColumnLEDs:
    {
      ModuleColumnLEDsState.state = false;
      digitalWrite(templeLightsLEDRelayControlPin, HIGH);
      DEBUG_PRINT("Column LEDs module stopped\n");
      break;
    }
    default: {
      DEBUG_PRINT("No matching module number %u to stop\n", mod);
      break;
    }
  }
}

// Stops all modules. Automation will not be stopped
void stopAllPhysicalModules() {
  for (uint8_t i = 0; i < Modules::MODULES_COUNT; ++i) {
    if (i == Modules::Automation)
      continue;
    stopModule((Modules)i);
  }
}

void playAarti() { if (wifiSetupSuccessful) wifiSendCmdOverUdpWithRetries("play_aarti"); }
void stopAarti() { if (wifiSetupSuccessful) wifiSendCmdOverUdpWithRetries("stop_aarti"); }