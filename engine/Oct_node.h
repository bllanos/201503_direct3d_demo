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
 
 #include <vector>
 
 using namespace std;
 
 template<typename T>;
 
 class Octnode{
	friend class Octtree;
 
	public:
		Octnode(XMFLOAT3 position, float length, int depth, int depthThis, Octnode * parent);
		~Octnode();
		
		int fits(T * newGameObject);
		
	protected:
		Octnode[8] * children;
		vector<T*> nodeObjectList;
		Octnode * parentNode;
		XMFLOAT3 origin;
		float length;
		int depthMe;
		XMFLOAT3[8] vertices;
		
		bool spherePlaneCheck(XMFLOAT3[3] planePoints, XMFLOAT3 sphereOrigin, float squareLength, float sphereRadi);
 }