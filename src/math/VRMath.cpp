#include "VRMath.h"

#include <math.h>
#define VRMATH_EPSILON 1e-8

namespace MinVR {

VRPoint3::VRPoint3() {
  x = y = z = 0;
} 
  
VRPoint3::VRPoint3(float xx, float yy, float zz) {
  x = xx; y = yy; z = zz;
} 
  
VRPoint3::VRPoint3(float *p) { 
  x = p[0]; y = p[1]; z = p[2]; 
}

VRPoint3::VRPoint3(VRFloatArray da) {
  x = da[0]; y = da[1]; z = da[2];
}

VRPoint3::VRPoint3(VRAnyCoreType t) {
  VRFloatArray da = t;
  x = da[0]; y = da[1]; z = da[2];
}

VRPoint3::VRPoint3(const VRPoint3& p) { 
  x = p.x; y = p.y; z = p.z; 
}
  
VRPoint3::~VRPoint3() {
}
  
bool VRPoint3::operator==(const VRPoint3& p) const {
  return (fabs(p.x - x) < VRMATH_EPSILON && 
    fabs(p.y - y) < VRMATH_EPSILON && 
    fabs(p.z - z) < VRMATH_EPSILON);
}
  
bool VRPoint3::operator!=(const VRPoint3& p) const {
  return (fabs(p.x - x) >= VRMATH_EPSILON || 
    fabs(p.y - y) >= VRMATH_EPSILON || 
    fabs(p.z - z) >= VRMATH_EPSILON);
}
  
VRPoint3& VRPoint3::operator=(const VRPoint3& p) {
  x = p.x; y = p.y; z = p.z;
  return *this;		
}
  
float VRPoint3::operator[](const int i) const { 
  if (i==0) return x;
  else if (i==1) return y;
  else if (i==2) return z;
  else return 1.0; // w component of a point is 1 so return the constant 1.0
}
  
float& VRPoint3::operator[](const int i) { 
  if (i==0) return x;
  else if (i==1) return y;
  else return z;
}

VRFloatArray VRPoint3::toVRFloatArray() const {
  VRFloatArray a;
  a.push_back(x);
  a.push_back(y);
  a.push_back(z);
  return a;
}




VRVector3::VRVector3() { 
  x = y = z = 0; 
}
  
VRVector3::VRVector3(float xx, float yy, float zz) {
  x = xx; y = yy; z = zz;
} 
  
VRVector3::VRVector3(float *v) { 
  x = v[0]; y = v[1]; z = v[2]; 
}

VRVector3::VRVector3(VRFloatArray da) {
  x = da[0]; y = da[1]; z = da[2];
}

VRVector3::VRVector3(VRAnyCoreType t) {
  VRFloatArray da = t;
  x = da[0]; y = da[1]; z = da[2];
}

VRVector3::VRVector3(const VRVector3& v) {
  x = v.x; y = v.y; z = v.z;
} 
  
VRVector3::~VRVector3() {
}

bool VRVector3::operator==(const VRVector3& v) const {
  return (fabs(v.x - x) < VRMATH_EPSILON && 
    fabs(v.y - y) < VRMATH_EPSILON && 
    fabs(v.z - z) < VRMATH_EPSILON);
}
  
bool VRVector3::operator!=(const VRVector3& v) const {
  return (fabs(v.x - x) >= VRMATH_EPSILON || 
    fabs(v.y - y) >= VRMATH_EPSILON || 
    fabs(v.z - z) >= VRMATH_EPSILON);
}

VRVector3& VRVector3::operator=(const VRVector3& v) {
  x = v.x; y = v.y; z = v.z;
  return *this;		
}
  
float VRVector3::operator[](const int i) const { 
  if (i==0) return x;
  else if (i==1) return y;
  else return z;
}

float& VRVector3::operator[](const int i) { 
  if (i==0) return x;
  else if (i==1) return y;
  else return z;
}
  
float VRVector3::dot(const VRVector3& v) {
  return x * v[0] + y * v[1] + z * v[2];
}

VRVector3 VRVector3::cross(const VRVector3& v) {
  return VRVector3(y * v[2] - z * v[1],  z * v[0] - x * v[2],  x * v[1] - y * v[0]);
}

float VRVector3::length() {
  return sqrt(x*x + y*y + z*z); 
}

VRVector3 VRVector3::normalize() {
  // Hill & Kelley provide this:
  float sizeSq = x*x + y*y + z*z; 
  if (sizeSq < VRMATH_EPSILON) { 
    return VRVector3(x,y,z); // does nothing to zero vectors;
  } 
  float scaleFactor = (float)1.0/(float)sqrt(sizeSq);
  float xx = x * scaleFactor; 
  float yy = y * scaleFactor;
  float zz = z * scaleFactor;
  return VRVector3(xx, yy, zz);
}

VRFloatArray VRVector3::toVRFloatArray() const {
  VRFloatArray a;
  a.push_back(x);
  a.push_back(y);
  a.push_back(z);
  return a;
}




VRMatrix4::VRMatrix4() {
  m[0] = m[5]  = m[10] = m[15] = 1.0;
  m[1] = m[2]  = m[3]  = m[4]  = 0.0;
  m[6] = m[7]  = m[8]  = m[9]  = 0.0;
  m[11]= m[12] = m[13] = m[14] = 0.0; 
}
  
  
VRMatrix4::VRMatrix4(const float* a) { 
  memcpy(m,a,16*sizeof(float)); 
}

VRMatrix4::VRMatrix4(VRFloatArray da) {
    memcpy(m,&da[0],16*sizeof(float));
}

VRMatrix4::VRMatrix4(VRAnyCoreType t) {
  VRFloatArray da = t;
  memcpy(m,&da[0],16*sizeof(float));
}

  
VRMatrix4::VRMatrix4(const VRMatrix4& m2) { 
  memcpy(m,m2.m,16*sizeof(float)); 
}
  
VRMatrix4::~VRMatrix4() {
}
  
    
bool VRMatrix4::operator==(const VRMatrix4& m2) const {
  for (int i=0;i<16;i++) {
    if (fabs(m2.m[i] - m[i]) > VRMATH_EPSILON) {
      return false;
    }
  }
  return true;
}

bool VRMatrix4::operator!=(const VRMatrix4& m2) const {
  return !(*this == m2);
}
  
VRMatrix4& VRMatrix4::operator=(const VRMatrix4& m2) {
  memcpy(m,m2.m,16*sizeof(float));
  return *this;
}
  
float VRMatrix4::operator()(const int r, const int c) const { 
  return m[c*4+r]; 
}
  
float& VRMatrix4::operator()(const int r, const int c) { 
  return m[c*4+r]; 
}


    
VRMatrix4 VRMatrix4::scale(const VRVector3& v) {
    return VRMatrix4::fromRowMajorElements(v[0], 0, 0, 0,
                                           0, v[1], 0, 0,
                                           0, 0, v[2], 0,
                                           0, 0, 0, 1);
}

    
VRMatrix4 VRMatrix4::translation(const VRVector3& v) {
  return VRMatrix4::fromRowMajorElements(1, 0, 0, v[0],
                                         0, 1, 0, v[1],
                                         0, 0, 1, v[2],
                                         0, 0, 0, 1);
}

    
VRMatrix4 VRMatrix4::rotationX(const float radians) {
  const float cosTheta = cos(radians); 
  const float sinTheta = sin(radians);  
  return VRMatrix4::fromRowMajorElements(1, 0, 0, 0,
                                         0, cosTheta, -sinTheta, 0,
                                         0, sinTheta, cosTheta, 0,
                                         0, 0, 0, 1);
}

    
VRMatrix4 VRMatrix4::rotationY(const float radians) {
  const float cosTheta = cos(radians); 
  const float sinTheta = sin(radians);  
  return VRMatrix4::fromRowMajorElements(cosTheta, 0, sinTheta, 0,
                                         0, 1, 0, 0,
                                         -sinTheta, 0, cosTheta, 0,
                                         0, 0, 0, 1);
}

    
VRMatrix4 VRMatrix4::rotationZ(const float radians) {
  const float cosTheta = cos(radians); 
  const float sinTheta = sin(radians);  
  return VRMatrix4::fromRowMajorElements(cosTheta, -sinTheta, 0, 0,
                                         sinTheta, cosTheta, 0, 0,
                                         0, 0, 1, 0,
                                         0, 0, 0, 1);
}

    
VRMatrix4 VRMatrix4::rotation(const VRPoint3& p, const VRVector3& v, const float a) {
  // Translate to origin from point p
  const float vZ = v[2];
  const float vX = v[0];
  const float theta = atan2(vZ, vX);
  const float phi   = -atan2((float)v[1], (float)sqrt(vX * vX + vZ * vZ));

  const VRMatrix4 transToOrigin = VRMatrix4::translation(-1.0*VRVector3(p[0], p[1], p[2]));
  const VRMatrix4 A = VRMatrix4::rotationY(theta);
  const VRMatrix4 B = VRMatrix4::rotationZ(phi);
  const VRMatrix4 C = VRMatrix4::rotationX(a);
  const VRMatrix4 invA = VRMatrix4::rotationY(-theta);
  const VRMatrix4 invB = VRMatrix4::rotationZ(-phi);
  const VRMatrix4 transBack = VRMatrix4::translation(VRVector3(p[0], p[1], p[2]));
  
  return transBack * invA * invB * C * B * A * transToOrigin;
}

    
VRMatrix4 VRMatrix4::projection(float left, float right,
                                float bottom, float top,
		                        float near, float far)
{
  return VRMatrix4::fromRowMajorElements(2.0*near/(right-left), 0, (right+left)/(right-left), 0,
                                         0, 2.0*near/(top-bottom), (top+bottom)/(top-bottom), 0,
                                         0, 0, -(far+near)/(far-near), -2.0*far*near/(far-near),
                                         0, 0, -1, 0);
}

    
VRMatrix4 VRMatrix4::fromRowMajorElements(
    const float r1c1, const float r1c2, const float r1c3, const float r1c4,
    const float r2c1, const float r2c2, const float r2c3, const float r2c4,
    const float r3c1, const float r3c2, const float r3c3, const float r3c4,
    const float r4c1, const float r4c2, const float r4c3, const float r4c4)
{
  float m[16];
  m[0]=r1c1; m[4]=r1c2;  m[8]=r1c3; m[12]=r1c4;
  m[1]=r2c1; m[5]=r2c2;  m[9]=r2c3; m[13]=r2c4;
  m[2]=r3c1; m[6]=r3c2; m[10]=r3c3; m[14]=r3c4;
  m[3]=r4c1; m[7]=r4c2; m[11]=r4c3; m[15]=r4c4;
  return VRMatrix4(m);
}
    

    

VRMatrix4 VRMatrix4::orthonormal() const {
  VRVector3 x = getColumn(0).normalize();
  VRVector3 y = getColumn(1);
  y = (y - y.dot(x)*x).normalize();
  VRVector3 z = x.cross(y).normalize();
    return VRMatrix4::fromRowMajorElements(x[0], y[0], z[0], m[3],
                                           x[1], y[1], z[1], m[7],
                                           x[2], y[2], z[2], m[11],
                                           m[12], m[13], m[14], m[15]);
}


VRMatrix4 VRMatrix4::transpose() const {
    return VRMatrix4::fromRowMajorElements(m[0], m[1], m[2], m[3],
                                           m[4], m[5], m[6], m[7],
                                           m[8], m[9], m[10], m[11],
                                           m[12], m[13], m[14], m[15]);
}

    
    
    
// Returns the determinant of the 3x3 matrix formed by excluding the specified row and column
// from the 4x4 matrix.  The formula for the determinant of a 3x3 is discussed on
// page 705 of Hill & Kelley, but note that there is a typo within the m_ij indices in the 
// equation in the book that corresponds to the cofactor02 line in the code below.
float VRMatrix4::subDeterminant(int excludeRow, int excludeCol) const {
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
  float cofactor00 =    (*this)(row[1],col[1]) * (*this)(row[2],col[2])  -  (*this)(row[1],col[2]) * (*this)(row[2],col[1]);
  float cofactor01 = - ((*this)(row[1],col[0]) * (*this)(row[2],col[2])  -  (*this)(row[1],col[2]) * (*this)(row[2],col[0]));  
  float cofactor02 =    (*this)(row[1],col[0]) * (*this)(row[2],col[1])  -  (*this)(row[1],col[1]) * (*this)(row[2],col[0]);
  
  // The determinant is then the dot product of the first row and the cofactors of the first row
  return (*this)(row[0],col[0])*cofactor00 + (*this)(row[0],col[1])*cofactor01 + (*this)(row[0],col[2])*cofactor02;
}

// Returns the cofactor matrix.  The cofactor matrix is a matrix where each element c_ij is the cofactor 
// of the corresponding element m_ij in M.  The cofactor of each element m_ij is defined as (-1)^(i+j) times 
// the determinant of the "submatrix" formed by deleting the i-th row and j-th column from M.
// See the definition in section A2.1.4 (page 705) in Hill & Kelley.   
VRMatrix4 VRMatrix4::cofactor() const {
  VRMatrix4 out;
  // We'll use i to incrementally compute -1^(r+c)
  int i = 1;
  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      // Compute the determinant of the 3x3 submatrix
      float det = subDeterminant(r, c);
      out(r,c) = i * det;
      i = -i;
    }
    i = -i;
  }
  return out;
}

