/*
GalleonShipModel.cpp

*/

#include "GalleonShipModel.h"

using namespace DirectX;

GalleonShipModel::GalleonShipModel()
: IGeometry(),
LogUser(true, GALLEONSHIPMODEL_START_MSG_PREFIX),
body(0), leftWing(0), rightWing(0), bodyFront(0), bodyBack(0)
{
}

GalleonShipModel::~GalleonShipModel(void)
{
	if (body != 0) {
		delete body;
		body = 0;
	}

	if (leftWing != 0) {
		delete leftWing;
		leftWing = 0;
	}

	if (rightWing != 0) {
		delete rightWing;
		rightWing = 0;
	}

	if (bodyFront != 0){
		delete bodyFront;
		bodyFront = 0;
	}

	if (bodyBack != 0){
		delete bodyBack;
		bodyBack = 0;
	}
}

HRESULT GalleonShipModel::initialize(ID3D11Device* d3dDevice, vector<Transformable*>* bones)
{
	m_bones = bones;

	//rootTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-10.0f, -10.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	body = new CubeModel(m_bones->at(0),
		1.0f, 0.75f, 4.0f, 0);

	//Transformable* wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f));
	leftWing = new CubeModel(m_bones->at(1),
		1.0f, 0.75f, 3.0f, 0);

	//wingTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, -0.5f, 0.0f, 1.0f));
	rightWing = new CubeModel(m_bones->at(2),
		1.0f, 0.75f, 3.0f, 0);

	//Transformable* sphereBodyTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bodyFront = new SphereModel(m_bones->at(3), 1, 0);

	//sphereBodyTransform = new Transformable(XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bodyBack = new SphereModel(m_bones->at(4), 1, 0);

	if (FAILED(body->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(bodyFront->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(bodyBack->initialize(d3dDevice))) {
		logMessage(L"Failed to initialize CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	leftWing->setParentTransformable(bones->at(0));
	rightWing->setParentTransformable(bones->at(0));
	bodyFront->setParentTransformable(bones->at(0));
	bodyBack->setParentTransformable(bones->at(0));
	
	return ERROR_SUCCESS;
}
/*
HRESULT GalleonShipModel::spawn(Octtree* octtree)
{
	ObjectModel* galleonShipObject = new ObjectModel(this);
	galleonShipObject->addTransformable(body->getTransformable());
	galleonShipObject->addTransformable(leftWing->getTransformable());
	galleonShipObject->addTransformable(rightWing->getTransformable());
	galleonShipObject->addTransformable(bodyFront->getTransformable());
	galleonShipObject->addTransformable(bodyBack->getTransformable());

	if (octtree->addObject(galleonShipObject) == -1){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}
*/
HRESULT GalleonShipModel::drawUsingAppropriateRenderer(
	ID3D11DeviceContext* const context,
	GeometryRendererManager& manager,
	const Camera* const camera
	)
{
	if (FAILED(body->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(leftWing->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(rightWing->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(bodyFront->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	if (FAILED(bodyBack->drawUsingAppropriateRenderer(context, manager, camera))) {
		logMessage(L"Failed to draw CubeModel object.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	return ERROR_SUCCESS;
}

HRESULT GalleonShipModel::setTransformables(const std::vector<Transformable*>* const transforms)
{
	if (transforms == 0){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	if (transforms->size() != static_cast<std::vector<Transformable*>::size_type>(5)){
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_NULL_INPUT);
	}

	vector<Transformable*>* tform = new vector<Transformable*>();
	tform->push_back(transforms->at(0));

	if (FAILED(body->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(1));

	if (FAILED(leftWing->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(2));

	if (FAILED(rightWing->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(3));

	if (FAILED(bodyFront->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;

	tform = new vector<Transformable*>();
	tform->push_back(transforms->at(4));

	if (FAILED(bodyBack->setTransformables(tform))) {
		logMessage(L"Failed to set CubeModel object transforms.");
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}

	delete tform;
	return ERROR_SUCCESS;
}

XMFLOAT3 GalleonShipModel::getPosition()
{
	return XMFLOAT3(0,0,0);
}

float GalleonShipModel::getRadius()
{
	return 2.0f;
}