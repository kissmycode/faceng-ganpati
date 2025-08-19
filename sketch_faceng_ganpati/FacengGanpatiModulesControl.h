#ifndef _modules_control_h_
#define _modules_control_h_

/*****************************************************************************************/
//
// Storing and manupulating module state.
//
/*****************************************************************************************/

// Numbered mode-modules map
enum Modules : uint8_t {
    Bell = 0,
    SpinWheel,
    Cart,
    Balloon,
    Automation,
    Music, // Not exposed to user interface. Managed by Automation
    MODULES_COUNT,
};

extern struct _ModuleBellState {
  // false == off; true == on;
  bool state;
} ModuleBellState;

extern struct _ModuleSpinWheelState {
  // false == off; true == on;
  bool state;
} ModuleSpinWheelState;

extern struct _ModuleCartState {
    // Unscoped enum declaration
    enum CartState {
        Stopped,
        Forward,
        Reverse,
    };
    CartState state;
} ModuleCartState;

extern struct _ModuleBalloonState {
    // Unscoped enum declaration
    enum BalloonState {
        PressureHold,
        Inflate,
        Deflate,
    };
    BalloonState state;
} ModuleBalloonState;

extern struct _ModuleMusicState {
  // false == off; true == on;
  bool state;
} ModuleMusicState;

extern struct _ModuleAutomationState {
  // false == off; true == on;
  bool state;
} ModuleAutomationState;

void startModule(Modules mod, bool moveInPositiveDir = true);
void stopModule(Modules mod);
void stopAllPhysicalModules();
void playAarti();
void stopAarti();

#endif // _modules_control_h_