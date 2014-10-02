/* Adapted from the following source: COMP2501A Tutorial 4 */

////////////////////////////////////////////////////////////////////////////////
// Filename: Keyboard.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Keyboard.h"
#include "engineGlobals.h"
#include <exception>

// Initialize character key codes
const int Keyboard::ascii_A = 65;
const int Keyboard::ascii_B = 66;
const int Keyboard::ascii_C = 67;
const int Keyboard::ascii_D = 68;
const int Keyboard::ascii_E = 69;
const int Keyboard::ascii_F = 70;
const int Keyboard::ascii_G = 71;
const int Keyboard::ascii_H = 72;
const int Keyboard::ascii_I = 73;
const int Keyboard::ascii_J = 74;
const int Keyboard::ascii_K = 75;
const int Keyboard::ascii_L = 76;
const int Keyboard::ascii_M = 77;
const int Keyboard::ascii_N = 78;
const int Keyboard::ascii_O = 79;
const int Keyboard::ascii_P = 80;
const int Keyboard::ascii_Q = 81;
const int Keyboard::ascii_R = 82;
const int Keyboard::ascii_S = 83;
const int Keyboard::ascii_T = 84;
const int Keyboard::ascii_U = 85;
const int Keyboard::ascii_V = 86;
const int Keyboard::ascii_W = 87;
const int Keyboard::ascii_X = 88;
const int Keyboard::ascii_Y = 89;
const int Keyboard::ascii_Z = 90;

Keyboard::Keyboard(void) : m_keys(0), m_lastKeys(0), 
	m_timePressed(static_cast<DWORD>(0)), m_timeReleased(static_cast<DWORD>(0)), 
	m_t(static_cast<DWORD>(0)), m_tPast(static_cast<DWORD>(0))
{}

Keyboard::~Keyboard()
{
	if (m_keys)
	{
		delete m_keys;
		m_keys = 0;
	}

	if (m_lastKeys)
	{
		delete m_lastKeys;
		m_lastKeys = 0;
	}

	if (m_timePressed) 
	{
		delete m_timePressed;
		m_timePressed = 0;
	}

	if (m_timeReleased)
	{
		delete m_timeReleased;
		m_timeReleased = 0;
	}
}

Keyboard::Keyboard(const Keyboard& other)
{
	throw std::exception("Not implemented"); // This is a Microsoft-specific constructor
}

Keyboard& Keyboard::operator=(const Keyboard& other)
{
	throw std::exception("Not implemented"); // This is a Microsoft-specific constructor
}

int Keyboard::Initialize(void)
{
	m_keys = new bool[N_KEYS];
	if (!m_keys)
	{
		return C_BAD_CNSTRCT;
	}

	m_lastKeys = new bool[N_KEYS];
	if (!m_lastKeys) 
	{
		return C_BAD_CNSTRCT;
	}

	m_timePressed = new DWORD[N_KEYS];
	if (!m_timePressed)
	{
		return C_BAD_CNSTRCT;
	}

	m_timeReleased = new DWORD[N_KEYS];
	if (!m_timeReleased)
	{
		return C_BAD_CNSTRCT;
	}

	// Initialize all the keys to being released and not pressed.
	for (int i = 0; i < N_KEYS; ++i)
	{
		m_keys[i] = false;
		m_lastKeys[i] = false;
		m_timePressed[i] = static_cast<DWORD>(0);
		m_timeReleased[i] = static_cast<DWORD>(0);
	}

	m_t = GetTickCount();

	return C_OK;
}

int Keyboard::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return C_OK;
}


int Keyboard::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return C_OK;
}


bool Keyboard::IsKeyDown(unsigned int key) const
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}

bool Keyboard::Up(unsigned int key)
{
	if (!m_keys[key] && m_lastKeys[key]) {
		return true;
	}
	return false;
}

bool Keyboard::Down(unsigned int key)
{
	if (m_keys[key] && !m_lastKeys[key]) {
		return true;
	}
	return false;
}

DWORD Keyboard::TimePressed(unsigned int key) const
{
	return m_timePressed[key];
}

DWORD Keyboard::TimeReleased(unsigned int key) const
{
	return m_timeReleased[key];
}

int Keyboard::Update(void)
{
	// save the time from last iteration
	m_tPast = m_t;
	// update the time
	m_t = GetTickCount();

	for (int i = 0; i < N_KEYS; ++i)
	{
		// handle TimePressed function requirements
		if (m_keys[i] && m_lastKeys[i]) {
			m_timePressed[i] += m_t - m_tPast;
		}
		else {
			m_timePressed[i] = static_cast<DWORD>(0);
		}

		// handle TimeReleased function requirements
		if (!m_keys[i] && !m_lastKeys[i]) {
			m_timeReleased[i] += m_t - m_tPast;
		}
		else {
			m_timeReleased[i] = static_cast<DWORD>(0);
		}

		// handle Up and Down function requirements
		m_lastKeys[i] = m_keys[i];
	}

	

	return C_OK;
}

LRESULT CALLBACK Keyboard::winProc(HWND bwin, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	int result = 1; // Zero would mean that the message was fully handled here

	switch (umsg)
	{
	case WM_KEYDOWN:
		KeyDown((unsigned int)wparam);
		return 0;
	case WM_KEYUP:
		KeyUp((unsigned int)wparam);
		return 0;
	default:
		result = C_BAD_INPUT; // Someone else must process this message
	}

	return result;
}