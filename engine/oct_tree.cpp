#include "oct_tree.h"
#include "ObjectModel.h"

Octtree::Octtree(XMFLOAT3 position, float length, int depth){
	maxDepth = depth;
	rootNode = new Octnode(position, length, depth, 0, NULL);
	completeObjectList = vector<ObjectModel *>();
}

Octtree::~Octtree(){
	delete rootNode;
	for (int i = 0; i < completeObjectList.size(); i++){
		//delete completeObjectList.at[i];
	}
}

int Octtree::advance(){
	//Will register all the movement of objects and update them and their location in the tree
	//Pay particular attention to the fact that all objects with exception the ship are moving
	//Apply the negation of the ships movement to ALL objects in the oct tree
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
	/*
	
	*/
	return 0;
}

int Octtree::checkCollisionsBetween(Octnode* node1, Octnode* node2, vector<ObjectModel *> outCollisions){
	for (int i = 0; i < node1->nodeObjectList.size(); i++){

	}
	return 0;
}