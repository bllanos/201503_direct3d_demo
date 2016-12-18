/*
TwoFrameSSSE.h
--------------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos

Created November 10, 2014

Primary basis: SSSE.h

Description
  -A screen-space special effect which has access to both
   the current frame and the previous frame for input.
     -The previous frame can either include the SSSE,
	  or exclude the SSSE, as determined by configuration data.
*/

#pragma once

#include "SSSE.h"

#define TWOFRAMESSSE_NTEXTURES 2

// Default log message prefix used before more information is available
#define TWOFRAMESSSE_START_MSG_PREFIX L"TwoFrameSSSE"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

#define TWOFRAMESSSE_SCOPE L"TwoFrameSSSE"

// Determines the frequency at which the past frame texture will be updated
#define TWOFRAMESSSE_PERIOD_MIN 0
#define TWOFRAMESSSE_PERIOD_DEFAULT 	0 // Continual (zero milliseconds)
#define TWOFRAMESSSE_PERIOD_FIELD		L"pastFrameUpdateInterval"

// Setting for 'm_refreshPastWithSSSE'
#define TWOFRAMESSSE_PASTFRAME_CONTAINS_SSSE_DEFAULT 	true
#define TWOFRAMESSSE_PASTFRAME_CONTAINS_SSSE_FIELD		L"pastFrameContainsSSSE"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
 */
#define TWOFRAMESSSE_LOGUSER_SCOPE		L"TwoFrameSSSE_LogUser"
#define TWOFRAMESSSE_CONFIGUSER_SCOPE		L"TwoFrameSSSE_ConfigUser"

/* Texture constructor and configuration parameters */
#define TWOFRAMESSSE_TEXTURE_CURRENT_FIELD_PREFIX L"currentFrame_"
#define TWOFRAMESSSE_TEXTURE_PAST_FIELD_PREFIX L"pastFrame_"

class TwoFrameSSSE : public SSSE {

public:

	/* Proxy of the SSSE constructor
	 */
	template<typename ConfigIOClass> TwoFrameSSSE(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

protected:
	/* Passes the appropriate arguments to SSSE::configure()
	   If 'configUserScope' is null, it defaults to TWOFRAMESSSE_SCOPE_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to TWOFRAMESSSE_SCOPE_LOGUSER_SCOPE.
	*/
	virtual HRESULT configure(const std::wstring& scope = TWOFRAMESSSE_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

public:
	virtual ~TwoFrameSSSE(void);

	/* Augments the base class version of this function as follows:
	     -After rendering, the current frame texture is copied into
		    the past frame texture.
	 */
	virtual HRESULT apply(ID3D11DeviceContext* const context) override;

	/* Uses the back buffer ('backBuffer' parameter) as the source
	   of data for updating the past frame texture, if
	   'm_refreshPastWithSSSE' has been configured to 'true'.

	   Also calls the base class's initialize() function.
	 */
	virtual HRESULT initialize(ID3D11Device* const device, ID3D11Texture2D* const backBuffer, UINT width, UINT height) override;

	// Helper functions
protected:

	/* Texture initialization
	   Augments the base class version of this function as follows:
	     -Initializes the second texture (the past frame texture)
		    with the background colour.
	 */
	virtual HRESULT initializeTextures(ID3D11Device* const device, ID3D11Texture2D* const backBuffer) override;

	/* Provides SSSE_NVERTICES vertices
	   to initialize the vertex buffer,
	   with D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP topology.
	*/
	virtual HRESULT createVertexData(SSSE_VERTEX_TYPE* const vertices) override;

	// Data members
private:

	/* Past frame copying will be done at a configurable time.
	   This variable stores the time of the last copy.
	 */
	DWORD m_refreshTimer;

	// Interval at which 'm_refreshTimer' will reset [milliseconds]
	DWORD m_refreshPeriod;

	/* If true, the past frame texture will be updated
	   with the current frame with the SSSE applied.

	   If false, the past frame texture will be updated
	   with the current frame as it was before the SSSE
	   was applied.
	 */
	bool m_refreshPastWithSSSE;

	/* Back buffer which can be copied into the past frame
	   If not set, the object will behave as though
	   'm_refreshPastWithSSSE' is false.
	 */
	ID3D11Texture2D* m_backBuffer;

	// Currently not implemented - will cause linker errors if called
private:
	TwoFrameSSSE(const TwoFrameSSSE& other);
	TwoFrameSSSE& operator=(const TwoFrameSSSE& other);
};

template<typename ConfigIOClass> TwoFrameSSSE::TwoFrameSSSE(
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SSSE(
	true,
	TWOFRAMESSSE_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_refreshTimer(0), m_refreshPeriod(TWOFRAMESSSE_PERIOD_DEFAULT),
	m_backBuffer(0)
{
	if (FAILED(configure())) {
		logMessage(L"Configuration failed.");
	}
}