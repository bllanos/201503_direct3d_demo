#include "oct_tree.h"
#include "ObjectModel.h"

Octtree::~Octtree(){
	delete rootNode;
	for (int i = 0; i < completeObjectList.size(); i++){
		delete completeObjectList.at[i];
	}
}

Octtree::Octtree(XMFLOAT3 position, float length, int depth){
	rootNode = new Octnode(position, length, depth, 0, NULL);
	completeObjectList = vector<ObjectModel *>();
}

int Octtree::advance(){
	//TODO: make the objects in the tree do stuff
	//this will also handle the changing between the nodes
	return 0;
}

int Octtree::addObject(ObjectModel * newGameObject){
	//start a recursive call to put the object in the tree
	if(rootNode->fits(newGameObject) == 0){
		//only add the object to the list if it is put into the list at some point
		completeObjectList.push_back(newGameObject);
		return 0;
	}
	//something went wrong and the object doesn't fit anywhere in the tree
	return -1;
}

int Octtree::checkCollisions(vector<ObjectModel *> outCollidingObjects){
	return 0;
}