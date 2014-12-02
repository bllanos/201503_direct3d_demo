/*
splineParticlesVS.hlsl
-----------------------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created November 23, 2014

Primary basis: skinnedColorVS_noLight.hlsl
  and generalParticlesVS.hlsl

Description
  -Cubic Bezier curve spline evaluation,
     in addition to the behaviour in generalParticlesVS.hlsl
  -

Notes
  -The spline parameter, 't', is defined in the range 0 to 1.
     - 0 = start of the first segment
	 - 1 = end of the segment at index (spline capacity - 1)
     -Particles will have 't' values that start from
	    an initial value between 0 and 1, and wrap around at 1,
		not at a value corresponding to the index of the last valid segment in the spline.
*/

cbuffer CameraProperties : register(cb0) {
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 cameraPosition;
};

cbuffer Globals : register(cb1) {
	matrix worldMatrix;
	float4 blendAmountAndColorCast;
	/* x = current time (milliseconds)
	   y = update time interval (milliseconds)
	   z = current number of valid segments in the spline
	   w = spline capacity (maximum number of valid segments)
	 */
	float4 timeAndSplineParameters;
};

// See vertexTypes.h for details
struct VSInput {
	float3 position : POSITION;
	float3 billboard : BILLBOARD;
	float4 linearVelocity : LINEAR_VELOCITY;
	float4 life : LIFE;
	float4 index : INDEX;
};

struct VSOutput {
	float3 positionVS : POSITION_VIEW; // View space
	float2 billboard : BILLBOARD_WH; // Billboard dimensions (width, height)
	float angle : ANGLE; // Calculated based on direction, view direction, and rotation speed
	float3 life : LIFE; // (current age, current health, decay factor)
	float4 index : INDEX; // Same as input vertex
};

/* Cubic Bezier curve segment control points */
struct Segment {
	float4 p0;
	float4 p1;
	float4 p2;
	float4 p3;
};

StructuredBuffer<Segment> Spline : register(t0);

VSOutput VSMAIN(in VSInput input) {
	VSOutput output;

	// Compute age and health
	// ----------------------
	float time = timeAndSplineParameters.x;
	float age = max(0.0f, time - input.life.x); // If negative, particle has not yet been born.
	float health = input.life.y - ((input.life.z * age) % input.life.y);
	// Recompute age based on health (wrap around)
	age = (input.life.y - health) / input.life.z;
	if( health < input.life.w ) {
		health = 0.0f; // Below death cutoff
	}

	// Compute spline parameter and segment index
	float t = frac(input.position.x + (input.linearVelocity.z) * age); // Between 0 and 1
	float segmentIndex = t * timeAndSplineParameters.w;
	if (segmentIndex < 0.0f || segmentIndex >= timeAndSplineParameters.z) {
		health = 0.0f; // Out of bounds
	} else {

		// Compute base position by evaluating the spline
		// ----------------------------------------------
		float segmentT = frac(segmentIndex);
		Segment segment = Spline[(uint)segmentIndex];
		float3 splinePosition =
			(pow(1.0f - segmentT, 3)*segment.p0 +
			3.0f*segmentT*pow(1.0f - segmentT, 2)*segment.p1 +
			3.0f*pow(segmentT, 2)*(1.0f - segmentT)*segment.p2 +
			pow(segmentT, 3)*segment.p3).xyz;

		// Compute offset from base position and compute final position
		// ------------------------------------------------------------
		
		// Find the unit direction vector of the spline
		float3 splineDirection = normalize(
		(3.0f*pow(1.0f - segmentT, 2)*(segment.p1 - segment.p0) +
		6.0f*(1.0f - segmentT)*t*(segment.p2 - segment.p1) +
		3.0f*pow(segmentT, 2)*(segment.p3 - segment.p2)).xyz
			);

		// Pick somewhat arbitrary basis vectors to describe the plane normal to the spline
		float3 tangent1 = splinePosition - (dot(splinePosition, splineDirection) * splineDirection);
		// Avoid trying to normalize a zero vector
		if (!(any(tangent1))) {
			tangent1.x += 0.0001;
		}
		tangent1 = normalize(tangent1);
		float3 tangent2 = cross(tangent1, splineDirection);

		// Compute the offset radius
		float radius = input.position.y + (input.linearVelocity.x * age);

		// Calculate angle of the offset vector relative to 'tangent1'
		float angle = input.position.z + (input.linearVelocity.y * age);
		float3 offset = (tangent1 * cos(angle) + tangent2 * sin(angle));

		// Final position
		float4 inPosition = float4((radius * offset) + splinePosition, 1.0f);

		// Transform position into view space
		// ----------------------------------

		// World space
		inPosition = mul(inPosition, worldMatrix);
		offset = mul(float4(offset, 0.0f), worldMatrix).xyz; // Assuming uniform scaling (otherwise not correct)

		// View space
		output.positionVS = mul(inPosition, viewMatrix).xyz;
		offset = mul(float4(offset, 0.0f), viewMatrix).xyz; // Assuming uniform scaling (otherwise not correct)

		// Fill in the rest of the output values
		// -------------------------------------

		// Billboard
		output.billboard = input.billboard.xy;

		// Angular motion
		output.angle = input.billboard.z * age;

		// If direction of motion is away from viewer, reverse the direction of rotation
		float dotV_Vel = dot(offset, output.positionVS);
		if (dotV_Vel > 0.0f) {
			output.angle = -output.angle;
		}

		// Updated life information
		output.life.x = age;
		output.life.y = health;
		output.life.z = input.life.z;

		// Index - pass-through
		output.index = input.index;
	}
	return output;
}