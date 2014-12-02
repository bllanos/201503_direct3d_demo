/*
CubeModel.cpp
-------------

Adapted for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Incorporated September 20, 2014

Primary basis: COMP2501A project code
  (Bernard Llanos, Alec McGrail, Benjamin Smith - Winter 2014)
  -Adapted from the following source: COMP2501A Tutorial 5

Description
  -Implementation of the CubeModel class
*/

#include "CubeModel.h"
#include <exception>


CubeModel::CubeModel(float lengthX, float lengthY, float lengthZ, XMFLOAT4 * pColors) :
SimpleColorGeometry(true, CUBEMODEL_START_MSG_PREFIX, 0),
m_xlen(lengthX), m_ylen(lengthY), m_zlen(lengthZ),
m_blend(1.0f), m_pColors(pColors)
{
	m_transform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	if (m_xlen <= 0.0f || m_ylen <= 0.0f || m_zlen <= 0.0f) {
		// This is a Microsoft-specific constructor
		throw std::exception("Attempt to construct a CubeModel object with one or more negative or zero dimensions.");
	}
}

CubeModel::CubeModel(Transformable* transform, float lengthX, float lengthY, float lengthZ, XMFLOAT4 * pColors) :
SimpleColorGeometry(true, CUBEMODEL_START_MSG_PREFIX, 0),
m_xlen(lengthX), m_ylen(lengthY), m_zlen(lengthZ),
m_blend(1.0f), m_pColors(pColors)
{
	m_transform = transform;
	if (m_xlen <= 0.0f || m_ylen <= 0.0f || m_zlen <= 0.0f) {
		// This is a Microsoft-specific constructor
		throw std::exception("Attempt to construct a CubeModel object with one or more negative or zero dimensions.");
	}
}

CubeModel::~CubeModel(void)
{
	if( m_pColors != 0 ) {
		delete m_pColors;
		m_pColors = 0;
	}
}

