#ifndef INCLUDE_AL_QUAT_HPP
#define INCLUDE_AL_QUAT_HPP

#include "al_Quat.h"
#include "al_Vec.hpp"

#ifndef ABS
	#define ABS(x) ((x)<0?-(x):(x))
#endif

namespace al {

/*
	Quat<double> is equivalent to struct al_quat
*/

template<class T> class Quat;

typedef Quat<double>	Quatd;	///< Double-precision quaternion
typedef Quat<float>		Quatf;	///< Single-precision quaternion


/// Quaternion
template<typename T=double>
class Quat {	
public:
	union{
		struct{
			T w;
			T x;
			T y; 
			T z;
		};
		T components[4];
	};

	Quat(T w = T(1), T x = T(0), T y = T(0), T z = T(0));
	Quat(const Quat & src);

	/// Set component with index
	T& operator[](int i){ return components[i];}
	
	/// Get component with index
	const T& operator[](int i) const { return components[i]; }

	/// Returns true if all components are equal
	bool operator ==(const Quat& v) const { return (w==v.w) && (x==v.x) && (y==v.y) && (z==v.z); }
	
	/// Returns true if any components are not equal
	bool operator !=(const Quat& v) const {  return !(*this == v); }

	Quat operator - () const { return Quat(-w, -x, -y, -z); }
	Quat operator + (const Quat& v) const { return Quat(*this)+=v; }
	Quat operator + (const    T& v) const { return Quat(*this)+=v; }
	Quat operator - (const Quat& v) const { return Quat(*this)-=v; }
	Quat operator - (const    T& v) const { return Quat(*this)-=v; }
	Quat operator * (const Quat& v) const { return Quat(*this)*=v; }
	Quat operator * (const    T& v) const { return Quat(*this)*=v; }
	Quat operator / (const Quat& v) const { return Quat(*this)/=v; }
	Quat operator / (const    T& v) const { return Quat(*this)/=v; }


	Quat& operator  =(const Quat& v){ return set(v); }
	Quat& operator  =(const    T& v){ return set(v); }
	Quat& operator +=(const Quat& v){ w+=v.w; x+=v.x; y+=v.y; z+=v.z; return *this; }
	Quat& operator +=(const    T& v){ w+=  v; x+=  v; y+=  v; z+=  v; return *this; }
	Quat& operator -=(const Quat& v){ w-=v.w; x-=v.x; y-=v.y; z-=v.z; return *this; }
	Quat& operator -=(const    T& v){ w-=  v; x-=  v; y-=  v; z-=  v; return *this; }
	Quat& operator *=(const Quat& v){ return set(multiply(v)); }
	Quat& operator *=(const    T& v){ w*=  v; x*=  v; y*=  v; z*=  v; return *this; }
	Quat& operator /=(const Quat& v){ return (*this) *= v.recip(); }
	Quat& operator /=(const    T& v){ w/=v; x/=v; y/=v; z/=v; return *this; }



	/// Returns the conjugate
	Quat conjugate() const { return Quat(w, -x, -y, -z); }

	/// Returns dot product with another quaternion
	T dot(const Quat& v) const { return w*v.w + x*v.x + y*v.y + z*v.z; }

	/// Returns inverse (same as conjugate if normalized as q^-1 = q_conj/q_mag^2)
	Quat inverse() const { return sgn().conjugate(); }

	/// Get magnitude
	T mag() const { return (T)sqrt(magSqr()); }

	/// Get magnitude squared
	T magSqr() const { return dot(*this); }

	/// Returns multiplicative inverse
	Quat recip() const { return conjugate()/magSqr(); }

	/// Returns signum, q/|q|, the closest point on unit 3-sphere
	Quat sgn() const { return Quat(*this).normalize(); }

	Quat multiply(const Quat & q2) const;
	Quat reverseMultiply(const Quat & q2) const;

	/// Set to identity (1,0,0,0)
	Quat& identity(){ return set(1,0,0,0); }