// Returns the determinant of the 4x4 matrix
// See the hint in step 2 in Appendix A2.1.5 (page 706) in Hill & Kelley to learn how to compute this
float VRMatrix4::determinant() const {
  // The determinant is the dot product of any row of C (the cofactor matrix of m) with the corresponding row of m
  VRMatrix4 C = cofactor();
  return C(0,0)*(*this)(0,0) + C(0,1)*(*this)(0,1) + C(0,2)*(*this)(0,2) + C(0,3)*(*this)(0,3);
}

// Returns the inverse of the 4x4 matrix if it is nonsingular.  If it is singular, then returns the
// identity matrix. 
VRMatrix4 VRMatrix4::inverse() const {
  // Check for singular matrix
  float det = determinant();
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


VRVector3 VRMatrix4::getColumn(int c) const {
  return VRVector3(m[c*4], m[c*4+1], m[c*4+2]);
}


VRFloatArray VRMatrix4::toVRFloatArray() const {
  VRFloatArray a;
  a.push_back(m[0]);
  a.push_back(m[1]);
  a.push_back(m[2]);
  a.push_back(m[3]);

  a.push_back(m[4]);
  a.push_back(m[5]);
  a.push_back(m[6]);
  a.push_back(m[7]);

  a.push_back(m[8]);
  a.push_back(m[9]);
  a.push_back(m[10]);
  a.push_back(m[11]);

  a.push_back(m[12]);
  a.push_back(m[13]);
  a.push_back(m[14]);
  a.push_back(m[15]);
  return a;
}








VRVector3 operator/(const VRVector3& v, const float s) {
  const float invS = 1 / s;
  return VRVector3(v.x*invS, v.y*invS, v.z*invS);
}

VRVector3 operator*(const float s, const VRVector3& v) {
  return VRVector3(v.x*s, v.y*s, v.z*s);
}

VRVector3 operator*(const VRVector3& v, const float s) {
  return VRVector3(v.x*s, v.y*s, v.z*s);
}

VRVector3 operator-(const VRVector3& v) {
  return VRVector3(-v.x, -v.y, -v.z);
}

VRPoint3 operator+(const VRVector3& v, const VRPoint3& p) {
  return VRPoint3(p.x + v.x, p.y + v.y, p.z + v.z);
};

VRPoint3 operator+(const VRPoint3& p, const VRVector3& v) {
  return VRPoint3(p.x + v.x, p.y + v.y, p.z + v.z);
}

VRVector3 operator+(const VRVector3& v1, const VRVector3& v2) {
  return VRVector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

VRPoint3 operator-(const VRPoint3& p, const VRVector3& v) {
  return VRPoint3(p.x - v.x, p.y - v.y, p.z - v.z);
}

VRVector3 operator-(const VRVector3& v1, const VRVector3& v2) {
  return VRVector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

VRVector3 operator-(const VRPoint3& p1, const VRPoint3& p2) {
  return VRVector3(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

VRMatrix4 operator*(const VRMatrix4& m, const float& s) {
  VRMatrix4 result;
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      result(r,c) = m(r,c) * s;
    }
  }
  return result;
}

VRMatrix4 operator*(const float& s, const VRMatrix4& m) {
  return m*s;
}

    
VRPoint3 operator*(const VRMatrix4& m, const VRPoint3& p) {
	// For our points, p[3]=1 and we don't even bother storing p[3], so need to homogenize
	// by dividing by w before returning the new point.
    const float winv = 1 / (p[0] * m(3,0) + p[1] * m(3,1) + p[2] * m(3,2) + 1.0 * m(3,3));
    return VRPoint3(winv * (p[0] * m(0,0) + p[1] * m(0,1) + p[2] * m(0,2) + 1.0 * m(0,3)),
                    winv * (p[0] * m(1,0) + p[1] * m(1,1) + p[2] * m(1,2) + 1.0 * m(1,3)),
                    winv * (p[0] * m(2,0) + p[1] * m(2,1) + p[2] * m(2,2) + 1.0 * m(2,3)));

}

    
VRVector3 operator*(const VRMatrix4& m, const VRVector3& v) {
  // For a vector v[3]=0
  return VRVector3(v[0] * m(0,0) + v[1] * m(0,1) + v[2] * m(0,2),
                   v[0] * m(1,0) + v[1] * m(1,1) + v[2] * m(1,2),
                   v[0] * m(2,0) + v[1] * m(2,1) + v[2] * m(2,2));

}


    
VRMatrix4 operator*(const VRMatrix4& m1, const VRMatrix4& m2) {
  VRMatrix4 m = VRMatrix4::fromRowMajorElements(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      for (int i = 0; i < 4; i++) {
        m(r,c) += m1(r,i) * m2(i,c);
      }
    }
  }
  return m;
}

std::ostream & operator<< ( std::ostream &os, const VRPoint3 &p) {
  return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

std::istream & operator>> ( std::istream &is, VRPoint3 &p) {
  // format:  (x, y, z)
  char dummy;
  return is >> dummy >> p.x >> dummy >> p.y >> dummy >> p.z >> dummy;
}

std::ostream & operator<< ( std::ostream &os, const VRVector3 &v) {
  return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
}

std::istream & operator>> ( std::istream &is, VRVector3 &v) {
  // format:  <x, y, z>
  char dummy;
  return is >> dummy >> v.x >> dummy >> v.y >> dummy >> v.z >> dummy;
}

std::ostream & operator<< ( std::ostream &os, const VRMatrix4 &m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )
  return os << "[[" << m(0,0) << ", " << m(0,1) << ", " << m(0,2) << ", " << m(0,3) << "], "
        << "[" << m(1,0) << ", " << m(1,1) << ", " << m(1,2) << ", " << m(1,3) << "], "
        << "[" << m(2,0) << ", " << m(2,1) << ", " << m(2,2) << ", " << m(2,3) << "], "
        << "[" << m(3,0) << ", " << m(3,1) << ", " << m(3,2) << ", " << m(3,3) << "]]";
}

std::istream & operator>> ( std::istream &is, VRMatrix4 &m) {
  // format:  [[r1c1, r1c2, r1c3, r1c4], [r2c1, r2c2, r2c3, r2c4], etc.. ]
  char c;
  return is >> c >> c >> m(0,0) >> c >> m(0,1) >> c >> m(0,2) >> c >> m(0,3) >> c >> c
        >> c >> m(1,0) >> c >> m(1,1) >> c >> m(1,2) >> c >> m(1,3) >> c >> c
        >> c >> m(2,0) >> c >> m(2,1) >> c >> m(2,2) >> c >> m(2,3) >> c >> c
        >> c >> m(3,0) >> c >> m(3,1) >> c >> m(3,2) >> c >> m(3,3) >> c >> c;
}

} // ending namespace MinVR
