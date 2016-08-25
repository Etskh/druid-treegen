#ifndef _INCLUDED_MATH_HPP
#define _INCLUDED_MATH_HPP
// Math

#include <cmath>


inline float isEqual( float a, float b ) {
	static const float epsilon = 0.0001f;
	return abs( a - b ) < epsilon;
}




struct Vector3 {

	float x, y, z;

	static const Vector3 Up;

	static Vector3 random() {
		return Vector3(
			Random::Float(),
			Random::Float(),
			Random::Float()
		);
	}

	Vector3(){
		x = y = z = 0.0f;
	}
	Vector3( float _x, float _y, float _z ) {
		x = _x;
		y = _y;
		z = _z;
	}
	Vector3( const Vector3& ref) {
		x = ref.x;
		y = ref.y;
		z = ref.z;
	}

	void operator += ( const Vector3& ref ) {
		x += ref.x;
		y += ref.y;
		z += ref.z;
	}

	Vector3 operator * ( float scalar ) const {
		return Vector3( x*scalar, y*scalar, z*scalar );
	}

	Vector3 operator - ( const Vector3& ref ) const {
		return Vector3( x - ref.x, y - ref.y, z - ref.z );
	}

	inline Vector3 cross( const Vector3& ref ) const {
		return Vector3(
			y*ref.z - z*ref.y,
			z*ref.x - x*ref.z,
			x*ref.y - y*ref.z
		);
	}

	inline float dot ( const Vector3& ref ) const {
		return x*ref.x + y*ref.y + z*ref.z;
	}

	void normalize () {
		float len = getLength();
		x /= len;
		y /= len;
		z /= len;
	}

	float computeAngleBetween ( const Vector3& ref ) {
		return acos( dot(ref) / (getLength() * ref.getLength()) );
	}

	inline bool isNormalized() {
		return isEqual( getLengthSq(), 1.0f );
	}

	inline float getLength() const {
		return sqrt( x*x + y*y + z*z );
	}

	inline float getLengthSq() const {
		return x*x + y*y + z*z;
	}
};




const Vector3 Vector3::Up = Vector3( 0.0f, 1.0f, 0.0f );








struct Vector4 {
	Vector4( const Vector3& ref)
		: x(ref.x)
		, y(ref.y)
		, z(ref.z)
		, w(1.0f) {
		// empty
	}
	float x,y,z,w;
};








struct Matrix44 {
	float m[16];

	Matrix44() {
		setIdentity();
	}

	void translate ( const Vector3& ref ) {
		m[ 3] += ref.x;
		m[ 7] += ref.y;
		m[11] += ref.z;
	}

	//void rotate ( Vector3& ref) {
	//
	//}

	void scale ( const Vector3& ref ) {
		m[ 0] *= ref.x;
		m[ 5] *= ref.y;
		m[11] *= ref.z;
	}

	void applyTo( Vector3* out ) const {
		Vector4 ref(*out);
		out->x = ref.x*m[ 0] + ref.y*m[ 1] + ref.z*m[ 2] + ref.w*m[ 3];
		out->y = ref.x*m[ 4] + ref.y*m[ 5] + ref.z*m[ 6] + ref.w*m[ 7];
		out->z = ref.x*m[ 8] + ref.y*m[ 9] + ref.z*m[10] + ref.w*m[11];
		//out->w = ref.x*m[12] + ref.y*m[13] + ref.z*m[14] + ref.w*m[15];
	}

	void setIdentity () {
		for( size_t i=0; i<16; i++) {
			m[i] = 0.0f;
		}
		m[0]=m[5]=m[10]=m[15] = 1.0f;
	}
};




#endif // _INCLUDED_MATH_HPP
