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

int Octtree::checkCollisions(vector<ObjectModel **> outCollidingObjects){
	/*
	iterate through the tree starting at the deepest children and going up the tree checking for collisions
	*/
	traverseTreeDown(rootNode, outCollidingObjects);
	return 0;
}

int Octtree::checkCollisionsBetween(Octnode* node1, Octnode* node2, vector<ObjectModel **> outCollisions){
	//check all objects in the 2 passed nodes for collisions and pass back a list of all the collisions
	for (int i = 0; i < node1->nodeObjectList.size(); i++){
		for (int j = 0; j < node2->nodeObjectList.size(); j++){
			//sphere vs sphere collision checks
			//distace between the spheres
			float distBetween = sqrt(	pow(node1->nodeObjectList.at[i]->getBoundingOrigin().x - 
										node2->nodeObjectList.at[j]->getBoundingOrigin().x, 2) +
										pow(node1->nodeObjectList.at[i]->getBoundingOrigin().y -
										node2->nodeObjectList.at[j]->getBoundingOrigin().y, 2) +
										pow(node1->nodeObjectList.at[i]->getBoundingOrigin().z -
										node2->nodeObjectList.at[j]->getBoundingOrigin().z, 2)
				);
			//check if distance is less than the sum of the radiis
			if (distBetween < (	node1->nodeObjectList.at[i]->getBoundingRadius() +
								node2->nodeObjectList.at[j]->getBoundingRadius())){
				//if they are colliding add them to the list of pairs of collisions
				ObjectModel **newCollision = new ObjectModel*[2];
				newCollision[0] = node1->nodeObjectList.at[i];
				newCollision[1] = node2->nodeObjectList.at[j];
				outCollisions.push_back(newCollision);
			}
		}
	}
	return 0;
}

int Octtree::checkCollisionsWithin(Octnode* node, vector<ObjectModel **> outCollisions){
	//check for all possible collisions between this node and all other nodes
	for (int i = 0; i < node->nodeObjectList.size(); i++){
		for (int j = i; j < node->nodeObjectList.size(); j++){
			//sphere vs sphere collision checks
			//distace between the spheres
			float distBetween = sqrt(pow(node->nodeObjectList.at[i]->getBoundingOrigin().x -
				node->nodeObjectList.at[j]->getBoundingOrigin().x, 2) +
				pow(node->nodeObjectList.at[i]->getBoundingOrigin().y -
				node->nodeObjectList.at[j]->getBoundingOrigin().y, 2) +
				pow(node->nodeObjectList.at[i]->getBoundingOrigin().z -
				node->nodeObjectList.at[j]->getBoundingOrigin().z, 2)
				);
			//check if distance is less than the sum of the radiis
			if (distBetween < (node->nodeObjectList.at[i]->getBoundingRadius() +
				node->nodeObjectList.at[j]->getBoundingRadius())){
				//if they are colliding add them to the list of pairs of collisions
				ObjectModel **newCollision = new ObjectModel*[2];
				newCollision[0] = node->nodeObjectList.at[i];
				newCollision[1] = node->nodeObjectList.at[j];
				outCollisions.push_back(newCollision);
			}
		}
	}
	return 0;
}

int Octtree::checkUpTree(Octnode* currNode, Octnode* checkNode, vector<ObjectModel**> outcollisions){
	//check for all collisions between these nodes
	checkCollisionsBetween(currNode, checkNode, outcollisions);

	//so long as you didn't just check against the root check call this on the parent
	if (currNode->depthMe > 0){
		checkUpTree(currNode->parentNode, checkNode, outcollisions);
	}
	return 0;
}

int Octtree::traverseTreeDown(Octnode* node, vector<ObjectModel**> outcollisions){
	for (int i = 0; i < 8; i++){
		checkUpTree(node, node->children[i], outcollisions);
		checkCollisionsWithin(node, outcollisions);
		traverseTreeDown(node->children[i], outcollisions);
	}
	return 0;
}