/*oct_tree.h
 *
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
  *it, as it "moves", will apply the negation of its movement to every other object in the world
  *so the OctTree will not move and neither will the player but everything else will
  *this simplifies the updating of the OctTree by not requiring mass culling and additions to the tree
*/
#pragma once

#include "ObjectModel.h"
#include "Oct_node.h"
#include <vector>
#include <algorithm>

using namespace std;

class Octtree{
	public:
		Octtree(XMFLOAT3 position, float length, int depth);
		~Octtree();
		
		int addObject(ObjectModel * newGameObject);
		
		virtual HRESULT drawContents(ID3D11DeviceContext* const context, GeometryRendererManager& manager, Camera * camera);

		virtual HRESULT update(const DWORD currentTime, const DWORD updateTimeInterval);

	protected:
		Octnode * rootNode;
		int maxDepth;
		vector<ObjectModel *>* completeObjectList;
		
		virtual HRESULT refitting();
		
		int checkCollisions(vector<ObjectModel **>* outCollidingObjects);
		int checkCollisionsBetween(Octnode* node1, Octnode* node2, vector<ObjectModel **>* outCollisions);
		int checkCollisionsWithin(Octnode* node, vector<ObjectModel **>* outCollisions);
		int checkUpTree(Octnode* currNode, Octnode* checkNode, vector<ObjectModel**>* outcollisions);
		int traverseTreeDown(Octnode* node, vector<ObjectModel**>* outcollisions);
		int checkCollisionsRay(vector<ObjectModel *>* outColliding, XMFLOAT3 posRay, XMFLOAT3 dirRay);

};