	/// Normalize magnitude to one
	Quat& normalize();

	/// Set components
	Quat& set(T w, T x, T y, T z){
		this->w = w; this->x = x; this->y = y; this->z = z;
		return *this;
	}

	/// Set from other quaternion
	Quat& set(const Quat& q){ return set(q.w, q.x, q.y, q.z); }
	
	/// Warning: Assumes that axes are normalized.
	static Quat fromAxisAngle(T theta, T x1, T y1, T z1);
	static Quat fromEuler(T a, T e, T b);
	static Quat fromMatrix(T * matrix);
	
	/// Convert to 4x4 column-major matrix
	void toMatrix(T * matrix) const;
	
	/// Convert to axis-angle form
	void toAxisAngle(T * aa, T * ax, T * ay, T * az) const;

	/// Convert to Euler angles as T[3]
	void toEuler(T * e) const;
	void toVectorX(T * x, T * y, T * z) const;	
	void toVectorY(T * x, T * y, T * z) const;	
	void toVectorZ(T * x, T * y, T * z) const;	
	void toVectorX(Vec3<T> & v) const;	
	void toVectorY(Vec3<T> & v) const;	
	void toVectorZ(Vec3<T> & v) const;	
	
	/// Rotate vector
	void rotate(Vec3<T>& v) const;
	void rotateVector(const T * src, T * dst) const;
	void rotateVector(const Vec3<T>& src, Vec3<T>& dst) const;
	void rotateVectorTransposed(const T * src, T * dst) const;
	void rotateVectorTransposed(const Vec3<T>& src, Vec3<T>& dst) const;
	
	/// Spherical interpolation
	Quat& slerp(const Quat& target, T amt) { return set(slerp(*this, target, amt)); }
	
	/// Fill an array of Quats with a full spherical interpolation:
	static void slerp_buffer(const Quat& input, const Quat& target, Quat<T> * buffer, int numFrames);

	/*!
		Spherical linear interpolation of a quaternion

		@param result	Resulting interpolated quaternion
		@param target	The quaternion to interpolate toward
		@param amt		The amount to interpolate, range [0, 1]
	*/
	static Quat slerp(const Quat& input, const Quat& target, T amt);
	
	/// Get the quaternion from a given point and quaterion toward another point
	void towardPoint(Vec3<T> &pos, Quat<T> &q, Vec3<T> &v, float amt);
	