HRESULT CubeModel::initialize(ID3D11Device* const device) {

	/* This model provides a cube centered about the origin
	   Each face of the cube is a different color so that it is clear which side
	   is in view (assuming no custom colors were passed to the constructor).
	*/
	size_t vertexCount = 24;
	size_t indexCount = 36;

	SIMPLECOLORGEOMETRY_VERTEX_TYPE* vertices = new SIMPLECOLORGEOMETRY_VERTEX_TYPE[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	XMFLOAT4 vertexColor;
	vertexColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); //red
	if( m_pColors ) vertexColor = m_pColors[0];

	//Front Face
	vertices[0].position = XMFLOAT3(-m_xlen / 2, -m_ylen / 2, -m_zlen / 2); // Front Bottom left.
	vertices[0].color = vertexColor;

	vertices[1].position = XMFLOAT3(-m_xlen / 2, m_ylen / 2, -m_zlen / 2);  // Front Top left.
	vertices[1].color = vertexColor;

	vertices[2].position = XMFLOAT3(m_xlen / 2, -m_ylen / 2, -m_zlen / 2);  // Front Bottom right.
	vertices[2].color = vertexColor;

	vertices[3].position = XMFLOAT3(m_xlen / 2, m_ylen / 2, -m_zlen / 2);   // Front Top right.
	vertices[3].color = vertexColor;

	//Back Face
	vertexColor = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f); //magenta
	if( m_pColors ) vertexColor = m_pColors[1];

	vertices[4].position = XMFLOAT3(-m_xlen / 2, -m_ylen / 2, m_zlen / 2); // Back Bottom Left.
	vertices[4].color = vertexColor;

	vertices[5].position = XMFLOAT3(-m_xlen / 2, m_ylen / 2, m_zlen / 2);  // Back Top Left.
	vertices[5].color = vertexColor;

	vertices[6].position = XMFLOAT3(m_xlen / 2, -m_ylen / 2, m_zlen / 2);  // Back Bottom Right.
	vertices[6].color = vertexColor;

	vertices[7].position = XMFLOAT3(m_xlen / 2, m_ylen / 2, m_zlen / 2);   // Back Top Right.
	vertices[7].color = vertexColor;

	//Right Side
	vertexColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f); //blue
	if( m_pColors ) vertexColor = m_pColors[2];

	vertices[8].position = XMFLOAT3(m_xlen / 2, m_ylen / 2, -m_zlen / 2);   // Front Top right.
	vertices[8].color = vertexColor;

	vertices[9].position = XMFLOAT3(m_xlen / 2, -m_ylen / 2, -m_zlen / 2);  // Front Bottom right.
	vertices[9].color = vertexColor;

	vertices[10].position = XMFLOAT3(m_xlen / 2, m_ylen / 2, m_zlen / 2);   // Back Top right.
	vertices[10].color = vertexColor;

	vertices[11].position = XMFLOAT3(m_xlen / 2, -m_ylen / 2, m_zlen / 2);  // Back Bottom right.
	vertices[11].color = vertexColor;

	//Left Side
	vertexColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); //green
	if( m_pColors ) vertexColor = m_pColors[3];

	vertices[12].position = XMFLOAT3(-m_xlen / 2, m_ylen / 2, -m_zlen / 2);   // Front Top Left.
	vertices[12].color = vertexColor;

	vertices[13].position = XMFLOAT3(-m_xlen / 2, -m_ylen / 2, -m_zlen / 2);  // Front Bottom Left.
	vertices[13].color = vertexColor;

	vertices[14].position = XMFLOAT3(-m_xlen / 2, m_ylen / 2, m_zlen / 2);   // Back Top Left.
	vertices[14].color = vertexColor;

	vertices[15].position = XMFLOAT3(-m_xlen / 2, -m_ylen / 2, m_zlen / 2);  // Back Bottom Left.
	vertices[15].color = vertexColor;


	//Top Face
	vertexColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); //yellow
	if( m_pColors ) vertexColor = m_pColors[4];

	vertices[16].position = XMFLOAT3(-m_xlen / 2, m_ylen / 2, -m_zlen / 2);   // Front Top Left.
	vertices[16].color = vertexColor;

	vertices[17].position = XMFLOAT3(m_xlen / 2, m_ylen / 2, -m_zlen / 2);  // Front Top Right.
	vertices[17].color = vertexColor;

	vertices[18].position = XMFLOAT3(-m_xlen / 2, m_ylen / 2, m_zlen / 2);   // Back Top Left.
	vertices[18].color = vertexColor;

	vertices[19].position = XMFLOAT3(m_xlen / 2, m_ylen / 2, m_zlen / 2);  // Back Top Right.
	vertices[19].color = vertexColor;

	//Bottom Face
	vertexColor = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f); //cyan
	if( m_pColors ) vertexColor = m_pColors[5];

	vertices[20].position = XMFLOAT3(-m_xlen / 2, -m_ylen / 2, -m_zlen / 2);   // Front Bottom Left.
	vertices[20].color = vertexColor;

	vertices[21].position = XMFLOAT3(m_xlen / 2, -m_ylen / 2, -m_zlen / 2);  // Front Bottom Right.
	vertices[21].color = vertexColor;

	vertices[22].position = XMFLOAT3(-m_xlen / 2, -m_ylen / 2, m_zlen / 2);   // Back Bottom Left.
	vertices[22].color = vertexColor;

	vertices[23].position = XMFLOAT3(m_xlen / 2, -m_ylen / 2, m_zlen / 2);  // Back Bottom Right.
	vertices[23].color = vertexColor;


	// Load the index array with data.
	// Two triangles per face. The directions are consistent
	// With back-face culling in a left-hand co-ordinate system.

	//Front Face
	indices[0] = 0;  // Front Bottom left.
	indices[1] = 1;  // Front Top left.
	indices[2] = 2;  // Front Bottom right.
	indices[3] = 1;  // Front Top left.
	indices[4] = 3;  // Front Top right.  
	indices[5] = 2;  // Front Bottom right.

	//Right Side
	indices[6] = 9;  // Front Bottom right.
	indices[7] = 8;  // Front Top right.
	indices[8] = 11;  // Back Bottom right.
	indices[9] = 8;  // Front Top right.
	indices[10] = 10;  // Back Top right.
	indices[11] = 11;  // Back Bottom right.

	//Left Side
	indices[12] = 12;  // Top Front Left.
	indices[13] = 13;  // Bottom Front Left.
	indices[14] = 15;  // Bottom Back Left.
	indices[15] = 12;  // Top Front Left.
	indices[16] = 15;  // Bottom Back Left.
	indices[17] = 14;  // Top Back Left.

	//Top Face
	indices[18] = 17;  // Top Front Right
	indices[19] = 16;  // Top Front Left
	indices[20] = 18;  // Top Back Left
	indices[21] = 17;  // Top Front Right
	indices[22] = 18;  // Top Back Left
	indices[23] = 19;  // Top Back Right.

	//Bottom Face
	indices[24] = 20;  // Bottom Front Left
	indices[25] = 23;  // Bottom Back Right
	indices[26] = 22;  // Bottom Back Left
	indices[27] = 20;  // Bottom Front Left
	indices[28] = 21;  // Bottom Front Right
	indices[29] = 23;  // Bottom Back Right

	//Back Face
	indices[30] = 7;  // Top Back Right
	indices[31] = 5;  // Top Back Left
	indices[32] = 4;  // Bottom Back Left
	indices[33] = 7;  // Top Back Right
	indices[34] = 4;  // Bottom Back Left
	indices[35] = 6;  // Bottom Back Right

	// Free the colour data
	if( m_pColors != 0 ) {
		delete m_pColors;
		m_pColors = 0;
	}

	// Create Direct3D buffers to store the vertex and index data
	HRESULT result = SimpleColorGeometry::initialize(device,
		vertices, vertexCount,
		indices, indexCount,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if( FAILED(result) ) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	} else {
		// Data is no longer needed
		delete[] vertices;
		delete[] indices;
	}
	return ERROR_SUCCESS;
}

HRESULT CubeModel::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const {
	return m_transform->getWorldTransformNoScale(worldTransform);
}

float CubeModel::getTransparencyBlendFactor(void) const {
	return m_blend;
}

float CubeModel::setTransparencyBlendFactor(float newFactor) {
	float temp = m_blend;

	// Range checking
	if( newFactor < 0.0f ) {
		newFactor = 0.0f;
	} else if( newFactor > 1.0f ) {
		newFactor = 1.0f;
	}

	m_blend = newFactor;
	return temp;
}

void CubeModel::setParentTransformable(Transformable* theParent)
{
	m_transform->setParent(theParent);
}

Transformable* CubeModel::getTransformable() const
{
	return m_transform;
}

HRESULT CubeModel::update(const DWORD currentTime, const DWORD updateTimeInterval)
{
	return m_transform->update(currentTime, updateTimeInterval);
}

HRESULT CubeModel::setTransformables(const std::vector<Transformable*>* const transform) {
	if (transform == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}
	else if (transform->size() != static_cast<std::vector<Transformable*>::size_type>(1)) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	m_transform = (*transform)[0];
	return ERROR_SUCCESS;
}

float CubeModel::getRadius(){

	float theX = m_xlen;
	float theY = m_ylen;
	float theZ = m_zlen;

	float theDiagonal = sqrt(pow(theX, 2) + pow(theY, 2) + pow(theZ, 2));

	return theDiagonal*0.5f;
}

XMFLOAT3 CubeModel::getPosition(){
	return m_transform->getPosition();
}