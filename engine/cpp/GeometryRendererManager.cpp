/*
GeometryRendererManager.cpp
---------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created September 21, 2014

Primary basis: None
Other references: None

Description
  -Implementation of the GeometryRendererManager class
*/

#include "GeometryRendererManager.h"
#include "fileUtil.h"
#include "SimpleColorRenderer.h"
#include "SkinnedRenderer.h"
#include "InvariantParticlesRenderer.h"
#include "SplineParticlesRenderer.h"

using std::map;
using std::wstring;

const GeometryRendererManager::GeometryRendererType
GeometryRendererManager::s_geometryRendererTypesInOrder[] = {
	GeometryRendererType::SimpleColorRenderer,
	GeometryRendererType::SkinnedRendererNoLight,
	GeometryRendererType::SkinnedRendererLight,
	GeometryRendererType::SkinnedTexturedRendererNoLight,
	GeometryRendererType::SkinnedTexturedRendererLight,
	GeometryRendererType::InvariantParticlesRendererNoLight,
	GeometryRendererType::InvariantParticlesRendererLight,
	GeometryRendererType::InvariantTexturedParticlesRendererNoLight,
	GeometryRendererType::InvariantTexturedParticlesRendererLight,
	GeometryRendererType::SplineParticlesRendererNoLight,
	GeometryRendererType::SplineParticlesRendererLight
};

const size_t GeometryRendererManager::s_nGeometryRendererTypes =
	sizeof(s_geometryRendererTypesInOrder) /
	sizeof(GeometryRendererManager::GeometryRendererType);

const wstring GeometryRendererManager::s_geometryRendererTypeNames[] = {
	L"SimpleColorRenderer",
	L"SkinnedRendererNoLight",
	L"SkinnedRendererLight",
	L"SkinnedTexturedRendererNoLight",
	L"SkinnedTexturedRendererLight",
	L"InvariantParticlesRendererNoLight",
	L"InvariantParticlesRendererLight",
	L"InvariantTexturedParticlesRendererNoLight",
	L"InvariantTexturedParticlesRendererLight",
	L"SplineParticlesRendererNoLight",
	L"SplineParticlesRendererLight"
};

HRESULT GeometryRendererManager::wstringToGeometryRendererType(
	GeometryRendererType& out, const wstring& in) {
	for( size_t i = 0; i < s_nGeometryRendererTypes; ++i ) {
		if( in == s_geometryRendererTypeNames[i] ) {
			out = s_geometryRendererTypesInOrder[i];
			return ERROR_SUCCESS;
		}
	}
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
}

HRESULT GeometryRendererManager::geometryRendererTypeToWString(
	wstring& out, const GeometryRendererType& in) {
	for( size_t i = 0; i < s_nGeometryRendererTypes; ++i ) {
		if( in == s_geometryRendererTypesInOrder[i] ) {
			out = s_geometryRendererTypeNames[i];
			return ERROR_SUCCESS;
		}
	}
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
}

GeometryRendererManager::~GeometryRendererManager(void) {
	// Delete all dynamically-allocated IGeometryRenderer objects referred to by the map
	map<GeometryRendererType, IGeometryRenderer*>::iterator end = m_map.end();
	for( map<GeometryRendererType, IGeometryRenderer*>::iterator it = m_map.begin(); it != end; ++it ) {
		delete it->second;
		it->second = 0;
	}
}

#define INITIALIZE_RENDERER(EnumConstant, Class) \
case GeometryRendererType::EnumConstant: \
{ \
	geometryRenderer = new Class(filename); \
	if( FAILED(geometryRenderer->initialize(device)) ) { \
		logMessage(L"Call to initialize() function of renderer '" + field + L"' failed."); \
		result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL); \
		delete geometryRenderer; \
	} else { \
		if( m_map.count(type) != 0 ) { \
			logMessage(L"Found duplicate renderer '" + field + L"' in map of renderers. Code is likely broken."); \
			delete geometryRenderer; \
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE); \
		} else { \
			m_map[type] = geometryRenderer; \
			logMessage(L"Successfully created and initialized renderer '" + field + L"'."); \
		} \
	} \
	geometryRenderer = 0; \
	break; \
}

