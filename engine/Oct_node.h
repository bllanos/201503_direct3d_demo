/*
 *Mark Wilkes 100884169
 *
 *Oct_node are the nodes for the oct-tree
 *
 *the vertices are order in the following order:
	*0th vertex is front, top, left
	*1st vertex is front, bottom, left
	*2nd vertex is front, top, right
	*3rd vertex is front, bottom, right
	*4th vertex is back, top, left
	*5th vertex is back, bottom, left
	*6th vertex is back, top, right
	*7th vertex is back, bottom, right 
 */
#pragma once

#include <vector>
#include "ObjectModel.h"
#include "Oct_tree.h"

using namespace std;
 
class Octnode{
	friend class Octtree;
 
	public:
		Octnode(XMFLOAT3 position, float length, int depth, int depthThis, Octnode * parent);
		~Octnode();
		
		int fits(ObjectModel * newGameObject);
		
	protected:
		Octnode* children[8];
		vector<ObjectModel *> nodeObjectList;
		Octnode * parentNode;
		XMFLOAT3 origin;
		float length;
		int depthMe;
		XMFLOAT3 vertices[8];
		
		bool spherePlaneCheck(XMFLOAT3 planePoints[3], XMFLOAT3 sphereOrigin, float squareLength, float sphereRadi);
};