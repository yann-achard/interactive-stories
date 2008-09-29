/*
** This file is part of assignment two for the course Interactive 3D graphics
** and animation. This is property of Yann Achard.
*/
//------------------------------------------------
#ifndef __VEC3DF_H__
#define __VEC3DF_H__
//------------------------------------------------
#include <math.h>
//------------------------------------------------
// This class is used to handle vectors of three floats and all operations
// related
class Vec3Df {
public:

	// The three floats
	float x;
	float y;
	float z;

	//------------------------------------------------
	// Default Ctor
	Vec3Df(){};
	//------------------------------------------------
	// Ctor with initialization
	Vec3Df(float _x, float _y, float _z){
		x = _x; y = _y;	z = _z;
	};
	//------------------------------------------------
	// Copy Ctor
	Vec3Df(const Vec3Df& v){
		x = v.x; y = v.y; z = v.z;
	};
	//------------------------------------------------
	// Setting the variables
	void	set(float _x, float _y, float _z){
		x = _x;	y = _y;	z = _z;
	};
	//------------------------------------------------
	// Vector = operator
	Vec3Df&	operator=(const Vec3Df& v){
		x = v.x; y = v.y; z = v.z;
		return (*this);
	}
	//------------------------------------------------
	// Vector += operator 
	Vec3Df& operator+=(const Vec3Df& v){
		x += v.x; y += v.y; z += v.z;
		return (*this);
	}
	//------------------------------------------------
	// Vector *= operator
	Vec3Df& operator*=(const Vec3Df& v){
		x *= v.x; y *= v.y; z *= v.z;
		return (*this);
	}
	//------------------------------------------------
	// Vector -= operator
	Vec3Df& operator-=(const Vec3Df& v){
		x -= v.x; y -= v.y; z -= v.z;
		return (*this);
	}
	//------------------------------------------------
	// Vector and float -= operator
	Vec3Df& operator-=(float k){
		x -= k; y -= k; z -= k;
		return (*this);
	}
	//------------------------------------------------
	// Vector and float *= operator
	Vec3Df& operator*=(float k){
		x *= k; y *= k; z *= k;
		return (*this);
	}
	//------------------------------------------------
	// Vector + operator
	const Vec3Df	operator+(Vec3Df& v)const{
		Vec3Df res = *this;
		res += v;
		return (res);
	}
	//------------------------------------------------
	// Vector - operator
	const Vec3Df	operator-(Vec3Df& v)const{
		Vec3Df res = *this;
		res -= v;
		return (res);
	}
	//------------------------------------------------
	// Vector * operator
	const Vec3Df	operator*(Vec3Df& v)const{
		Vec3Df res = *this;
		res *= v;
		return (res);
	}
	//------------------------------------------------
	// Vector and float * operator
	const Vec3Df	operator*(float k)const{
		Vec3Df res = *this;
		res *= k;
		return (res);
	}
	//------------------------------------------------
	// Cross product
	void crossProduct(Vec3Df& u, Vec3Df& v){
		x = u.y*v.z - u.z*v.y;
		y = u.z*v.x - u.x*v.z;
		z = u.x*v.y - u.y*v.x;
	}
	//------------------------------------------------
	// Norm of the vector
	float norm(){
		return (sqrtf(x*x+y*y+z*z));
	}
	//------------------------------------------------
	// Distance between two points
	float dist(Vec3Df& v){
		float dx = x-v.x;
		float dy = y-v.y;
		float dz = z-v.z;
		return (sqrtf(dx*dx+dy*dy+dz*dz));
	}
	//------------------------------------------------
};
//------------------------------------------------
#endif//__VEC3DF_H__
