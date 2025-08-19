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

void startModule(Modules mod, bool moveInPositiveDir) {
  switch (mod) {
    case Modules::Bell: {
      DEBUG_PRINT("Bell module started\n");
      ModuleBellState.state = true;
      digitalWrite(bellRelayControlPin, HIGH);
      break;
    }
    case Modules::SpinWheel: {
      DEBUG_PRINT("SpinWheel module started\n");
      ModuleSpinWheelState.state = true;
      digitalWrite(spinWheenRelayControlPin, HIGH);
      break;
    }
    case Modules::Cart: {
      DEBUG_PRINT("Cart module started");
      if (moveInPositiveDir) {
        ModuleCartState.state = ModuleCartState.Forward;
        digitalWrite(cartForwardRelayControlPin, HIGH);
        DEBUG_PRINT(" foward\n");
      } else {
        ModuleCartState.state = ModuleCartState.Reverse;
        digitalWrite(cartReverseRelayControlPin, HIGH);
        DEBUG_PRINT(" reverse\n");
      }
      break;
    }
    case Modules::Balloon: {
      DEBUG_PRINT("Balloon module started");
      if (moveInPositiveDir) {
        ModuleBalloonState.state = ModuleBalloonState.Inflate;
        digitalWrite(balloonInflateRelayControlPin, HIGH);
        DEBUG_PRINT(" inflation\n");
      } else {
        ModuleBalloonState.state = ModuleBalloonState.Deflate;
        digitalWrite(balloonDeflateRelayControlPin, HIGH);
        DEBUG_PRINT(" deflation\n");
      }
      break;
    }
    case Modules::Music: {
      DEBUG_PRINT("Music module started");
      playAarti();
      ModuleMusicState.state = true;
      break;
    }
    case Modules::Automation: {
      break;
    }
    default: {
      DEBUG_PRINT("No matching module number %u to start\n", mod);
      break;
    }
  }
}

void stopModule(Modules mod) {
  switch (mod) {
    case Modules::Bell: {
      ModuleBellState.state = false;
      digitalWrite(bellRelayControlPin, ModuleBellState.state);
      DEBUG_PRINT("Bell module stopped\n");
      break;
    }
    case Modules::SpinWheel: {
      ModuleSpinWheelState.state = false;
      digitalWrite(spinWheenRelayControlPin, ModuleSpinWheelState.state);
      DEBUG_PRINT("SpinWheel module stopped\n");
      break;
    }
    case Modules::Cart: {
      ModuleCartState.state = ModuleCartState.Stopped;
      digitalWrite(cartForwardRelayControlPin, LOW);
      digitalWrite(cartReverseRelayControlPin, LOW);
      DEBUG_PRINT("Cart module stopped\n");
      break;
    }
    case Modules::Balloon: {
      ModuleBalloonState.state = ModuleBalloonState.PressureHold;
      digitalWrite(balloonInflateRelayControlPin, LOW);
      digitalWrite(balloonDeflateRelayControlPin, LOW);
      DEBUG_PRINT("Balloon module stopped\n");
      break;
    }
    case Modules::Music: {
      stopAarti();
      ModuleMusicState.state = false;
      break;
    }
    case Modules::Automation: {
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

void playAarti() { if (wifiSetupSuccessful) wifiSendCmdOverUdp("play_aarti"); }
void stopAarti() { if (wifiSetupSuccessful) wifiSendCmdOverUdp("stop_aarti"); }