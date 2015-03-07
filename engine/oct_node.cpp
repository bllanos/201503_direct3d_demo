#include "oct_node.h"
#include "ObjectModel.h"
#include "DirectXMath.h"

Octnode::Octnode(XMFLOAT3 position, float length, int depth, int depthThis, Octnode * parent){
	nodeObjectList = new vector<ObjectModel *>();
	
	this->length = length;

	depthMe = depthThis;

	depthMax = depth;
	
	origin = position;
	
	parentNode = parent;
	
	if(depthMe == depth){
		for(int i=0; i < 8; i++){
			children[i] = NULL;
		}
	}
	else{
		//create the children of this node
		children[0] = new Octnode(XMFLOAT3(position.x, 				position.y, 			position.z), 			length/2, depth, depthMe+1, this);
		children[1] = new Octnode(XMFLOAT3(position.x, 				position.y-length/2, 	position.z), 			length/2, depth, depthMe+1, this);
		children[2] = new Octnode(XMFLOAT3(position.x+length/2, 	position.y, 			position.z), 			length/2, depth, depthMe+1, this);
		children[3] = new Octnode(XMFLOAT3(position.x+length/2, 	position.y-length/2, 	position.z), 			length/2, depth, depthMe+1, this);
		children[4] = new Octnode(XMFLOAT3(position.x, 				position.y, 			position.z+length/2), 	length/2, depth, depthMe+1, this);
		children[5] = new Octnode(XMFLOAT3(position.x, 				position.y-length/2, 	position.z+length/2), 	length/2, depth, depthMe+1, this);
		children[6] = new Octnode(XMFLOAT3(position.x+length/2, 	position.y, 			position.z+length/2),	length/2, depth, depthMe+1, this);
		children[7] = new Octnode(XMFLOAT3(position.x+length/2, 	position.y-length/2, 	position.z+length/2), 	length/2, depth, depthMe+1, this);
	}
	
	//set the vertex positions of this node
	vertices[0] = XMFLOAT3(position.x,			position.y,			position.z);
	vertices[1] = XMFLOAT3(position.x, 			position.y-length, 	position.z);
	vertices[2] = XMFLOAT3(position.x+length, 	position.y, 		position.z);
	vertices[3] = XMFLOAT3(position.x+length, 	position.y-length, 	position.z);
	vertices[4] = XMFLOAT3(position.x, 			position.y, 		position.z+length);
	vertices[5] = XMFLOAT3(position.x, 			position.y-length, 	position.z+length);
	vertices[6] = XMFLOAT3(position.x+length, 	position.y, 		position.z+length);
	vertices[7] = XMFLOAT3(position.x+length, 	position.y-length, 	position.z+length);
	
}

Octnode::~Octnode(){
	//delete nodeObjectList;
	for (std::vector<ObjectModel*>::size_type i = 0; i < nodeObjectList->size(); i++){
		delete nodeObjectList->at(i);
		(*nodeObjectList)[i] = 0;
	}
	delete nodeObjectList;
	nodeObjectList = 0;
	for (size_t i = 0; i < 8; ++i) {
		delete children[i];
		children[i] = 0;
	}
}

int Octnode::fits(ObjectModel * newGameObject){
	// Add it to this node
	nodeObjectList->push_back(newGameObject);
	return 0;
}

/*
return int is a marker of if something must be done to find the new home for this object
0 = it doesn't need to move
-1 = something does need to happen to fit it in again
*/
int Octnode::refit(ObjectModel * gameObject){
	return 0;
}

HRESULT Octnode::checkObjectUpdates(vector<ObjectModel*>* outRefit){
	for (size_t i = 0; i < nodeObjectList->size(); i++){
		if (refit((*nodeObjectList)[i]) == -1){
			outRefit->push_back((*nodeObjectList)[i]);
			(*nodeObjectList)[i] = 0;
			nodeObjectList->erase(nodeObjectList->begin() + i);
		}
	}

	for (size_t j = 0; j < 8; j++){
		if (children[j] != NULL){
			children[j]->checkObjectUpdates(outRefit);
		}
	}

	return ERROR_SUCCESS;
}

bool Octnode::spherePlaneCheck(XMFLOAT3 planePoints[3] , XMFLOAT3 sphereOrigin, float squareLength, float sphereRadi){

	//first vector 0th and 2nd
	
	XMFLOAT3 a(	planePoints[2].x - planePoints[0].x, 
				planePoints[2].y - planePoints[0].y, 
				planePoints[2].z - planePoints[0].z);
	
	//second vector 0th and 1st
	
	XMFLOAT3 b(	planePoints[1].x - planePoints[0].x, 
				planePoints[1].y - planePoints[0].y, 
				planePoints[1].z - planePoints[0].z);
							 
	//cross product of the vectors
	//a = (ay * bz - az * by)
	//b = (az * bx - ax * bz)
	//c = (ax * by - ay * bx)
	
	XMFLOAT3 firstCrossSecond(	(a.y * b.z) - (a.z * b.y),
								(a.z * b.x) - (a.x * b.z),
								(a.x * b.y) - (a.y * b.x));
							  
	//d value of the plane
	float planeD = 0-(	(firstCrossSecond.x * planePoints[0].x) + 
						(firstCrossSecond.y * planePoints[0].y) + 
						(firstCrossSecond.z * planePoints[0].z));
	
	// distance between plane and origin of the object
	// P(x,y,z) Plane(ax + by + cz = d)
	// Distance = |a * P(x) + b * P(y) + c * P(z) - d|/(sqrt((a*a) + (b*b) + (c*c))
	
	//top half
	float distPlanePoint = ( (	(firstCrossSecond.x * sphereOrigin.x) + 
								(firstCrossSecond.y * sphereOrigin.y) +
								(firstCrossSecond.z * sphereOrigin.z)) + planeD);
	
	//absolute value of top half
	
	distPlanePoint = abs(distPlanePoint);
	
	//divide top half by bottom half
	
	distPlanePoint = distPlanePoint / sqrt(	(firstCrossSecond.x * firstCrossSecond.x) +
											(firstCrossSecond.y * firstCrossSecond.y) +
											(firstCrossSecond.z * firstCrossSecond.z));
	
	//check bounding

	if (distPlanePoint < sphereRadi) return false;

	if (distPlanePoint + sphereRadi > squareLength) return false;

	return true;
}