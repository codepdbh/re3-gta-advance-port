//
// On-screen virtual gamepad state for Android, fed from TouchControlsView.java
// over JNI and consumed once per frame by CaptureTouchPad() in skel/sdl2/sdl2.cpp.
//

#ifndef RE3_TOUCHCONTROLS_H
#define RE3_TOUCHCONTROLS_H

#if defined ANDROID

struct TouchPadState
{
	// Movement / camera sticks, normalized -1..1 (same convention a real
	// gamepad's axes are converted to before being applied to CControllerState).
	float leftX, leftY;
	float rightX, rightY;

	// Everything below mirrors a real gamepad's digital buttons, using the
	// game's actual default PS2-style bindings for a 16-button pad (see
	// CControllerConfigManager::InitDefaultControlConfigJoyPad() in
	// ControllerConfig.cpp -- this is not a guess):
	//   Circle    = fire weapon
	//   Cross     = accelerate (vehicle) / sprint (on foot)
	//   Square    = brake (vehicle) / jump (on foot)
	//   Triangle  = enter / exit vehicle
	//   L1        = center camera behind player / change radio station
	//   R1        = handbrake / lock target (aim)
	//   L2        = cycle weapon left / look left (vehicle)
	//   R2        = cycle weapon right / look right (vehicle)
	//   Select    = change camera view
	//   L3        = horn
	//   R3        = look behind / toggle submissions
	//   Start     = pause
	//   D-Pad     = frontend/menu navigation (GO_FORWARD/BACK/LEFT/RIGHT)
	bool circle, cross, square, triangle;
	bool leftShoulder1, rightShoulder1; // L1, R1
	bool leftShoulder2, rightShoulder2; // L2, R2
	bool select, start;
	bool leftStickClick, rightStickClick; // L3, R3
	bool dpadUp, dpadDown, dpadLeft, dpadRight;

	// Frontend menus support real mouse hover/click (CMenuManager::CheckHover(),
	// see cursorCB() in skel/sdl2/sdl2.cpp for the desktop equivalent this
	// mirrors) -- letting a tap on a menu item work directly, instead of only
	// through the D-Pad. Screen-pixel coordinates, same space as a real
	// SDL_MOUSEMOTION event. Only applied while a menu is active.
	float menuMouseX, menuMouseY;
	bool menuMouseDown;
};

extern TouchPadState g_TouchState;

// Which physical input the player is using right now. The on-screen controls
// are only shown (and only read) in INPUT_TOUCH; INPUT_PAD is a connected
// gamepad being used; INPUT_KBM is a hardware keyboard / mouse. Changed by:
// a finger touching the screen (TouchControlsView), gamepad input or a
// controller connecting (sdl2.cpp), a key press or mouse event (sdl2.cpp).
enum {
	INPUT_TOUCH = 0,
	INPUT_PAD   = 1,
	INPUT_KBM   = 2,
};
extern volatile int g_InputMode;

// Mouse movement accumulated from SDL_MOUSEMOTION since the last frame,
// consumed by CPad::UpdateMouse() while in relative-mouse (in-game) mode.
extern float g_MouseRelX, g_MouseRelY;

#endif

#endif //RE3_TOUCHCONTROLS_H
