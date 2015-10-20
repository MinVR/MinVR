/** This single-header-file math library provides lightweight support for the graphics math
    needed inside MinVR. Inspired the math libraries used in Brown computer graphics courses.
    Also based on some routines introduced in the Hill & Kelley text used in UMN courses.
    Intended to be lightweight, for use inside MinVR only.  Application programmers will
    probably want to use the math package that is native to whatever graphics engine they
    are pairing with MinVR.
*/

#ifndef VRMATH_H
#define VRMATH_H

#include <math.h>
#define VRMATH_EPSILON 1e-8



/** @class VRPoint3 
  * @brief 3D Point with floating point coordinates.
  */
class VRPoint3 
{  
public:  
  /// Default point at the origin
  VRPoint3() {x = y = z = 0;} 
  
  /// Constructs a point given (x,y,z, 1)
  VRPoint3(double xx, double yy, double zz) {x = xx; y = yy; z = zz;} 
  
  /// Constructs a point given a pointer to x,y,z data
  VRPoint3(double *p) { x = p[0]; y = p[1]; z = p[2]; }
  
  /// Copy constructor for point
  VRPoint3(const VRPoint3& p) { x = p.x; y = p.y; z = p.z; }
  
  /// Point destructor (does nothing)
  virtual ~VRPoint3() {}
  
  /// Check for "equality" -- take floating point imprecision into account
  inline bool operator==(const VRPoint3& p) const {
    return (fabs(p.x - x) < VRMATH_EPSILON && 
	    fabs(p.y - y) < VRMATH_EPSILON && 
	    fabs(p.z - z) < VRMATH_EPSILON);
  }
  
  /// Check for "inequality" -- take floating point imprecision into account
  inline bool operator!=(const VRPoint3& p) const {
    return (fabs(p.x - x) >= VRMATH_EPSILON || 
	    fabs(p.y - y) >= VRMATH_EPSILON || 
	    fabs(p.z - z) >= VRMATH_EPSILON);
  }
  
  /// Assignment operator
  inline VRPoint3& operator=(const VRPoint3& p) {
    x = p.x; y = p.y; z = p.z;
    return *this;		
  }
  
  /// Accesses the ith coordinate of the point
  inline double operator[](const int i) const { 
    if (i==0) return x;
    else if (i==1) return y;
    else if (i==2) return z;
	  else if (i==3) return 1.0; // w component of a point is 1 so return the constant 1.0
  }
  
  /// Accesses the ith coordinate of the point
  inline double& operator[](const int i) { 
    if (i==0) return x;
    else if (i==1) return y;
    else return z;
  }

public:
  double x,y,z; 
};




/** @class VRVector3 
  * @brief 3D vector containing both magnitude and direction.
  */
class VRVector3 
{  
public:  
  /// Default constructor to create zero vector
  VRVector3() { x = y = z = 0; }
  
  /// Constructs a vector (x, y, z, 0)
  VRVector3(double xx, double yy, double zz) {x = xx; y = yy; z = zz;} 
  
  /// Constructs a vector given a pointer to x,y,z data
  VRVector3(double *v) { x = v[0]; y = v[1]; z = v[2]; }

  /// Copy constructor for vector
  VRVector3(const VRVector3& v) {x = v.x; y = v.y; z = v.z;} 
  
  /// Vector destructor (does nothing)
  virtual ~VRVector3() {}

  /// Check for "equality" -- take floating point imprecision into account
  inline bool operator==(const VRVector3& v) const {
    return (fabs(v.x - x) < VRMATH_EPSILON && 
	    fabs(v.y - y) < VRMATH_EPSILON && 
	    fabs(v.z - z) < VRMATH_EPSILON);
  }
  
  /// Check for "inequality" -- take floating point imprecision into account
  inline bool operator!=(const VRVector3& v) const {
    return (fabs(v.x - x) >= VRMATH_EPSILON || 
	    fabs(v.y - y) >= VRMATH_EPSILON || 
	    fabs(v.z - z) >= VRMATH_EPSILON);
  }

  /// Vector assignment operator
  inline VRVector3& operator=(const VRVector3& v) {
    x = v.x; y = v.y; z = v.z;
    return *this;		
  }
  
  /// Returns the ith coordinate of the vector
  inline double operator[](const int i) const { 
    if (i==0) return x;
    else if (i==1) return y;
    else return z;
  }

