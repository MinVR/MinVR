/** This small math library provides lightweight support for the
    graphics math needed inside MinVR. Some aspects (e.g., separate
    classes for Point3 and Vector3) are inspired the math libraries
    used in Brown computer graphics courses.  Also based on some
    routines introduced in the Hill & Kelley text used in UMN courses.
    Intended to be lightweight, for use inside MinVR only since
    application programmers will probably want to use the math package
    that is native to whatever graphics engine they are pairing with
    MinVR.
*/

#ifndef VRMATH_H
#define VRMATH_H

#include <iostream>

#include <config/VRDatum.h>

namespace MinVR {

/** @class VRPoint3 
  * @brief 3D Point with floating point coordinates.
  */
class VRPoint3 : public VRFloatArrayConvertible {
public:  
  /// Default point at the origin
  VRPoint3();

  /// Constructs a point given (x,y,z, 1)
  VRPoint3(float x, float y, float z);

  /// Constructs a point given a pointer to x,y,z data
  VRPoint3(float *p);
  
  /// Constructs a point from a VRFloatArray with the first three elements
  /// being [x,y,z]
  VRPoint3(VRFloatArray da);
  
  /// Constructs a point from the VRAnyCoreType wrapper class. The argument
  /// must be able to be interpreted as a VRFloatArray core type.
  VRPoint3(VRAnyCoreType t);
  
  /// Copy constructor for point
  VRPoint3(const VRPoint3& p);

  /// Point destructor
  virtual ~VRPoint3();
  
  /// Check for "equality", taking floating point imprecision into account
  bool operator==(const VRPoint3& p) const;

  /// Check for "inequality", taking floating point imprecision into account
  bool operator!=(const VRPoint3& p) const;

  /// Assignment operator
  VRPoint3& operator=(const VRPoint3& p);

  /// Accesses the ith coordinate of the point
  float operator[](const int i) const;

  /// Accesses the ith coordinate of the point
  float& operator[](const int i);
  
  /// Converts the point to a VRFloatArray for data in a VRDataIndex
  VRFloatArray toVRFloatArray() const;

public:
  float x,y,z; 
};




/** @class VRVector3 
  * @brief 3D vector (magnitude and direction).
  */
class VRVector3 : public VRFloatArrayConvertible {
public:
  /// Default constructor to create zero vector
  VRVector3();

  /// Constructs a vector (x, y, z, 0)
  VRVector3(float x, float y, float z);

  /// Constructs a vector given a pointer to x,y,z data
  VRVector3(float *v);
  
  /// Constructs a vector from a VRFloatArray with the first three elements
  /// being [x,y,z]
  VRVector3(VRFloatArray da);
  
  /// Constructs a vector from the VRAnyCoreType wrapper class. The argument
  /// must be able to be interpreted as a VRFloatArray core type.
  VRVector3(VRAnyCoreType t);

  /// Copy constructor for vector
  VRVector3(const VRVector3& v);

  /// Vector destructor
  virtual ~VRVector3();

  /// Check for "equality", taking floating point imprecision into account
  bool operator==(const VRVector3& v) const;

  /// Check for "inequality", taking floating point imprecision into account
  bool operator!=(const VRVector3& v) const;

  /// Vector assignment operator
  VRVector3& operator=(const VRVector3& v);

  /// Returns the ith coordinate of the vector
  float operator[](const int i) const;

  /// Returns the ith coordinate of the vector
  float& operator[](const int i);  

  // --- Vector operations ---

  /// Returns "this dot v"
  float dot(const VRVector3& v);

  /// Returns "this cross v"
  VRVector3 cross(const VRVector3& v);

  /// Returns the length of the vector
  float length();

  /// Returns a normalized (i.e. unit length) version of the vector
  VRVector3 normalize();
  
  /// Converts the point to a VRFloatArray for data in a VRDataIndex
  VRFloatArray toVRFloatArray() const;

public:
  float x,y,z; 
};



/** @class VRMatrix4
  * @brief A 4x4 transformation matrix
  */
class VRMatrix4 : public VRFloatArrayConvertible {
public: 
  /// Default constructor creates an identity matrix:
  VRMatrix4();

  /// Constructs a matrix given from an array of 16 floats in OpenGL matrix format
  /// (i.e., column major).
  VRMatrix4(const float* a);
  
  /// Constructs a matrix from a VRFloatArray -- an array of 16 floats in OpenGL
  ///  matrix format (i.e., column major order).
  VRMatrix4(VRFloatArray da);
  
  /// Constructs a matrix from the VRAnyCoreType wrapper class. The argument
  /// must be able to be interpreted as a VRFloatArray core type.
  VRMatrix4(VRAnyCoreType t);

  /// Copy constructor
  VRMatrix4(const VRMatrix4& m2);

  /// Destructor
  virtual ~VRMatrix4();
  
  /// Check for "equality", taking floating point imprecision into account
  bool operator==(const VRMatrix4& m2) const;

  /// Check for "inequality", taking floating point imprecision into account
  bool operator!=(const VRMatrix4& m2) const;

  /// Matrix assignment operator
  VRMatrix4& operator=(const VRMatrix4& m2);
  

  /// Returns a pointer to the raw data array used to store the matrix.  This
  /// is a 1D array of 16-elements stored in column-major order.
  float* getArray() { return m; }
    
  /// Access an individual element of the array using the syntax:
  /// VRMatrix4 mat; float row1col2 = mat(1,2);
  float operator()(const int row, const int col) const;

  /// Access an individual element of the array using the syntax:
  /// VRMatrix4 mat; mat(1,2) = 1.0;
  float& operator()(const int row, const int col);
                    
