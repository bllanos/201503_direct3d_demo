/*
SplineParticles.h
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 22, 2014

Primary basis: InvariantTexturedParticles.h and SkinnedColorGeometry.h

Description
  -Extends the InvariantTexturedParticles class to use a structured buffer
     of spline control points.
  -In the shader, particle positions should be computed first based on the spline,
     and then transformed using this object's current Transformable.
  -This class does not update or otherwise manage the Spline
     object to which it refers.
*/

#pragma once

#include "InvariantTexturedParticles.h"
#include "Spline.h"

class SplineParticles : public InvariantTexturedParticles {

	/* Proxying the ConfigUser constructors
	   for use by derived classes
	*/
protected:
	SplineParticles(const bool enableLogging, const std::wstring& msgPrefix,
		Usage usage);

	SplineParticles(const bool enableLogging, const std::wstring& msgPrefix,
		Config* sharedConfig);

	template<typename ConfigIOClass> SplineParticles(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	template<typename ConfigIOClass> SplineParticles(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

protected:

	/* The number of control points stored in the 'spline' parameter,
	   if it was at full capacity, will be used to set the size
	   of this object's control point buffer.
	 */
	virtual HRESULT initialize(ID3D11Device* const device,
		const  INVARIANTPARTICLES_VERTEX_TYPE* const vertices, const size_t nVertices,
		const Transformable* const transform,
		const Spline* const spline,
		const D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	/* Initializes the spline control point buffer
	   and spline-related data members.
	 */
	virtual HRESULT initializeSplineData(ID3D11Device* const device,
		const Spline* const spline);

	/* Objects of this class can use renderers
	   corresponding to enumeration constants
	   'SplineParticlesRendererNoLight' and
	   'SplineParticlesRendererLight'.

	  This function must be called at least once, with a valid argument,
	  before the object can be rendered for the first time.
	 */
	virtual HRESULT setRendererType(const GeometryRendererManager::GeometryRendererType type) override;

	/* Calls the base class's version of this function,
	   then calls setRendererType() appropriately.
	 */
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0) override;

public:
	virtual ~SplineParticles(void);

	virtual HRESULT drawUsingAppropriateRenderer(ID3D11DeviceContext* const context, GeometryRendererManager& manager, const Camera* const camera) override;

	/* Proxy of the corresponding function in the Spline class.
	   To be used to set constant buffer parameters during rendering.
	 */
	size_t getNumberOfSegments(const bool capacity) const;

	/* The new spline must have the same capacity as the spline
	   passed to initialize().
	*/
	virtual HRESULT setSpline(const Spline* const spline);

protected:
	/* Prepares spline control point data on the pipeline
	   prior to rendering.
	 */
	virtual HRESULT updateAndBindSplineBuffer(ID3D11DeviceContext* const context);

	// Data members
private:
	/* Shared - Not deleted by the destructor */
	const Spline* m_spline;
	size_t m_splineCapacity;

	ID3D11Buffer *m_splineBuffer;
	ID3D11ShaderResourceView *m_splineBufferView;

	// Currently not implemented - will cause linker errors if called
private:
	SplineParticles(const SplineParticles& other);
	SplineParticles& operator=(const SplineParticles& other);
};

template<typename ConfigIOClass> SplineParticles::SplineParticles(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const Config* locationSource,
	const std::wstring filenameScope,
	const std::wstring filenameField,
	const std::wstring directoryScope,
	const std::wstring directoryField
	) :
	InvariantTexturedParticles(
	enableLogging,
	msgPrefix,
	optionalLoader,
	locationSource,
	filenameScope,
	filenameField,
	directoryScope,
	directoryField
	),
	m_spline(0), m_splineCapacity(0),
	m_splineBuffer(0), m_splineBufferView(0)
{}

template<typename ConfigIOClass> SplineParticles::SplineParticles(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	InvariantTexturedParticles(
	enableLogging,
	msgPrefix,
	optionalLoader,
	filename,
	path
	),
	m_spline(0), m_splineCapacity(0),
	m_splineBuffer(0), m_splineBufferView(0) 
{}