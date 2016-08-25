#ifndef _INCLUDED_TREE_HPP
#define _INCLUDED_TREE_HPP
// Tree



#include <functional>


#include "core.hpp"
#include "math.hpp"
#include "gfx.hpp"


namespace tree {



struct TreeConfigs {
	/// With full energy, how long is a branch
	float baseLength;
	/// The width to height ratio (0.2)
	float widthHeightRatio;
	/// The maximum number of divisions
	float maxEnergy;
	/// when a branch happens, how much energy goes to it
	float branchEnergyRatio;
	//float lengthVariance; // how much variance does length have in a node (0=baseLength, 0.5= 0.5*baseLength -> 1.5*baseLength)
	//float branchTrunkRatio; // when a branch splits from the trunk, how much energy is sapped from the trunk to the branch (0.5 = even, 1 = all of it)
	//float skyCurve;			// the angle at which new nodes point upwards (0.0 - 0.999)
	//*childNodePrototype*	// when you can't create more nodes, what happens (generate a flower or something)
};









class Node {
public:
	typedef std::shared_ptr< Node > Handle;

	static Node::Handle create( const TreeConfigs& c, float e ) {
		auto node = Node::Handle(new Node(c, e, Vector3::Up, nullptr ));
		node->_self = node;
		return node;
	}

	static Node::Handle create( const TreeConfigs& c, float e, const Vector3& growthDir, Node::Handle parent ) {
		auto node = Node::Handle(new Node(c, e, growthDir, parent));
		node->_self = node;
		return node;
	}

	Node( const TreeConfigs& c, float e, const Vector3& growthDir, Node::Handle p )
		: _id (s_nextId++)
		, _configs(c)
		, _origin()
		, _growthVector(growthDir)
		, _width(0.0f)
		, _length(0.0f)
		, _energy(e)
		, _parent(p)
		, _self(nullptr) {

		_growthVector.normalize();

		// TODO: make the base relative to the maxEnergy
		_width = _length * _configs.widthHeightRatio;

		// If there's a parent, pull the node out in growthDir `length` units
		// from parent's origin
		if( _parent ) {
			_length = log10(e) * _configs.baseLength;
			_origin = _parent->_origin;
			_origin += growthDir * _length;
		}
	}

	void output_r() const {
		output_r(0);
	}

	void output_r( size_t indent ) const {
		const size_t indentBufferSize=512;
		char indentBuffer[indentBufferSize];
		memset(indentBuffer, 0, indentBufferSize);
		for( size_t i=0; i< indent*2; i++) {
			indentBuffer[i] = ' ';
		}

		printf("%s{\n", indentBuffer );
		printf("%s  id: %zu\n", indentBuffer, _id);
		printf("%s  origin: [%.2f, %.2f, %.2f]\n", indentBuffer, _origin.x, _origin.y, _origin.z);
		//printf("%s  width: %f\n", indentBuffer, _width);
		printf("%s  length: %f\n", indentBuffer, _length);
		printf("%s  energy: %f\n", indentBuffer, _energy);
		printf("%s  children: [", indentBuffer);
		auto child = _children.begin();
		while ( child != _children.end() ) {
			(*child)->output_r(indent+1);
			child++;
		}
		printf("%s  ]\n", indentBuffer);
		printf("%s}\n", indentBuffer);
	}

	size_t countChildren_r() const {
		size_t cnt = _children.size();
		auto c = _children.begin();
		while( c != _children.end()) {
			cnt += (*c)->countChildren_r();
			c++;
		}
		return cnt;
	}

	void generateChildren_r ( float energy ) {
		// TODO: Try a gemetric approach
		float childEnergy = energy - 1.0f;
		if( childEnergy <= 1 ) {
			return;
		}

		// Pick a growth vector for the child
		Vector3 growthVector(Vector3::Up);
		if( _parent ) {
			growthVector = _origin - _parent->_origin;
		}

		// TODO: Add some wiggle to the growth vector

		// Make the main trunk
		Node::Handle trunk( Node::create( _configs, childEnergy, growthVector, _self ));
		_children.push_back( trunk );

		{
			// Make the branch that runs off the side
			growthVector = growthVector.cross( Vector3::random() );
			Node::Handle branch( Node::create( _configs, childEnergy * _configs.branchEnergyRatio, growthVector, _self ));
			_children.push_back( branch );
			branch->generateChildren_r( branch->_energy );
		}

		// Now work our way up
		trunk->generateChildren_r( childEnergy );
	}


	void iterateAll_r( std::function<void(const Node::Handle)> callback ) {

		callback(_self);

		auto c = _children.begin();
		while( c != _children.end()) {

			(*c)->iterateAll_r(callback);

			c++;
		}
	}


	size_t getId() const {
		return _id;
	}

	const Vector3& getGrowthVector() const {
		return _growthVector;
	}

	const Vector3& getOrigin() const {
		return _origin;
	}

	float getLength() const {
		return _length;
	}

	float getWidth() const {
		return _width;
	}

private:
	/// The unique Id for this Node
	size_t _id;
	/// List of configuration settings for the node
	const TreeConfigs& _configs;
	/// The 3D placement
	Vector3 _origin;
	/// Normalized vector of growth
	Vector3 _growthVector;
	/// Thickness of the node
	float _width;
	/// Length of the branch from the parent
	float _length;
	/// Energy remaining
	float _energy;
	/// List of all children
	List<Node::Handle> _children;
	/// Parent node
	Node::Handle _parent;
	/// Itself
	Node::Handle _self;

	/// Next Id for all nodes generated - these should not be used as
	/// unique identifiers amond different generated flora
	static size_t s_nextId;
};

size_t Node::s_nextId = 1;





/// A representation of a tree - generates nodes, meshes, and (eventually) Lods.
class Tree : public GFXObject {
public:
	/// Handle for a tree object
	typedef std::shared_ptr< Tree > Handle;

	/// Randomly generates a tree given a set of configurations
	static Tree::Handle generate( int seed, const TreeConfigs& configs ) {
		Tree::Handle tree(new Tree(configs));

		tree->_rootNode = Node::create(configs, configs.maxEnergy );
		tree->_rootNode->generateChildren_r( configs.maxEnergy );

		tree->generateMesh();

		return tree;
	}


	Node::Handle getRootNode() {
		return _rootNode;
	}

	size_t countNodes() const {
		return _rootNode->countChildren_r() + 1;
	}

	virtual bool outputAsJson( String*& output ) const {
		*output = "";
		return true;
	}

private:
	const TreeConfigs& _configs;
	unsigned int _nodeCount;
	Node::Handle _rootNode;
	Mesh::Handle _mesh;

	Tree ( const TreeConfigs& configs )
		: _configs(configs) {
		// empty
	}

	void generateMesh() {
		// Create a new mesh
		_mesh = Mesh::createEmpty();

		// Create the call back iterator
		auto callback = std::bind(
			&Tree::createBranchFromNode, this, std::placeholders::_1 );

		// Iterate through them
		_rootNode->iterateAll_r(callback);
	}

	void createBranchFromNode( const Node::Handle node ) {
		// center of cube = origin + growthVector * (length/2)
		// scale = width, length, width
		// upvector = growthvector

		Vector3 centre = node->getOrigin();
		centre += node->getGrowthVector() * ( node->getLength() / 2 );
		Vector3 scale = Vector3(
			node->getWidth(), node->getLength(), node->getWidth() );

		_mesh->addCube( centre, node->getGrowthVector(), scale );
	}
};



}  // namespace tree

#endif // _INCLUDED_TREE_HPP