  /// Returns the ith coordinate of the vector
  inline double& operator[](const int i) { 
    if (i==0) return x;
    else if (i==1) return y;
    else return z;
  }
  

  // --- Vector operations ---

  /// Returns "this dot v"
  inline double dot(const VRVector3& v) {
    return x * v[0] + y * v[1] + z * v[2];
  }

  /// Returns "this cross v"
  inline VRVector3 cross(const VRVector3& v) {
    return VRVector3(y * v[2] - z * v[1], 
       z * v[0] - x * v[2], 
       x * v[1] - y * v[0]);
  }

  /// Returns the length of the vector
  inline double length() {
    return sqrt(x*x + y*y + z*z); 
  }

  /// Returns a normalized (i.e. unit length) version of the vector
  inline VRVector3 normalize() {
    // Hill & Kelley provide this:
    double sizeSq = x*x + y*y + z*z; 
    if (sizeSq < VRMATH_EPSILON) { 
      return v; // does nothing to zero vectors; 
    } 
    double scaleFactor = (double)1.0/(double)sqrt(sizeSq);
    double xx = x * scaleFactor; 
    double yy = y * scaleFactor;
    double zz = z * scaleFactor;
    return VRVector3(xx, yy, zz);
  }


public:
  double x,y,z; 
};



/** @class VRMatrix4
  * @brief A 4x4 transformation matrix
  */
class VRMatrix4 
{
public: 
  /// Default constructor creates an identity matrix:
  VRMatrix4() {
    m[0] = m[5]  = m[10] = m[15] = 1.0; 
    m[1] = m[2]  = m[3]  = m[4]  = 0.0; 
    m[6] = m[7]  = m[8]  = m[9]  = 0.0; 
    m[11]= m[12] = m[13] = m[14] = 0.0; 
  } 
  
  /// Constructs a matrix given the elments.  Be careful, the elements are 
  /// passed in using row major order, so the matrix looks "correct" on the
  /// screen if you write this constructor using 4 lines as it is below.  But,
  /// to stay consistent with OpenGL, the matrix is stored internally in
  /// column major order!
  VRMatrix4(const double a, const double b, const double c, const double d, 
	  const double e, const double f, const double g, const double h, 
	  const double i, const double j, const double k, const double l, 
	  const double mm, const double n, const double o, const double p) {
    m[0]=a; m[4]=b; m[8]=c; m[12]=d;
    m[1]=e; m[5]=f; m[9]=g; m[13]=h;
    m[2]=i; m[6]=j; m[10]=k; m[14]=l;
    m[3]=mm; m[7]=n; m[11]=o; m[15]=p;
  }
  
  /// Constructs a matrix given a 16 element array
  VRMatrix4(const double* a) { memcpy(m,a,16*sizeof(double)); }
  
  /// Copy constructor
  VRMatrix4(const VRMatrix4& m2) { memcpy(m,m2.m,16*sizeof(double)); }
  
  /// Destructor (does nothing)
  virtual ~VRMatrix4() {}
  
  /// Check for "equality" -- take floating point imprecision into account
  inline bool operator==(const VRMatrix4& m2) const {
    int a;
    for(a = 4; a--;) {
      if (fabs(m2.m[a + 0] - m[a + 0]) > VRMATH_EPSILON || 
	  fabs(m2.m[a + 4] - m[a + 4]) > VRMATH_EPSILON || 
	  fabs(m2.m[a + 8] - m[a + 8]) > VRMATH_EPSILON || 
	  fabs(m2.m[a + 12] - m[a + 12]) > VRMATH_EPSILON)
	return false;
    }
    return true;
  }  
  
  /// Check for "inequality" -- take floating point imprecision into account
  inline bool operator!=(const VRMatrix4& m2) const {
    return !(*this == m2);
  }
  
  /// Matrix assignment operator
  inline VRMatrix4& operator=(const VRMatrix4& m2) {
    memcpy(m,m2.m,16*sizeof(double));
    return *this;
  }
  
  /// Returns the (r,c) elment of the matrix
  inline double operator()(const int r, const int c) const { return m[c*4+r]; }
  
  /// Returns the (r,c) elment of the matrix
  inline double& operator()(const int r, const int c) { return m[c*4+r]; }
	

  // --- Static Constructors for Special Matrices ---

  /// Returns the scale matrix described by the vector
  static inline VRMatrix4 scale(const VRVector3& v) {
    return Matrix4(v[0], 0, 0, 0, 
       0, v[1], 0, 0, 
       0, 0, v[2], 0, 
       0, 0, 0, 1);
  }

