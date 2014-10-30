/*
*
*
*/

#pragma once

#include <DirectXMath.h>
#include <vertexTypes.h>
#include "SimpleColorGeometry.h"
#include "Transformable.h"

#include <string>

#define SPHEREMODEL_START_MSG_PREFIX L"SphereModel "

class SphereModel : public SimpleColorGeometry
{
public:
	/* The 'pColors' argument will be owned by this object,
	as with the 'transformable' argument.
	(Both will be deleted by this object's destructor.)
	*/
	SphereModel(Transformable* const transformable,
		float radius, DirectX::XMFLOAT4 * pColors = 0);

	virtual ~SphereModel(void);

	virtual HRESULT initialize(ID3D11Device* const d3dDevice);

	virtual HRESULT getWorldTransform(DirectX::XMFLOAT4X4& worldTransform) const override;

	virtual float getTransparencyBlendFactor(void) const override;

	/* Returns the previous value */
	virtual float setTransparencyBlendFactor(float newFactor);

	//float getBoundingRadius();

	//XMFLOAT3 getBoundingOrigin();

	// Data members
protected:
	/* The Transformable object is responsible for making this
	model behave like a physical entity.

	The SphereModel object assumes that it owns this pointer
	(i.e. the CubeModel destructor will delete the pointer).
	*/
	Transformable* m_transformable;

	/* Model dimensions */
	float m_radius;
	float m_theta;
	float m_phi;
	int m_circle;
	int m_loop;
	int m_segment;

	XMFLOAT3 thisnor;

	// Transparency multiplier
	float m_blend;

	// Temporarily stored until the model has been initialized, then deleted
	DirectX::XMFLOAT4 * m_pColors;

	// Currently not implemented - will cause linker errors if called
private:
	SphereModel(const SphereModel& other);
	SphereModel& operator=(const SphereModel& other);
};