  /// Returns the c-th column of the matrix as a VRVector type, e.g.,:
  /// VRVector3 x = mat.getColumn(0);
  VRVector3 getColumn(int c) const;
  
  

  // --- Static Constructors for Special Matrices ---

  /// Returns the scale matrix described by the vector
  static VRMatrix4 scale(const VRVector3& v);

  /// Returns the translation matrix described by the vector
  static VRMatrix4 translation(const VRVector3& v);

  /// Returns the rotation matrix about the x axis by the specified angle
  static VRMatrix4 rotationX(const float radians);

  /// Returns the rotation matrix about the y axis by the specified angle
  static VRMatrix4 rotationY(const float radians);

  /// Returns the rotation matrix about the z axis by the specified angle
  static VRMatrix4 rotationZ(const float radians);
  
  /// Returns the rotation matrix around the vector v placed at point p, rotate by angle a
  static VRMatrix4 rotation(const VRPoint3& p, const VRVector3& v, const float a);

  /// Returns a projection matrix based on clipping
  static VRMatrix4 projection(float left, float right, float bottom, float top, float near, float far);

  /// Returns a matrix constructed from individual elements passed in row major
  /// order so that the matrix looks "correct" on the screen as you write this
  /// constructor on 4 lines of code as below.  Note the that internally the
  /// matrix constructed will be stored in a 16 element column major array to
  /// be consistent with OpenGL.
  static VRMatrix4 fromRowMajorElements(
      const float r1c1, const float r1c2, const float r1c3, const float r1c4,
      const float r2c1, const float r2c2, const float r2c3, const float r2c4,
      const float r3c1, const float r3c2, const float r3c3, const float r3c4,
      const float r4c1, const float r4c2, const float r4c3, const float r4c4);
    
    
  // --- Transpose, Inverse, and Other General Matrix Functions ---

  /// Returns an orthonormal version of the matrix, i.e., guarantees that the
  /// rotational component of the matrix is built from column vectors that are
  /// all unit vectors and orthogonal to each other.
  VRMatrix4 orthonormal() const;
  
  /// Returns the transpose of the matrix
  VRMatrix4 transpose() const;

  // Returns the determinant of the 3x3 matrix formed by excluding the specified row and column
  // from the 4x4 matrix.
  float subDeterminant(int excludeRow, int excludeCol) const;

  // Returns the cofactor matrix.
  VRMatrix4 cofactor() const;

  // Returns the determinant of the 4x4 matrix
  float determinant() const;

  // Returns the inverse of the 4x4 matrix if it is nonsingular.  If it is singular, then returns the
  // identity matrix. 
  VRMatrix4 inverse() const;
  
    
  /// Converts the point to a VRFloatArray for storage in a VRDataIndex
  VRFloatArray toVRFloatArray() const;
    
    
private:

  float m[16]; // hold a 4 by 4 matrix

};



// ---------- Operator Overloads for Working with Points, Vectors, & Matrices ---------- 


// --- Scalers ---

/// Divide the vector by the scalar s
VRVector3 operator/(const VRVector3& v, const float s);

/// Multiply the vector by the scalar s
VRVector3 operator*(const float s, const VRVector3& v);

/// Multiply the vector by the scalar s
VRVector3 operator*(const VRVector3& v, const float s);

/// Negate the vector
VRVector3 operator-(const VRVector3& v);

// Note: no -(point) operator, that's an undefined operation


// --- Point and Vector Arithmetic ---

/// Adds a vector and a point, returns a point
VRPoint3 operator+(const VRVector3& v, const VRPoint3& p);

/// Adds a point and a vector, returns a point
VRPoint3 operator+(const VRPoint3& p, const VRVector3& v);

/// Adds a vector and a vector, returns a vector
VRVector3 operator+(const VRVector3& v1, const VRVector3& v2);

// Note: no (point + point) operator, that's an undefined operation

/// Subtracts a vector from a point, returns a point
VRPoint3 operator-(const VRPoint3& p, const VRVector3& v);

/// Subtracts v2 from v1, returns a vector
VRVector3 operator-(const VRVector3& v1, const VRVector3& v2);

/// Returns the vector spanning p1 and p2
VRVector3 operator-(const VRPoint3& p1, const VRPoint3& p2);

// Note: no (vector - point) operator, that's an undefined operation


// --- Matrix multiplication for Points, Vectors, & Matrices ---

/// Multiply matrix and scalar, returns the new matrix
VRMatrix4 operator*(const VRMatrix4& m, const float& s);

/// Multiply matrix and scalar, returns the new matrix
VRMatrix4 operator*(const float& s, const VRMatrix4& m);

/// Multiply matrix and point, returns the new point
VRPoint3 operator*(const VRMatrix4& m, const VRPoint3& p);

/// Multiply matrix and vector, returns the new vector
VRVector3 operator*(const VRMatrix4& m, const VRVector3& v);

/// Multiply two matrices, returns the result
VRMatrix4 operator*(const VRMatrix4& m1, const VRMatrix4& m2);


// --- Stream operators ---

// VRPoint3
std::ostream & operator<< ( std::ostream &os, const VRPoint3 &p);
std::istream & operator>> ( std::istream &is, VRPoint3 &p);

// VRVector3
std::ostream & operator<< ( std::ostream &os, const VRVector3 &v);
std::istream & operator>> ( std::istream &is, VRVector3 &v);

// VRMatrix4
std::ostream & operator<< ( std::ostream &os, const VRMatrix4 &m);
std::istream & operator>> ( std::istream &is, VRMatrix4 &m);

} // ending namespace MinVR
 
#endif
