/* Adapted from the following source: COMP2501A Tutorial 4 */

////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#define N_KEYS 256 // Number of keys in the m_keys array

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass(void);
	~InputClass(void);

	// The following are not to be used and will throw exceptions
	InputClass(const InputClass&);
	InputClass& operator=(const InputClass& other);

	int Initialize(void);
	int KeyDown(unsigned int);
	int KeyUp(unsigned int);
	bool IsKeyDown(unsigned int) const;

	// Key codes
public:
	static const int ascii_A;
	static const int ascii_B;
	static const int ascii_C;
	static const int ascii_D;
	static const int ascii_E;
	static const int ascii_I;
	static const int ascii_O;
	static const int ascii_P;
	static const int ascii_R;
	static const int ascii_U;
	static const int ascii_V;
	static const int ascii_W;
	static const int ascii_X;
	static const int ascii_Y;
	static const int ascii_Z;

private:
	bool* m_keys;
};

#endif