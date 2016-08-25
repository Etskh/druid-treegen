#ifndef _INCLUDED_GFX_HPP
#define _INCLUDED_GFX_HPP

#include "core.hpp"
#include "math.hpp"

// GFX


// GFX


struct VertexPos {
	Vector3 pos;
	//Vector3 colour;
	//Vector3 uv;
};

struct Surface {
	Surface( size_t v0, size_t v1, size_t v2 ) {
		verts[0] = v0;
		verts[1] = v1;
		verts[2] = v2;
	}
	/// Faces for the vertices - triangles ONLY
	size_t verts[3];
	//Vector3 colour;
};


class GFXObject : public Object {
public:
	typedef std::shared_ptr<GFXObject> Handle;

	virtual bool outputAsJson( String*& output ) const = 0;

	bool addChild( GFXObject::Handle child ) {
		_children.push_back( child );
		return true;
	}

	GFXObject::Handle toGFXHandle () {
		return _self;
	}

protected:
	GFXObject()
		: Object() {
		// empty
	}

	GFXObject::Handle _self;

private:
	List< GFXObject::Handle > _children;
};








class Mesh : public GFXObject {
public:
	typedef std::shared_ptr<Mesh> Handle;

	static Mesh::Handle createEmpty() {
		Mesh::Handle mesh(new Mesh());

		return mesh;
	}

	virtual bool outputAsJson ( String*& output ) const {
		*output = "";

		return true;
	}

	inline bool addTri ( size_t v1, size_t v2, size_t v3 ) {
		_faces.push_back( Surface(v1,v2,v3) );
		return true;
	}

	inline bool addQuad ( size_t v1, size_t v2, size_t v3, size_t v4 ) {
		return addTri( v1, v2, v3 ) && addTri( v3, v4, v1 );
	}

	bool addCube ( const Vector3& origin, const Vector3& upVector, const Vector3& scale ){

		// Create the list of vertices
		VertexPos verts[8] = {
			{ Vector3( 0.5f, 0.5f, 0.5f ) },
			{ Vector3( 0.5f, 0.5f,-0.5f ) },
			{ Vector3(-0.5f, 0.5f,-0.5f ) },
			{ Vector3(-0.5f, 0.5f,-0.5f ) },
			{ Vector3( 0.5f,-0.5f, 0.5f ) },
			{ Vector3( 0.5f,-0.5f,-0.5f ) },
			{ Vector3(-0.5f,-0.5f,-0.5f ) },
			{ Vector3(-0.5f,-0.5f,-0.5f ) }
		};

		// Create the matrix for the transformation
		Matrix44 mat;
		mat.translate(origin);
		// Translate to origin
		// Rotate by axis-angle
		// Vector3 rotationAxis = Vector3::Up.cross(upVector);
		// float rotationAngle = Vector3::Up.computeAngleBetween(upVector);
		// And scale by scale
		mat.scale(scale);

		// Get the starting place of the vertices
		size_t index = _vertices.size();



		// Push on the list of faces
		for( size_t v=0; v< 8; v++ ) {
			// Multiply the positions before!
			mat.applyTo( &(verts[v].pos) );
			_vertices.push_back( verts[v] );
		}


		// Now add all the faces in the right order -
		// Counter-clockwise in RHR
		// top   (+y)
		addQuad( index+0, index+1, index+2, index+3 );
		// front (+z)
		addQuad( index+0, index+3, index+7, index+4 );
		// back  (-z)
		addQuad( index+1, index+5, index+6, index+2 );
		// left  (-x)
		addQuad( index+2, index+6, index+7, index+3 );
		// right (+x)
		addQuad( index+0, index+4, index+5, index+1 );
		// bottom(-y)
		addQuad( index+5, index+4, index+7, index+6 );

		return true;
	}

protected:
	Mesh()
		: GFXObject() {
		// empty
	}

private:
	Array<VertexPos> _vertices;
	Array<Surface> _faces;
};


class Billboard : public GFXObject {
};
class Emitter : public GFXObject {
};
class LODManager : public GFXObject {
};
#endif // _INCLUDED_GFX_HPP