	// v1 and v2 must be normalized
	// alternatively expressed as Q = (1+gp(v1, v2))/sqrt(2*(1+dot(b, a)))
	static Quat<T> rotor(Vec3<T> &v1, Vec3<T> &v2);

};

template<class T> Quat<T> operator + (T r, const Quat<T>& q){ return  q+r; }
template<class T> Quat<T> operator - (T r, const Quat<T>& q){ return -q+r; }
template<class T> Quat<T> operator * (T r, const Quat<T>& q){ return  q*r; }
template<class T> Quat<T> operator / (T r, const Quat<T>& q){ return  q.conjugate()*(r/q.magSqr()); }





/// Implementation

template<typename T>
inline Quat<T> :: Quat(T w, T x, T y, T z)
: w(w), x(x), y(y), z(z)
{}

template<typename T>
inline Quat<T> :: Quat(const Quat & src) {
	w = src.w; x = src.x; y = src.y; z = src.z;
}

template<typename T>
inline Quat<T>& Quat<T> :: normalize() {
	T unit = magSqr();
	if(unit*unit < QUAT_EPSILON){
		// unit too close to epsilon, set to default transform
		identity();
	}
	else if(unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN){
		T invmag = 1.0/sqrt(unit);
		(*this) *= invmag;
	}
	return *this;
}

// assumes both are already normalized!
template<typename T>
inline Quat<T> Quat<T> :: multiply(const Quat<T> & q2) const {
	return Quat(
		w*q2.w - x*q2.x - y*q2.y - z*q2.z,
		w*q2.x + x*q2.w + y*q2.z - z*q2.y,
		w*q2.y + y*q2.w + z*q2.x - x*q2.z,
		w*q2.z + z*q2.w + x*q2.y - y*q2.x
	);
}

// assumes both are already normalized!
template<typename T>
inline Quat<T> Quat<T> :: reverseMultiply(const Quat<T> & q2) const {
	return q2 * (*this);
}

template<typename T>
inline Quat<T> Quat<T> :: fromAxisAngle(T theta, T x1, T y1, T z1) {
	Quat q;
	T t2 = theta * 0.00872664626; // * 0.5 * 180/PI
	T sinft2 = sin(t2);
	q.w = cos(t2);
	q.x = x1 * sinft2;
	q.y = y1 * sinft2;
	q.z = z1 * sinft2;
	return q.normalize();
}

template<typename T>
inline Quat<T> Quat<T> :: fromEuler(T az, T el, T ba) {
	//http://vered.rose.utoronto.ca/people/david_dir/GEMS/GEMS.html
	//Converting from Euler angles to a quaternion is slightly more tricky, as the order of operations 
	//must be correct. Since you can convert the Euler angles to three independent quaternions by 
	//setting the arbitrary axis to the coordinate axes, you can then multiply the three quaternions 
	//together to obtain the final quaternion.

	//So if you have three Euler angles (a, b, c), then you can form three independent quaternions
	//Qx = [ cos(a/2), (sin(a/2), 0, 0)]
    //Qy = [ cos(b/2), (0, sin(b/2), 0)]
    //Qz = [ cos(c/2), (0, 0, sin(c/2))]
	//And the final quaternion is obtained by Qx * Qy * Qz.
	
	T c1 = cos(az * QUAT_DEG2RAD_BY2);
	T c2 = cos(el * QUAT_DEG2RAD_BY2);
	T c3 = cos(ba * QUAT_DEG2RAD_BY2);
	T s1 = sin(az * QUAT_DEG2RAD_BY2);
	T s2 = sin(el * QUAT_DEG2RAD_BY2);
	T s3 = sin(ba * QUAT_DEG2RAD_BY2);

	// equiv quat_multiply(&Qy, &Qx, &Q1); // since many terms are zero
	T tw = c1*c2;
	T tx = c1*s2;
	T ty = s1*c2;
	T tz = - s1*s2;
	
	// equiv quat_multiply(&Q1, &Qz, &Q2); // since many terms are zero
	Quat q;
	q.w = tw*c3 - tz*s3;
	q.x = tx*c3 + ty*s3;
	q.y = ty*c3 - tx*s3;
	q.z = tw*s3 + tz*c3;
	return q.normalize();
}

template<typename T>
inline Quat<T> Quat<T> :: fromMatrix(T *m) {
	Quat q;
	T trace = m[0]+m[5]+m[10];
	q.w = sqrt(1. + trace)*0.5;
	
	if(trace > 0.) {
		q.x = (m[6] - m[9])/(4.*q.w);
		q.y = (m[8] - m[2])/(4.*q.w);
		q.z = (m[1] - m[4])/(4.*q.w);
	}
	else {
		if(m[0] > m[5] && m[0] > m[10]) {
			// m[0] is greatest
			q.x = sqrt(1. + m[0]-m[5]-m[10])*0.5;
			q.w = (m[6] - m[9])/(4.*q.x);
			q.y = (m[1] + m[4])/(4.*q.x);
			q.z = (m[2] + m[8])/(4.*q.x);
		}
		else if(m[5] > m[0] && m[5] > m[10]) {
			// m[1] is greatest
			q.y = sqrt(1. + m[5]-m[0]-m[10])*0.5;
			q.w = (m[8] - m[2])/(4.*q.y);
			q.x = (m[1] + m[4])/(4.*q.y);
			q.z = (m[6] + m[9])/(4.*q.y);
		}
		else { //if(m[10] > m[0] && m[10] > m[5]) {
			// m[2] is greatest
			q.z = sqrt(1. + m[10]-m[0]-m[5])*0.5;
			q.w = (m[1] - m[4])/(4.*q.z);
			q.x = (m[2] + m[8])/(4.*q.z);
			q.y = (m[6] + m[9])/(4.*q.z);
		}
	}
	return q;
}

template<typename T>
inline void Quat<T> :: toMatrix(T * m) const {

	static const T _2 = T(2);
	static const T _1 = T(1);		
	T	_2w=_2*w, _2x=_2*x, _2y=_2*y;
	T	wx=_2w*x, wy=_2w*y, wz=_2w*z, xx=_2x*x, xy=_2x*y, 
		xz=_2x*z, yy=_2y*y, yz=_2y*z, zz=_2*z*z;

	m[ 0] =-zz - yy + _1;
	m[ 1] = xy - wz;
	m[ 2] = wy + xz;
	m[ 3] = 0;
	
	m[ 4] = wz + xy;
	m[ 5] =-zz - xx + _1;
	m[ 6] = yz - wx;
	m[ 7] = 0;
	
	m[ 8] = xz - wy;
	m[ 9] = wx + yz;
	m[10] =-yy - xx + _1;
	m[11] = 0;
	
	m[12]=m[13]=m[14]=0;
	m[15]=_1;
}

template<typename T>
inline void Quat<T> :: toAxisAngle(T * aa, T * ax, T * ay, T * az) const {
  T unit = w*w;
  if (unit > QUAT_ACCURACY_MAX || unit < QUAT_ACCURACY_MIN) {
	T invSinAngle = 1.f/sqrt(1.f - unit);
	*aa = acosf(w) * 114.59155902616; // * 2 * 180 / pi
	*ax = x * invSinAngle;
	*ay = y * invSinAngle;
	*az = z * invSinAngle;
  } else {
	*aa = 0.f;
	*ax = x;
	*ay = y;
	*az = z;
  }
}

template<typename T>
inline void Quat<T> :: toEuler(T * e) const {
	// http://www.mathworks.com/access/helpdesk/help/toolbox/aeroblks/quaternionstoeulerangles.html
	T sqw = w*w;
	T sqx = x*x;
	T sqy = y*y;
	T sqz = z*z;
	e[0] = M_RAD2DEG * asin(-2.0 * (x*z - w*y));
	e[1] = M_RAD2DEG * atan2(2.0 * (y*z + w*x),(sqw - sqx - sqy + sqz));
	e[2] = M_RAD2DEG * atan2(2.0 * (x*y + w*z), (sqw + sqx - sqy - sqz));
}

template<typename T>
inline void Quat<T> :: toVectorX(T * vx, T * vy, T * vz) const {
	*vx = 1.0 - 2.0*y*y - 2.0*z*z;
	*vy = 2.0*x*y + 2.0*z*w;
	*vz = 2.0*x*z - 2.0*y*w;	
}

template<typename T>
inline void Quat<T> :: toVectorY(T * vx, T * vy, T * vz) const {
	*vx = 2.0*x*y - 2.0*z*w;
	*vy = 1.0 - 2.0*x*x - 2.0*z*z;
	*vz = 2.0*y*z + 2.0*x*w;
}

template<typename T>
inline void Quat<T> :: toVectorZ(T * vx, T * vy, T * vz) const {
	*vx = 2.0*x*z + 2.0*y*w;
	*vy = 2.0*y*z - 2.0*x*w;
	*vz = 1.0 - 2.0*x*x - 2.0*y*y;
}

template<typename T>
inline void Quat<T> :: toVectorX(Vec3<T>& v) const {
	toVectorX(&v[0], &v[1], &v[2]);
}	

template<typename T>
inline void Quat<T> :: toVectorY(Vec3<T>& v) const {
	toVectorY(&v[0], &v[1], &v[2]);
}	

template<typename T>
inline void Quat<T> :: toVectorZ(Vec3<T>& v) const {
	toVectorZ(&v[0], &v[1], &v[2]);
}		

template<typename T>
inline void Quat<T>::rotate(Vec3<T>& v) const{
	rotateVector(v,v);
}

template<typename T>
inline void Quat<T> :: rotateVector(const T * src, T * dst) const {
	T matrix[16];
	toMatrix(matrix);
	T x = src[0] * matrix[0] + src[1] * matrix[1] + src[2] * matrix[2];
	T y = src[0] * matrix[4] + src[1] * matrix[5] + src[2] * matrix[6];
	T z = src[0] * matrix[8] + src[1] * matrix[9] + src[2] * matrix[10];
	dst[0]=x; dst[1]=y; dst[2]=z;
}

template<typename T>
inline void Quat<T> :: rotateVector(const Vec3<T>& src, Vec3<T>& dst) const {
	rotateVector(&src[0], &dst[0]);
}

template<typename T>
inline void Quat<T> :: rotateVectorTransposed(const T * src, T * dst) const {
	T matrix[16];
	toMatrix(matrix);
	dst[0] = src[0] * matrix[0] + src[1] * matrix[4] + src[2] * matrix[8];
	dst[1] = src[0] * matrix[1] + src[1] * matrix[5] + src[2] * matrix[9];
	dst[2] = src[0] * matrix[2] + src[1] * matrix[6] + src[2] * matrix[10];
}

template<typename T>
inline void Quat<T> :: rotateVectorTransposed(const Vec3<T>& src, Vec3<T>& dst) const {
	rotateVectorTransposed(&src[0], &dst[0]);
}


template<typename T>
Quat<T> Quat<T> :: slerp(const Quat& input, const Quat& target, T amt){
	Quat<T> result;
	int bflip = 0;
	T dot_prod = input.dot(target);
	T a, b;

	//clamp
	dot_prod = (dot_prod < -1) ? -1 : ((dot_prod > 1) ? 1 : dot_prod);

	// if B is on opposite hemisphere from A, use -B instead
	if (dot_prod < 0.0) {
		dot_prod = -dot_prod;
		bflip = 1;
	}

	T cos_angle = acos(dot_prod);
	if(ABS(cos_angle) > QUAT_EPSILON) {
		T sine = sin(cos_angle);
		T inv_sine = 1./sine;

		a = sin(cos_angle*(1.-amt)) * inv_sine;
		b = sin(cos_angle*amt) * inv_sine;

		if (bflip) {
			b = -b;
		}
	}
	else {
		a = amt;
		b = 1.-amt;
	}

	result.w = a*input.w + b*target.w;
	result.x = a*input.x + b*target.x;
	result.y = a*input.y + b*target.y;
	result.z = a*input.z + b*target.z;

	result.normalize();
	return result;
}

template<typename T>
void Quat<T> :: slerp_buffer(const Quat& input, const Quat& target, Quat<T> * buffer, int numFrames){


	/// Sinusoidal generator based on recursive formula x0 = c x1 - x2
	struct RSin {

		/// Constructor
		RSin(const T& frq=T(0), const T& phs=T(0), const T& amp=T(1))
		:	val2(0), mul(0){ 
			T f=frq, p=phs;
			mul  = (T)2 * (T)cos(f);
			val2 = (T)sin(p - f * T(2))*amp;
			val  = (T)sin(p - f       )*amp;
		}

		/// Generate next value.
		T operator()() const {
			T v0 = mul * val - val2;
			val2 = val;
			return val = v0;
		}

		mutable T val;
		mutable T val2;
		T mul;			///< Multiplication factor. [-2, 2] range gives stable sinusoids.
	};


	int bflip = 1;
	T dot_prod = input.dot(target);

	//clamp
	dot_prod = (dot_prod < -1) ? -1 : ((dot_prod > 1) ? 1 : dot_prod);

	// if B is on opposite hemisphere from A, use -B instead
	if (dot_prod < 0.0) {
		dot_prod = -dot_prod;
		bflip = -1;
	}

	const T cos_angle = acos(dot_prod);
	const T inv_frames = 1./((T)numFrames);
		
	if(ABS(cos_angle) > QUAT_EPSILON) 
	{
		const T sine = sin(cos_angle);
		const T inv_sine = 1./sine;
		RSin sinA(-cos_angle*inv_frames, cos_angle, inv_sine);
		RSin sinB(cos_angle*inv_frames, 0, inv_sine * bflip);

		for (int i=0; i<numFrames; i++) {
			T amt = i*inv_frames;
			T a = sinA();
			T b = sinB();

			buffer[i].w = a*input.w + b*target.w;
			buffer[i].x = a*input.x + b*target.x;
			buffer[i].y = a*input.y + b*target.y;
			buffer[i].z = a*input.z + b*target.z;
			buffer[i].normalize();
		}
	} else {
		for (int i=0; i<numFrames; i++) {
			T a = i*inv_frames;
			T b = 1.-a;
			
			buffer[i].w = a*input.w + b*target.w;
			buffer[i].x = a*input.x + b*target.x;
			buffer[i].y = a*input.y + b*target.y;
			buffer[i].z = a*input.z + b*target.z;
			buffer[i].normalize();
		}
	}
}

/*!
	Get the quaternion from a given point and quaterion toward another point
*/
template<typename T>
void Quat<T> :: towardPoint(Vec3<T> &pos, Quat<T> &q, Vec3<T> &v, float amt) {
	Vec3<T> diff, axis;
	Vec3<T>::sub(diff, v, pos);
	Vec3<T>::normalize(diff);
	
	if(amt < 0) {
		diff = diff*-1.;
	}
	
	Vec3<T> zaxis;
	q.toVectorZ(zaxis);
	Vec3<T>::cross(axis, zaxis, diff);
	Vec3<T>::normalize(axis);
	
	float axis_mag_sqr = Vec3<T>::dot(axis, axis);
	float along = Vec3<T>::dot(zaxis, diff);
	
	if(axis_mag_sqr < 0.001 && along < 0) {
		Vec3<T>::cross(axis, zaxis, Vec3<T>(0., 0., 1.));
		Vec3<T>::normalize(axis);
		
		if(axis_mag_sqr < 0.001) {
			Vec3<T>::cross(axis, zaxis, Vec3<T>(0., 1., 0.));
			Vec3<T>::normalize(axis);
		}
		
		axis_mag_sqr = Vec3<T>::dot(axis, axis);
	}
	
	if(along < 0.9995 && axis_mag_sqr > 0.001) {
		float theta = ABS(amt)*acos(along)*M_RAD2DEG;
//			printf("theta: %f  amt: %f\n", theta, amt);
		fromAxisAngle(theta, axis.x, axis.y, axis.z);
	}
	else {
		identity();
	}
}

// v1 and v2 must be normalized
// alternatively expressed as Q = (1+gp(v1, v2))/sqrt(2*(1+dot(b, a)))
template<typename T>
Quat<T> Quat<T> :: rotor(Vec3<T> &v1, Vec3<T> &v2) {
	// get the normal to the plane (i.e. the unit bivector containing the v1 and v2)
	Vec3<T> n;
	Vec3<T>::cross(n, v1, v2);
	Vec3<T>::normalize(n);	// normalize because the cross product can get slightly denormalized

	// calculate half the angle between v1 and v2
	T dotmag = Vec3<T>::dot(v1, v2);
	T theta = acos(dotmag)*0.5;

	// calculate the scaled actual bivector generaed by v1 and v2
	Vec3<T> bivec = n*sin(theta);
	Quat<T> q(cos(theta), bivec.x, bivec.y, bivec.z);

	return q;
}

} // namespace

#endif /* include guard */
