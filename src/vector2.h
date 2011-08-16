#ifndef _VECTOR2_H
#define _VECTOR2_H

/*
 * 2D vector for sizes, points etc.
 * Has less functionality than vector3
 */
template <typename T>
class vector2 {
public:
	T x,y;

	vector2() {};
	vector2(const vector2<T > &v) : x(v.x), y(v.y) { }
	vector2(T val) : x(val), y(val) { }
	vector2(T _x, T _y) : x(_x),  y(_y)  { };
	vector2(const T vals[2]) : x(vals[0]), y(vals[1]) { }

	T& operator[](const size_t i) { return (&x)[i]; }

	vector2 operator+(const vector2 &a) const { return vector2(a.x+x, a.y+y); }

	vector2 operator-(const vector2 &a) const { return vector2(x-a.x, y-a.y); }
	vector2 operator-() const { return vector2(-x, -y); }
	//todo: comp w/ epsilon!
	bool operator==(const vector2 &a) const { return ((a.x==x)&&(a.y==y)); }
	bool operator!=(const vector2 &a) const { return ((a.x!=x)&&(a.y!=y)); }
};

typedef vector2<int > vector2i;
typedef vector2<float > vector2f;
typedef vector2<double > vector2d;
typedef vector2<int > vec2i;
typedef vector2<float > vec2f;
typedef vector2<double > vec2d;

#endif
