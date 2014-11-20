#include "oct_tree.h"
#include "ObjectModel.h"

Octtree::Octtree(XMFLOAT3 position, float length, int depth){
	maxDepth = depth;
	rootNode = new Octnode(position, length, depth, 0, NULL);
	completeObjectList = new vector<ObjectModel *>();
}

Octtree::~Octtree(){
	delete rootNode;
	/*
	for (std::vector<ObjectModel*>::size_type i = 0; i < completeObjectList->size(); i++){
		delete completeObjectList->at(i);
	}
	*/
	if (completeObjectList != 0){
		delete completeObjectList;
		completeObjectList = 0;
	}
}

int Octtree::addObject(ObjectModel * newGameObject){
	//start a recursive call to put the object in the tree
	if(rootNode->fits(newGameObject) == 0){
		//only add the object to the list if it is put into the list at some point
		completeObjectList->push_back(newGameObject);
		return 0;
	}
	//something went wrong and the object doesn't fit anywhere in the tree
	return -1;
}

int Octtree::checkCollisions(vector<ObjectModel **>* outCollidingObjects){
	/*
	iterate through the tree starting at the deepest children and going up the tree checking for collisions
	*/
	traverseTreeDown(rootNode, outCollidingObjects);
	return 0;
}

int Octtree::checkCollisionsBetween(Octnode* node1, Octnode* node2, vector<ObjectModel **>* outCollisions){
	//check all objects in the 2 passed nodes for collisions and pass back a list of all the collisions
	for (std::vector<ObjectModel*>::size_type i = 0; i < node1->nodeObjectList->size(); i++){
		for (std::vector<ObjectModel*>::size_type j = 0; j < node2->nodeObjectList->size(); j++){
			//sphere vs sphere collision checks
			//distace between the spheres
			float distBetween = sqrt(	pow(node1->nodeObjectList->at(i)->getBoundingOrigin().x - 
										node2->nodeObjectList->at(j)->getBoundingOrigin().x, 2) +
										pow(node1->nodeObjectList->at(i)->getBoundingOrigin().y -
										node2->nodeObjectList->at(j)->getBoundingOrigin().y, 2) +
										pow(node1->nodeObjectList->at(i)->getBoundingOrigin().z -
										node2->nodeObjectList->at(j)->getBoundingOrigin().z, 2)
				);
			//check if distance is less than the sum of the radiis
			if (distBetween < (	node1->nodeObjectList->at(i)->getBoundingRadius() +
								node2->nodeObjectList->at(j)->getBoundingRadius())){
				//if they are colliding add them to the list of pairs of collisions
				ObjectModel **newCollision = new ObjectModel*[2];
				newCollision[0] = node1->nodeObjectList->at(i);
				newCollision[1] = node2->nodeObjectList->at(j);
				outCollisions->push_back(newCollision);
			}
		}
	}
	return 0;
}

int Octtree::checkCollisionsWithin(Octnode* node, vector<ObjectModel **>* outCollisions){
	//check for all possible collisions between this node and all other nodes
	for (std::vector<ObjectModel*>::size_type i = 0; i < node->nodeObjectList->size(); i++){
		for (std::vector<ObjectModel*>::size_type j = i; j < node->nodeObjectList->size(); j++){
			//sphere vs sphere collision checks
			//distace between the spheres
			float distBetween = sqrt(pow(node->nodeObjectList->at(i)->getBoundingOrigin().x -
				node->nodeObjectList->at(j)->getBoundingOrigin().x, 2) +
				pow(node->nodeObjectList->at(i)->getBoundingOrigin().y -
				node->nodeObjectList->at(j)->getBoundingOrigin().y, 2) +
				pow(node->nodeObjectList->at(i)->getBoundingOrigin().z -
				node->nodeObjectList->at(j)->getBoundingOrigin().z, 2)
				);
			//check if distance is less than the sum of the radiis
			if (distBetween < (node->nodeObjectList->at(i)->getBoundingRadius() +
				node->nodeObjectList->at(j)->getBoundingRadius())){
				//if they are colliding add them to the list of pairs of collisions
				ObjectModel **newCollision = new ObjectModel*[2];
				newCollision[0] = node->nodeObjectList->at(i);
				newCollision[1] = node->nodeObjectList->at(j);
				outCollisions->push_back(newCollision);
			}
		}
	}
	return 0;
}

