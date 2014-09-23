/* Adapted from the following source: COMP2501A Tutorial 4 */

////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"
#include "engineGlobals.h"
#include <exception>

// Initialize character key codes
const int InputClass::ascii_A = 65;
const int InputClass::ascii_B = 66;
const int InputClass::ascii_C = 67;
const int InputClass::ascii_D = 68;
const int InputClass::ascii_E = 69;
const int InputClass::ascii_I = 73;
const int InputClass::ascii_O = 79;
const int InputClass::ascii_P = 80;
const int InputClass::ascii_R = 82;
const int InputClass::ascii_U = 85;
const int InputClass::ascii_V = 86;
const int InputClass::ascii_W = 87;
const int InputClass::ascii_X = 88;
const int InputClass::ascii_Y = 89;
const int InputClass::ascii_Z = 90;

InputClass::InputClass(void) : m_keys(0)
{}

InputClass::~InputClass()
{
	if(m_keys)
	{
		delete m_keys;
		m_keys = 0;
	}
}

InputClass::InputClass(const InputClass& other)
{
	throw std::exception("Not implemented"); // This is a Microsoft-specific constructor
}

InputClass& InputClass::operator=(const InputClass& other)
{
	throw std::exception("Not implemented"); // This is a Microsoft-specific constructor
}

int InputClass::Initialize(void)
{
	m_keys = new bool[N_KEYS];
	if(!m_keys)
	{
		return C_BAD_CNSTRCT;
	}

	// Initialize all the keys to being released and not pressed.
	for(int i=0; i<N_KEYS; ++i)
	{
		m_keys[i] = false;
	}
	return C_OK;
}

int InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return C_OK;
}


int InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return C_OK;
}


bool InputClass::IsKeyDown(unsigned int key) const
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}