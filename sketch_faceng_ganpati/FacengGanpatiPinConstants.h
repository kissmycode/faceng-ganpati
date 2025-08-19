#ifndef _pin_constants_h_
#define _pin_constants_h_

/*****************************************************************************************/
//
// Various input and output pin numbers
//
/*****************************************************************************************/

constexpr uint8_t bellRelayControlPin = 42;
constexpr uint8_t spinWheenRelayControlPin = 41;
constexpr uint8_t cartForwardRelayControlPin = 40;
constexpr uint8_t cartReverseRelayControlPin = 39;
constexpr uint8_t balloonInflateRelayControlPin = 38;
constexpr uint8_t balloonDeflateRelayControlPin = 37;
constexpr uint8_t automationLEDPin = 36;
constexpr uint8_t bootedOkLEDPin = 35;
constexpr uint8_t wifiOkLEDPin = 34;
// Various input signals for Arduino
constexpr uint8_t startModuleControlPin = 3; // Push to start/forward. Release to stop
constexpr uint8_t stopModuleControlPin = 4;  // Push to reverse. Release to stop. No-op for binary module
constexpr uint8_t cartLimitSwitchInputPin = 31; // Stop Cart on reaching this
constexpr uint8_t balloonTopLimitSwitchInputPin = 30; // Stop balloon inflation on reaching this

constexpr uint8_t relayControlPins[] = 
{
  bellRelayControlPin,
  spinWheenRelayControlPin,
  cartForwardRelayControlPin,
  cartReverseRelayControlPin,
  balloonInflateRelayControlPin,
  balloonDeflateRelayControlPin,
  automationLEDPin,
  wifiOkLEDPin,
  bootedOkLEDPin,
  // If a new output pin is added, ensure it goes here
};
constexpr size_t numRelayControlPins = sizeof(relayControlPins) / sizeof(relayControlPins[0]);

constexpr uint8_t inputControlPins[] = 
{
  startModuleControlPin,
  stopModuleControlPin,
  balloonTopLimitSwitchInputPin,
  cartLimitSwitchInputPin,
  // If a new input pin is added, ensure it goes here
};
constexpr size_t numInputControlPins = sizeof(inputControlPins) / sizeof(inputControlPins[0]); 

#endif // _pin_constants_h_