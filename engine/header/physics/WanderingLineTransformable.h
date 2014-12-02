/*
WanderingLineTransformable.h
----------------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 26, 2014

Primary basis: Transformable.h

Description
  -A Transformable which positions itself along a line
     joining two other Transformables.
  -Additionally, a random offset and non-random, cycling orientation
     can be applied to make the Transformable less
	 tightly coupled to the line.
*/

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <random>
#include "Transformable.h"

class WanderingLineTransformable : public Transformable {

public:
	struct Parameters {
		/* Linear interpolation parameter for interpolation between the start
		   and end Transformables. Must be in the range [0, 1]
		 */
		float t;

		/* Maximum allowed drift distance from the linearly interpolated position,
		   when t = 1/2, as passed to the constructor.
		   Actual allowed drift distance will be calculated as
		     maxRadius * (1.0 - 2.0f * abs(t - 0.5))
		 */
		float maxRadius;

		/* Amount of drift per millisecond
		   (Length of drift vector which is sampled uniformly at random from
		    the surface of a sphere)
		 */
		float linearSpeed;

		/* Maximum allowed orientation deviation from the line connecting
		   the start and end Transformables, when t = 1/2, as passed to the constructor.
		   Actual allowed orientation deviation will
		   be calculated (per component) as
		     maxRollPitchYaw * (1.0 - 2.0f *abs(t - 0.5))
		 */
		DirectX::XMFLOAT3 maxRollPitchYaw;

		// Amount of rotational drift per millisecond
		DirectX::XMFLOAT3 rollPitchYawSpeeds;
	};

public:
	/* If 'parameters.t' is not in the range [0,1], an exception
	   will be thrown.
	 */
	WanderingLineTransformable(Transformable* const start,
		Transformable* const end, const Parameters& parameters);

	virtual ~WanderingLineTransformable(void);

	/* Calls refresh(), then calls the base class version of this function.
	 */
	virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval) override;

	/* Changes the endpoints of the line used for interpolation,
	   then calls refresh().
	 */
	HRESULT setEndpoints(Transformable* const start,
		Transformable* const end);

protected:

	/* Re-interpolates position and scaling,
	   then updates offsets and calculates final position and orientation.
	 */
	virtual HRESULT refresh(const DWORD updateTimeInterval);

	// Data members
private:
	DirectX::XMFLOAT3 m_offset; // Positional offset
	DirectX::XMFLOAT3 m_rollPitchYaw; // Rotational offset

	Parameters m_parameters;

	Transformable* m_start; // Shared - not deleted by destructor

	Transformable* m_end; // Shared - not deleted by destructor

	bool m_rollPitchYawDirection[3];

private:
	static std::default_random_engine s_generator;
	static std::uniform_real_distribution<float> s_offsetDistribution;

	// Currently not implemented - will cause linker errors if called
private:
	WanderingLineTransformable(const WanderingLineTransformable& other);
	WanderingLineTransformable& operator=(const WanderingLineTransformable& other);
};