int Octtree::checkUpTree(Octnode* currNode, Octnode* checkNode, vector<ObjectModel**>* outcollisions){
	//check for all collisions between these nodes
	checkCollisionsBetween(currNode, checkNode, outcollisions);

	//so long as you didn't just check against the root check call this on the parent
	if (currNode->depthMe > 0){
		checkUpTree(currNode->parentNode, checkNode, outcollisions);
	}
	return 0;
}

int Octtree::traverseTreeDown(Octnode* node, vector<ObjectModel**>* outcollisions){
	for (int i = 0; i < 8; i++){
		checkUpTree(node, node->children[i], outcollisions);
		checkCollisionsWithin(node, outcollisions);
		traverseTreeDown(node->children[i], outcollisions);
	}
	return 0;
}


HRESULT Octtree::drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera) {
	/*
	if (FAILED(m_tree->drawUsingAppropriateRenderer(context, manager, m_camera))) {
	logMessage(L"Failed to render Tree of game objects.");
	return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
	}
	*/
	for (std::vector<ObjectModel*>::size_type i = 0; i < completeObjectList->size(); i++){
		if (FAILED((*completeObjectList)[i]->draw(context, manager, camera))){
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}
	return ERROR_SUCCESS;
}

HRESULT Octtree::update(const DWORD currentTime, const DWORD updateTimeInterval) {
	for (std::vector<ObjectModel*>::size_type i = 0; i < completeObjectList->size(); i++){
		if (FAILED(((*completeObjectList)[i]->updateContainedTransforms(currentTime, updateTimeInterval)))){
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_BL_ENGINE, ERROR_FUNCTION_CALL);
		}
	}

	return refitting();
}

HRESULT Octtree::refitting(){
	vector<ObjectModel*>* newList = new vector<ObjectModel*>();

	HRESULT result = ERROR_SUCCESS;

	//call the recursive function to find all the objects that don't fit in the node any more
	rootNode->checkObjectUpdates(newList);

	//find them a new home or they don't need to be around anymore
	for (size_t i = 0; i < newList->size(); i++){
		//can't fit it anymore so it needs to get gone
		if (rootNode->fits((*newList)[i]) == -1){
			//find the not so fitting object in the master list
			vector<ObjectModel*>::iterator loc = find(completeObjectList->begin(), completeObjectList->end(), (*newList)[i]);
			//make sure we found it in case of some magic happening (read as something breaking and going unnoticed)
			if (loc != completeObjectList->end()){
				 completeObjectList->erase(loc);
			}
			else{
				result = ERROR_DATA_NOT_FOUND;
			}
		}
	}

	delete newList;

	return result;
}

int Octtree::checkCollisionsRay(vector<ObjectModel *>* outCollsion, XMFLOAT3 posRay, XMFLOAT3 dirRay){
	int result = -1;
	
	for (size_t i = 0; i < completeObjectList->size(); i++){
		XMFLOAT3 p = XMFLOAT3	(posRay.x - completeObjectList->at(i)->getBoundingOrigin().x,
								posRay.y - completeObjectList->at(i)->getBoundingOrigin().y,
								posRay.z - completeObjectList->at(i)->getBoundingOrigin().z);
		float b = -((p.x * dirRay.x) + (p.y * dirRay.y) + (p.z * dirRay.z));
		float det = b*b - ((p.x * p.x)+(p.y * p.y)+(p.z * p.z)) + pow(completeObjectList->at(i)->getBoundingRadius(),2);

		if (det < 0) continue;
		det = sqrt(det);
		float t1 = b - det;
		float t2 = b + det;

		if (t2 < 0) continue;
		outCollsion->push_back(completeObjectList->at(i));
		result = 0;
	}
	
	return result;
}