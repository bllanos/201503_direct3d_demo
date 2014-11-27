/* galleonShipModel.h

*/


#pragma once

#include <DirectXMath.h>
#include "Transformable.h"
#include "SphereModel.h"
#include "CubeModel.h"
#include "LogUser.h"
#include "../../oct_tree.h"
#include "../../ObjectModel.h"
#include "IGeometry.h"
#include <string>

// Default log message prefix used before more information is available
#define GALLEONSHIPMODEL_START_MSG_PREFIX L"GalleonShipModel "

/* The following definitions are:
-Key parameters used to retrieve configuration data
-Default values used in the absence of configuration data
or constructor/function arguments (where necessary)
*/

#define GALLEONSHIPMODEL_SCOPE L"GalleonShipModel"

/* LogUser and ConfigUser configuration parameters
Refer to LogUser.h and ConfigUser.h
*/
#define GALLEONSHIPMODEL_LOGUSER_SCOPE		L"GalleonShipModel_LogUser"
#define GALLEONSHIPMODEL_CONFIGUSER_SCOPE		L"GalleonShipModel_ConfigUser"

class GalleonShipModel : public IGeometry, public LogUser
{
	// Initialization and destruction
public:

	GalleonShipModel();
	virtual ~GalleonShipModel(void);

	//HRESULT spawn(Octtree*);
	HRESULT initialize(ID3D11Device* d3dDevice, vector<Transformable*>* bones);
	// body is root transform
	vector<Transformable*>* m_bones;

	virtual HRESULT drawUsingAppropriateRenderer(
		ID3D11DeviceContext* const context,
		GeometryRendererManager& manager,
		const Camera* const camera
		) override;

	virtual HRESULT setTransformables(const std::vector<Transformable*>* const transforms) override;
	virtual XMFLOAT3 getPosition() override;
	virtual float getRadius() override;

private:
	CubeModel* body;

	CubeModel* leftWing;
	CubeModel* rightWing;

	SphereModel* bodyFront;
	SphereModel* bodyBack;

	GalleonShipModel(const GalleonShipModel& other);
	GalleonShipModel& operator=(const GalleonShipModel& other);
};