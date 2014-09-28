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
	void ResetKeys();
	
	LRESULT CALLBACK winProc(HWND, UINT, WPARAM, LPARAM);

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
	bool* m_keys;
};

#endif