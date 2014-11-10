/*
OneTextureSSSE.h
------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 10, 2014

Primary basis: SSSE.h

Description
  -A basic screen-space special effect which only uses one texture
   for input during rendering.
*/

#pragma once

#include "SSSE.h"

#define ONETEXTURESSSE_NTEXTURES 1

// Default log message prefix used before more information is available
#define ONETEXTURESSSE_START_MSG_PREFIX L"OneTextureSSSE"

/* The following definitions are:
-Key parameters used to retrieve configuration data
-Default values used in the absence of configuration data
or constructor/function arguments (where necessary)
*/

#define ONETEXTURESSSE_SCOPE L"OneTextureSSSE"

/* LogUser and ConfigUser configuration parameters
   Refer to LogUser.h and ConfigUser.h
*/
#define ONETEXTURESSSE_LOGUSER_SCOPE		L"OneTextureSSSE_LogUser"
#define ONETEXTURESSSE_CONFIGUSER_SCOPE		L"OneTextureSSSE_ConfigUser"

/* Texture constructor and configuration parameters */
#define ONETEXTURESSSE_TEXTURE_FIELD_PREFIX L"texture"

class OneTextureSSSE : public SSSE {

public:

	/* Proxy of the SSSE constructor
	 */
	template<typename ConfigIOClass> OneTextureSSSE(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

protected:

	/* Passes the appropriate arguments to SSSE::configure()
	   If 'configUserScope' is null, it defaults to ONETEXTURESSSE_CONFIGUSER_SCOPE.
	   If 'logUserScope' is null, it defaults to ONETEXTURESSSE_LOGUSER_SCOPE.
	*/
	virtual HRESULT configure(const std::wstring& scope = ONETEXTURESSSE_SCOPE, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

public:
	virtual ~OneTextureSSSE(void);

	// Helper functions
protected:

	/* Provides SSSE_NVERTICES vertices
	   to initialize the vertex buffer,
	   with D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP topology.
	 */
	virtual HRESULT createVertexData(SSSE_VERTEX_TYPE*& vertices) override;

	// Data members
private:

	// Currently not implemented - will cause linker errors if called
private:
	OneTextureSSSE(const OneTextureSSSE& other);
	OneTextureSSSE& operator=(const OneTextureSSSE& other);
};

template<typename ConfigIOClass> OneTextureSSSE::OneTextureSSSE(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	SSSE(
	true,
	ONETEXTURESSSE_START_MSG_PREFIX,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	)
{}