/*
 *Mark Wilkes 100884169
 *
 *Oct-tree is an organizing system for objects in 3-d space
 *it as the name suggest is a base 8 where each of the children is a sector of the parent
 *the origin of each node is a representation of the top, front, left corner of the cube
 *the children are organized as follows:
 *
 *0th child is front, top, left
 *1st child is front, bottom, left
 *2nd child is front, top, right
 *3rd child is front, bottom, right
 *4th child is back, top, left
 *5th child is back, bottom, left
 *6th child is back, top, right
 *7th child is back, bottom, right
 *
 *this system uses recursive division of space to increase the precision of the space modelling
 */
 
 /*
  *the spacial system will move with the player's craft centred in the root's cube
  *so this means it will be located on the origin of the roots 7th child
  *it, as it "moves", will apply its movement to every other object in the world
  *so the origin points will not be constant and will change as the player moves
  */
#include "ObjectModel.h"
#include "oct_node.h"
#include <vector>

using namespace std;

class Octtree {
	public:
		Octtree();
		Octtree(XMFLOAT3 position, float length, int depth);
		~Octtree();
		
		int advance();
		int addObject(ObjectModel * newGameObject);
		
	protected:
		Octnode * rootNode;
		vector<ObjectModel *> completeObjectList;
		
		int checkCollisions(vector<ObjectModel *> outCollidingObjects);
}