HRESULT GeometryRendererManager::initialize(ID3D11Device* const device) {

	HRESULT result = ERROR_SUCCESS; // Eventual return value

	GeometryRendererType type;
	const wstring scope = GEOMETRYRENDERERMANAGER_SCOPE;
	wstring field;
	wstring detailsField;
	const bool* boolValue = 0;
	const wstring* stringValue = 0;
	wstring filename;
	IGeometryRenderer* geometryRenderer = 0;

	for( size_t i = 0; i < s_nGeometryRendererTypes; ++i ) {

		// Retrieve the type of renderer to initialize
		type = s_geometryRendererTypesInOrder[i];

		// Get the name of this type of renderer
		if( FAILED(geometryRendererTypeToWString(field, type)) ) {
			logMessage(L"Error converting GeometryRendererType constant to a string. Code is likely broken.");
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
		}

		/* Check whether initialization of this type of renderer is desired
		   (If no flag is found, assume not desired)
		 */
		if( retrieve<Config::DataType::BOOL, bool>(scope, field, boolValue) ) {
			if( *boolValue ) {

				/* Retrieve the location of the configuration file
				   to pass to the renderer constructor
				 */
				// Name of file (possibly containing a path)
				detailsField = field + L"_" CONFIGUSER_INPUT_FILE_NAME_FIELD;
				if( retrieve<Config::DataType::FILENAME, wstring>(scope, detailsField, stringValue) ) {
					filename = *stringValue;
				} else {
					logMessage(L"Cannot set up renderer '" + field + L"' because renderer's configuration file name is missing.");
					result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
					continue;
				}
				// Retrieve an optional file path
				detailsField = field + L"_" CONFIGUSER_INPUT_FILE_PATH_FIELD;
				if( retrieve<Config::DataType::DIRECTORY, wstring>(scope, detailsField, stringValue) ) {
					if( FAILED(fileUtil::combineAsPath(filename, *stringValue, filename)) ) {
						logMessage(L"initialize() : fileUtil::combineAsPath() failed to combine a configuration "
							L"file name and path. Cannot set up renderer '" + field + L"'.");
						result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
						continue;
					}
				}

				// Attempt initialization
				switch( type ) {
					INITIALIZE_RENDERER(SimpleColorRenderer, SimpleColorRenderer)
					INITIALIZE_RENDERER(SkinnedRendererNoLight, SkinnedRenderer)
					INITIALIZE_RENDERER(SkinnedRendererLight, SkinnedRenderer)
					INITIALIZE_RENDERER(SkinnedTexturedRendererNoLight, SkinnedRenderer)
					INITIALIZE_RENDERER(SkinnedTexturedRendererLight, SkinnedRenderer)
					INITIALIZE_RENDERER(InvariantParticlesRendererNoLight, InvariantParticlesRenderer)
					INITIALIZE_RENDERER(InvariantParticlesRendererLight, InvariantParticlesRenderer)
					INITIALIZE_RENDERER(InvariantTexturedParticlesRendererNoLight, InvariantParticlesRenderer)
					INITIALIZE_RENDERER(InvariantTexturedParticlesRendererLight, InvariantParticlesRenderer)
					INITIALIZE_RENDERER(SplineParticlesRendererNoLight, SplineParticlesRenderer)
					INITIALIZE_RENDERER(SplineParticlesRendererLight, SplineParticlesRenderer)
				default:
					logMessage(L"Reached default case of geometry renderer initialization switch statement. Code is broken.");
					return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
				}
			}
		}
	}

	return result;
}

HRESULT GeometryRendererManager::render(
	ID3D11DeviceContext* const context,
	const IGeometry& geometry,
	const Camera* const camera,
	GeometryRendererType rendererType
	) {

	HRESULT result = ERROR_SUCCESS;
	IGeometryRenderer* geometryRenderer = 0;
	wstring name;

	// Retrieve the appropriate renderer
	map<GeometryRendererType, IGeometryRenderer*>::const_iterator
		mapping = m_map.find(rendererType);
	if( mapping != m_map.cend() ) {
		geometryRenderer = mapping->second;

		// Render the geometry
		if( FAILED(geometryRenderer->render(context, geometry, camera)) ) {

			if( FAILED(geometryRendererTypeToWString(name, rendererType)) ) {
				name = L"[Error converting GeometryRendererType constant to a string. Code is likely broken.]";
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
			}

			logMessage(L"Call to render() on renderer of type: " + name + L" failed.");
			if( SUCCEEDED(result) ) {
				result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
			}
		}

	} else {

		// Not found
		if( FAILED(geometryRendererTypeToWString(name, rendererType)) ) {
			name = L"[Error converting GeometryRendererType constant to a string. Code is likely broken.]";
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_BROKEN_CODE);
		}

		logMessage(L"render() could not find a renderer of type: " + name + L" to use for rendering.");
		if( SUCCEEDED(result) ) {
			result = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_DATA_NOT_FOUND);
		}
	}

	return result;
}