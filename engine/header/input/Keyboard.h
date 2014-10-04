/* Adapted from the following source: COMP2501A Tutorial 4 */

////////////////////////////////////////////////////////////////////////////////
// Filename: Keyboard.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define N_KEYS 256 // Number of keys in the m_keys array

#include "IWinMessageHandler.h"
#include "BasicWindow.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Keyboard
////////////////////////////////////////////////////////////////////////////////
class Keyboard : public IWinMessageHandler
{
public:
	Keyboard(void);
	~Keyboard(void);

	// The following are not to be used and will throw exceptions
	Keyboard(const Keyboard&);
	Keyboard& operator=(const Keyboard& other);

	int Initialize(void);
	int KeyDown(unsigned int);
	int KeyUp(unsigned int);
	bool IsKeyDown(unsigned int) const;

	// returns true if the given key was pressed last iteration, but not this iteration.
	bool Up(unsigned int);
	// returns true if given key was pressed this iteration and not last.
	bool Down(unsigned int);
	// returns number of milliseconds the given key has been held down since last being pressed.
	DWORD TimePressed(unsigned int) const;
	// tracks length of time the given key has not been pressed down since last being pressed/since program start.
	DWORD TimeReleased(unsigned int) const;

	// ensures the Up, Down, TimePressed, and TimeReleased functions operate correctly.
	int Update(void);
	
	LRESULT CALLBACK winProc(BasicWindow*, UINT, WPARAM, LPARAM);

	// Key codes
public:
	static const int Keyboard::ascii_A;
	static const int Keyboard::ascii_B;
	static const int Keyboard::ascii_C;
	static const int Keyboard::ascii_D;
	static const int Keyboard::ascii_E;
	static const int Keyboard::ascii_F;
	static const int Keyboard::ascii_G;
	static const int Keyboard::ascii_H;
	static const int Keyboard::ascii_I;
	static const int Keyboard::ascii_J;
	static const int Keyboard::ascii_K;
	static const int Keyboard::ascii_L;
	static const int Keyboard::ascii_M;
	static const int Keyboard::ascii_N;
	static const int Keyboard::ascii_O;
	static const int Keyboard::ascii_P;
	static const int Keyboard::ascii_Q;
	static const int Keyboard::ascii_R;
	static const int Keyboard::ascii_S;
	static const int Keyboard::ascii_T;
	static const int Keyboard::ascii_U;
	static const int Keyboard::ascii_V;
	static const int Keyboard::ascii_W;
	static const int Keyboard::ascii_X;
	static const int Keyboard::ascii_Y;
	static const int Keyboard::ascii_Z;

private:
	// this iteration's key values
	bool* m_keys;
	// last iteration's key values
	bool* m_lastKeys;
	// the time each key has been held down
	DWORD* m_timePressed;
	// the time each key has been released
	DWORD* m_timeReleased;

	// Time of the current message processing round
	DWORD m_t;
	// Time of the last message processing round
	DWORD m_tPast;
};

#endif