  /// Returns the translation matrix described by the vector
  static inline VRMatrix4 translation(const VRVector3& v) {
    return VRMatrix4(1, 0, 0, v[0], 
       0, 1, 0, v[1], 
       0, 0, 1, v[2], 
       0, 0, 0, 1);
  }

  /// Returns the rotation matrix about the x axis by the specified angle
  static inline VRMatrix4 rotationX(const double radians) {
    const double cosTheta = cos(radians); 
    const double sinTheta = sin(radians);  
    return VRMatrix4(1, 0, 0, 0, 
       0, cosTheta, -sinTheta, 0, 
       0, sinTheta, cosTheta, 0, 
       0, 0, 0, 1);
  }

  /// Returns the rotation matrix about the y axis by the specified angle
  static inline VRMatrix4 rotationY(const double radians) {
    const double cosTheta = cos(radians); 
    const double sinTheta = sin(radians);  
    return VRMatrix4(cosTheta, 0, sinTheta, 0, 
       0, 1, 0, 0, 
       -sinTheta, 0, cosTheta, 0, 
       0, 0, 0, 1);
  }

  /// Returns the rotation matrix about the z axis by the specified angle
  static inline VRMatrix4 rotationZ(const double radians) {
    const double cosTheta = cos(radians); 
    const double sinTheta = sin(radians);  
    return VRMatrix4(cosTheta, -sinTheta, 0, 0, 
       sinTheta, cosTheta, 0, 0, 
       0, 0, 1, 0, 
       0, 0, 0, 1);
  }

  /// Returns the rotation matrix around the vector v placed at point p, rotate by angle a
  static inline VRMatrix4 rotation(const VRPoint3& p, const VRVector3& v, const double a) {
    // Translate to origin from point p
    const double vZ = v[2];
    const double vX = v[0];
    const double theta = atan2(vZ, vX);
    const double phi   = -atan2((double)v[1], (double)sqrt(vX * vX + vZ * vZ));

    const VRMatrix4 transToOrigin = translation(-Vector3(p[0], p[1], p[2]));
    const VRMatrix4 A = rotationY(theta);
    const VRMatrix4 B = rotationZ(phi);
    const VRMatrix4 C = rotationX(a);
    const VRMatrix4 invA = rotationY(-theta);
    const VRMatrix4 invB = rotationZ(-phi);
    const VRMatrix4 transBack = translation(Vector3(p[0], p[1], p[2]));
    
    return transBack * invA * invB * C * B * A * transToOrigin;
  }


  // --- Transpose, Inverse, and Other General Matrix Functions ---

  /// Returns transpose of the matrix
  inline VRMatrix4 transpose() {
    return VRMatrix4(m[0], m[1], m[2], m[3], 
           m[4], m[5], m[6], m[7], 
           m[8], m[9], m[10], m[11], 
           m[12], m[13], m[14], m[15]);
  }

  // Returns the determinant of the 3x3 matrix formed by excluding the specified row and column
  // from the 4x4 matrix.  The formula for the determinant of a 3x3 is discussed on
  // page 705 of Hill & Kelley, but note that there is a typo within the m_ij indices in the 
  // equation in the book that corresponds to the cofactor02 line in the code below.
  inline double subDeterminant(int excludeRow, int excludeCol) {
    // Compute non-excluded row and column indices
    int row[3];
    int col[3];

    int r=0;
    int c=0;
    for (int i=0; i<4; i++) {
      if (i != excludeRow) {
        row[r] = i;
        r++;
      }
      if (i != excludeCol) {
        col[c] = i;
        c++;
      }
    }
    
    // Compute the cofactors of each element in the first row
    double cofactor00 =    (*this)(row[1],col[1]) * (*this)(row[2],col[2])  -  (*this)(row[1],col[2]) * (*this)(row[2],col[1]);
    double cofactor01 = - ((*this)(row[1],col[0]) * (*this)(row[2],col[2])  -  (*this)(row[1],col[2]) * (*this)(row[2],col[0]));  
    double cofactor02 =    (*this)(row[1],col[0]) * (*this)(row[2],col[1])  -  (*this)(row[1],col[1]) * (*this)(row[2],col[0]);
    
    // The determinant is then the dot product of the first row and the cofactors of the first row
    return (*this)(row[0],col[0])*cofactor00 + (*this)(row[0],col[1])*cofactor01 + (*this)(row[0],col[2])*cofactor02;
  }

