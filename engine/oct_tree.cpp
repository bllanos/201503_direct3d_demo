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
	return 0;
}

int Octtree::checkCollisionsWithin(Octnode* node, vector<ObjectModel **>* outCollisions){
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

/*
	A note of this function, you will return the list of all the collisions including the object firing the laser
	so you have to handle the list of all the return
*/
int Octtree::checkCollisionsRay(vector<ObjectModel *>* outCollsion, XMFLOAT3 posRay, XMFLOAT3 dirRay){
	return -1;
}