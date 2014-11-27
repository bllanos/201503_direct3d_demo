/*
*
*
*
*/

#define SEGMENTS 30
#define CIRCLE 30

#include "SphereModel.h"
#include <exception>

SphereModel::SphereModel(Transformable* const transformable,
	float radius, XMFLOAT4 * pColors) :
	SimpleColorGeometry(true, SPHEREMODEL_START_MSG_PREFIX, 0),
	m_transformable(transformable),
	m_radius(radius),
	m_circle(CIRCLE),
	m_segment(SEGMENTS),
	m_loop(3 * SEGMENTS),
	m_blend(1.0f),
	m_pColors(pColors)
{
	if (m_radius <= 0) {
		// This is a Microsoft-specific constructor
		throw std::exception("Attempt to construct a SphereModel object with a zero or negative radius.");
	}
}

SphereModel::~SphereModel(void)
{
	if (m_pColors != 0) {
		delete m_pColors;
		m_pColors = 0;
	}
}

HRESULT SphereModel::initialize(ID3D11Device* const device) {

	/* This model provides a cube centered about the origin
	Each face of the cube is a different color so that it is clear which side
	is in view (assuming no custom colors were passed to the constructor).
	*/
	size_t vertexCount = m_circle*m_loop;
	size_t indexCount = m_circle*m_loop * 6;

	SIMPLECOLORGEOMETRY_VERTEX_TYPE* vertices = new SIMPLECOLORGEOMETRY_VERTEX_TYPE[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	for (int i = 0; i < m_loop; i++)
	{
		m_theta = static_cast<float>(2 * i*3.1416 / m_loop); // large loop

		for (int j = 0; j < m_circle; j++) // small circle
		{

			m_phi = static_cast<float>(2 * j*3.1416 / m_circle); // from 0 to 2PI

			thisnor = // normal direction
				XMFLOAT3(cos(m_theta)*sin(m_phi), sin(m_theta)*sin(m_phi), cos(m_phi));
			vertices[i*m_circle + j].position = XMFLOAT3(thisnor.x*m_radius, thisnor.y*m_radius, thisnor.z*m_radius);
			vertices[i*m_circle + j].color = XMFLOAT4(static_cast<float>(i / (m_segment + 0.01)),
				static_cast<float>(j / (m_circle + 0.01)), 0.05f, 1.0f);
			//vertices[i*m_circle + j].color = XMFLOAT4(0.5, 0.5, 0.5, 1.0);

		}
	}

	int count = 0;
	for (int i = 0; i < m_loop; i++)
	{
		for (int j = 0; j < m_circle; j++)
		{
			// two triangles per quad

			// proper order:

			indices[count++] = WORD(((i + 1) % m_loop)*m_circle + j);
			indices[count++] = WORD(i*m_circle + ((j + 1) % m_circle));
			indices[count++] = WORD((i*m_circle + j));
			indices[count++] = WORD(((i + 1) % m_loop)*m_circle + j);
			indices[count++] = WORD(((i + 1) % m_loop)*m_circle + ((j + 1) % m_circle));
			indices[count++] = WORD(i*m_circle + ((j + 1) % m_circle));

		}
	}

	// Free the colour data
	if (m_pColors != 0) {
		delete m_pColors;
		m_pColors = 0;
	}

	// Create Direct3D buffers to store the vertex and index data
	HRESULT result = SimpleColorGeometry::initialize(device,
		vertices, vertexCount,
		indices, indexCount,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (FAILED(result)) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	else {
		// Data is no longer needed
		delete[] vertices;
		delete[] indices;
	}
	return ERROR_SUCCESS;
}

HRESULT SphereModel::getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const {
	return m_transformable->getWorldTransform(worldTransform);
}

float SphereModel::getTransparencyBlendFactor(void) const {
	return m_blend;
}

float SphereModel::setTransparencyBlendFactor(float newFactor) {
	float temp = m_blend;

	// Range checking
	if (newFactor < 0.0f) {
		newFactor = 0.0f;
	}
	else if (newFactor > 1.0f) {
		newFactor = 1.0f;
	}

	m_blend = newFactor;
	return temp;
}

void SphereModel::setParentTransformable(Transformable* theParent)
{
	m_transformable->setParent(theParent);
}

Transformable* SphereModel::getTransformable() const
{
	return m_transformable;
}

HRESULT SphereModel::setTransformables(const std::vector<Transformable*>* const transform) {
	if (transform == 0) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}
	else if (transform->size() != static_cast<std::vector<Transformable*>::size_type>(1)) {
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_INVALID_INPUT);
	}
	m_transformable = (*transform)[0];
	return ERROR_SUCCESS;
}


HRESULT SphereModel::update(const DWORD currentTime, const DWORD updateTimeInterval)
{
	return m_transformable->update(currentTime, updateTimeInterval);
}


float SphereModel::getRadius(){
	//XMFLOAT3 scale = m_transformable->getScale();
	return 1.0f;//((scale.x > scale.y) ? scale.x : scale.y) > scale.z ? ((scale.x > scale.y) ? scale.x : scale.y):scale.z;
}

XMFLOAT3 SphereModel::getPosition(){
	return XMFLOAT3(0, 0, 0);//return m_transformable->getPosition();
}