  // Returns the cofactor matrix.  The cofactor matrix is a matrix where each element c_ij is the cofactor 
  // of the corresponding element m_ij in M.  The cofactor of each element m_ij is defined as (-1)^(i+j) times 
  // the determinant of the "submatrix" formed by deleting the i-th row and j-th column from M.
  // See the definition in section A2.1.4 (page 705) in Hill & Kelley.   
  inline VRMatrix4 cofactor() {
    VRMatrix4 out;
    // We'll use i to incrementally compute -1^(r+c)
    int i = 1;
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        // Compute the determinant of the 3x3 submatrix
        double det = subDeterminant((*this), r, c);
        out(r,c) = i * det;
        i = -i;
      }
      i = -i;
    }
    return out;
  }

  // Returns the determinant of the 4x4 matrix
  // See the hint in step 2 in Appendix A2.1.5 (page 706) in Hill & Kelley to learn how to compute this
  inline double determinant() {
    // The determinant is the dot product of any row of C (the cofactor matrix of m) with the corresponding row of m
    VRMatrix4 C = cofactor((*this));
    return C(0,0)*(*this)(0,0) + C(0,1)*(*this)(0,1) + C(0,2)*(*this)(0,2) + C(0,3)*(*this)(0,3);
  }

  // Returns the inverse of the 4x4 matrix if it is nonsingular.  If it is singular, then returns the
  // identity matrix. 
  inline VRMatrix4 inverse()
  {
    // Check for singular matrix
    double det = determinant();
    if (fabs(det) < 1e-8) {
      return VRMatrix4();
    }

    // m in nonsingular, so compute inverse using the 4-step procedure outlined in Appendix A2.1.5 
    // (page 706) in Hill & Kelley
    // 1. Find cofactor matrix C
    VRMatrix4 C = cofactor();
    // 2. Find the determinant of M as the dot prod of any row of C with the corresponding row of M.
    // det = determinant(m);
    // 3. Transpose C to get Ctrans
    VRMatrix4 Ctrans = C.transpose();
    // 4. Scale each element of Ctrans by (1/det)
    return Ctrans * (1.0f / det);
  }

public:		
  double m[16]; // hold a 4 by 4 matrix 
};





// ---------- Global Operators ---------- 



// --- Scalers ---

/// Scale a vector by 1/s, returns scaled vector
inline VRVector3 operator/(const VRVector3& v, const double s) {
  const double invS = 1 / s;
  return Vector3(v.x*invS, v.y*invS, v.z*invS);
}

/// Scale a vector by s, returns scaled vector
inline VRVector3 operator*(const double s, const VRVector3& v) {
  return VRVector3(v.x*s, v.y*s, v.z*s);
}

/// Scale a vector by s, returns scaled vector (same as above, different order)
inline VRVector3 operator*(const VRVector3& v, const double s) {
  return VRVector3(v.x*s, v.y*s, v.z*s);
}

/// Returns the negated vector
inline VRVector3 operator-(const VRVector3& v) {
  return VRVector3(-v.x, -v.y, -v.z);
}

// Note: no -(point) operator



// --- Points and Vectors ---

/// Adds a vector and a point, returns a point
inline VRPoint3 operator+(const VRVector3& v, const VRPoint3& p) {
  return VRPoint3(p.x + v.x, p.y + v.y, p.z + v.z);
};

/// Adds a point and a vector, returns a point (same as previous, just different order)
inline VRPoint3 operator+(const VRPoint3& p, const VRVector3& v) {
  return VRPoint3(p.x + v.x, p.y + v.y, p.z + v.z);
}

/// Adds a vector and a vector, returns a vector
inline VRVector3 operator+(const VRVector3& v1, const VRVector3& v2) {
  return VRVector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// Note: no (point + point) operator

/// Subtracts a vector from a point, returns a point
inline VRPoint3 operator-(const VRPoint3& p, const VRVector3& v) {
  return VRPoint3(p.x - v.x, p.y - v.y, p.z - v.z);
}

/// Subtracts v2 from v1, returns a vector
inline VRVector3 operator-(const VRVector3& v1, const VRVector3& v2) {
  return VRVector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

/// The vector spanning p1 and p2
inline VRVector3 operator-(const VRPoint3& p1, const VRPoint3& p2) {
  return VRVector3(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

// Note: no (vector - point) operator



// --- Matrix operators ---

/// Multiply matrix and scalar, returns the new matrix
inline VRMatrix4 operator*(const VRMatrix4& m, const double& s) {
  VRMatrix4 result;
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      result(r,c) = m(r,c) * s;
    }
  }
  return result;
}

/// Multiply matrix and scalar, returns the new matrix (same as above, different order)
inline VRMatrix4 operator*(const double& s, const VRMatrix4& m) {
  return m*s;
}

/// Multiply matrix and point, returns the new point
inline VRPoint3 operator*(const VRMatrix4& m, const VRPoint3& p) {
	// For our points, p[3]=1 and we don't even bother storing p[3], so need to homogenize
	// by dividing by w before returning the new point.
	const double winv = 1 / (p[0] * m.m[3] + p[1] * m.m[7] + p[2] * m.m[11] + 1.0 * m.m[15]);  
	return VRPoint3(winv * (p[0] * m.m[0] + p[1] * m.m[4] + p[2] * m.m[8] + 1.0 * m.m[12]),
				  winv * (p[0] * m.m[1] + p[1] * m.m[5] + p[2] * m.m[9] + 1.0 * m.m[13]),
				  winv * (p[0] * m.m[2] + p[1] * m.m[6] + p[2] * m.m[10] + 1.0 * m.m[14])); 
}

/// Multiply matrix and vector, returns the new vector
inline VRVector3 operator*(const VRMatrix4& m, const VRVector3& v) {
	// For a vector v[3]=0
  return VRVector3(v[0] * m.m[0] + v[1] * m.m[4] + v[2] * m.m[8], 
		 v[0] * m.m[1] + v[1] * m.m[5] + v[2] * m.m[9],
		 v[0] * m.m[2] + v[1] * m.m[6] + v[2] * m.m[10]);
}

/// Multiply two matrices, returns the result
inline VRMatrix4 operator*(const VRMatrix4& m1, const VRMatrix4& m2) {
  VRMatrix4 result = VRMatrix4();
  register int a;
  for(a = 16; a--;) {
    // Dot product:  row(m1, a % 4) * col(m2, a / 4)
    const double *row = &(m1.m[a & 3]), *col = &(m2.m[a & ~3]);    
    result.m[a] = (*row) * (*col++);
    result.m[a] += *(row + 4) * (*col++);
    result.m[a] += *(row + 8) * (*col++);
    result.m[a] += *(row + 12) * (*col);
  }
  return result;
}


// --- Stream operators ---

// VRPoint3
std::ostream & operator<< ( std::ostream &os, const VRPoint3 &p) {
  return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

std::istream & operator>> ( std::istream &is, VRPoint3 &p) {
  // format:  (x, y, z)
  char dummy;
  return is >> dummy >> p.x >> dummy >> p.y >> dummy >> p.z >> dummy;
}


// VRVector3
std::ostream & operator<< ( std::ostream &os, const VRVector3 &v) {
  return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
}

std::istream & operator>> ( std::istream &is, VRVector3 &v) {
  // format:  <x, y, z>
  char dummy;
  return is >> dummy >> v.x >> dummy >> v.y >> dummy >> v.z >> dummy;
}


// VRMatrix4
std::ostream & operator<< ( std::ostream &os, const VRMatrix4 &m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )
  return os << "((" << m(0,0) << ", " << m(0,1) << ", " << m(0,2) << ", " << m(0,3) << "), "
        << "(" << m(1,0) << ", " << m(1,1) << ", " << m(1,2) << ", " << m(1,3) << "), "
        << "(" << m(2,0) << ", " << m(2,1) << ", " << m(2,2) << ", " << m(2,3) << "), "
        << "(" << m(3,0) << ", " << m(3,1) << ", " << m(3,2) << ", " << m(3,3) << "))";

}

std::istream & operator>> ( std::istream &is, VRMatrix4 &m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )
  char c;
  return is >> c >> c >> m(0,0) >> c >> m(0,1) >> c >> m(0,2) >> c >> m(0,3) >> c >> c
        >> c >> m(1,0) >> c >> m(1,1) >> c >> m(1,2) >> c >> m(1,3) >> c >> c
        >> c >> m(2,0) >> c >> m(2,1) >> c >> m(2,2) >> c >> m(2,3) >> c >> c
        >> c >> m(3,0) >> c >> m(3,1) >> c >> m(3,2) >> c >> m(3,3) >> c >> c;
}


#endif
s