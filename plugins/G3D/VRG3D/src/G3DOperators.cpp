//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

#include "../include/G3DOperators.h"



namespace MinVR {

using namespace G3D;

void growAABox(AABox &box, const Vector3 &point)
{
  box = AABox(min(point, box.low()), max(point, box.high()));
}

void growAABox(AABox &box, const AABox &box2)
{
  box = AABox(min(box.low(), box2.low()), max(box.high(), box2.high()));
}

Color3 Color3FromUints(G3D::uint8 r, G3D::uint8 g, G3D::uint8 b)
{
  return Color3((double)r/255.0, (double)g/255.0, (double)b/255.0);
}


// This could definately be implemented more smartly.. but it works.. Turn the double
// into a string, then use the usual code for computing a hash code from a string.
unsigned int hashCode(const double d)
{
  std::ostringstream ostr;
  // if using strstream rather than stringstream, the following call
  // requires a   << "\0"   at the end.
  ostr << d;
  std::string a = std::string(ostr.str());
  int s = (int)a.length();
  int i = 0;
  unsigned int key = s;
  s = G3D::iMin(s, 5);
  while (i < s) {
    key = key ^ ((unsigned int)a[i] << ((i & 3) * 8));
    ++i;
  }
  return key;
}


std::ostream & operator<< ( std::ostream &os, const Vector2 &vec2) {
  // format:  (x, y)
  return os << vec2.toString();
}

std::istream & operator>> ( std::istream &is, Vector2 &vec2) {
  // format:  (x, y)
  char dummy;
  return is >> dummy >> vec2.x >> dummy >> vec2.y >> dummy;
}

/*  This is now defined inside G3D:
std::ostream & operator<< ( std::ostream &os, const Vector3 &vec3) {
  // format:  (x, y, z)
  return os << vec3.toString();
}
*/

std::istream & operator>> ( std::istream &is, Vector3 &vec3) {
  // format:  (x, y, z)
  char dummy;
  return is >> dummy >> vec3.x >> dummy >> vec3.y >> dummy >> vec3.z >> dummy;
}

std::ostream & operator<< ( std::ostream &os, const Matrix3 &m) {
  // format:  ((r1c1, r1c2, r1c3), (r2c1, r2c2, r2c3), (r3c1, r3c2, r3c3))
  return os << "((" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << "), "
        << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << "), "
        << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << "))";
}

std::istream & operator>> ( std::istream &is, Matrix3&m) {
  // format:  ((r1c1, r1c2, r1c3), (r2c1, r2c2, r2c3), (r3c1, r3c2, r3c3))
  char c;
  return is >> c >> c >> m[0][0] >> c >> m[0][1] >> c >> m[0][2] >> c >> c
        >> c >> m[1][0] >> c >> m[1][1] >> c >> m[1][2] >> c >> c
        >> c >> m[2][0] >> c >> m[2][1] >> c >> m[2][2] >> c >> c;
}

std::ostream & operator<< ( std::ostream &os, const Matrix4 &m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )
  return os << "((" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << "), "
        << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << "), "
        << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << "), "
        << "(" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << "))";
}

std::istream & operator>> ( std::istream &is, Matrix4 &m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )
  char c;
  return is >> c >> c >> m[0][0] >> c >> m[0][1] >> c >> m[0][2] >> c >> m[0][3] >> c >> c
        >> c >> m[1][0] >> c >> m[1][1] >> c >> m[1][2] >> c >> m[1][3] >> c >> c
        >> c >> m[2][0] >> c >> m[2][1] >> c >> m[2][2] >> c >> m[2][3] >> c >> c
        >> c >> m[3][0] >> c >> m[3][1] >> c >> m[3][2] >> c >> m[3][3] >> c >> c;
}


// This orthonormalizes the rotation matrix, which means you will loose any scale
// that is in there, but you will gain orthonormal axes, which otherwise you wouldn't
// necessarily have due to precision errors when reading/writing out data to a string.
// If you want to keep scaling info use a Matrix4 instead.
std::istream & operator>> ( std::istream &is, CoordinateFrame &m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )
  char c;
  double d;
  Matrix3 r(1,0,0,0,1,0,0,0,1);
  Vector3 t;
  is >> c >> c >> r[0][0] >> c >> r[0][1] >> c >> r[0][2] >> c >> t[0] >> c >> c
     >> c >> r[1][0] >> c >> r[1][1] >> c >> r[1][2] >> c >> t[1] >> c >> c
     >> c >> r[2][0] >> c >> r[2][1] >> c >> r[2][2] >> c >> t[2] >> c >> c
     >> c >> d >> c >> d >> c >> d >> c >> d >> c >> c;
  r.orthonormalize();
  m = CoordinateFrame(r,t);
  return is;
}

std::ostream & operator<< ( std::ostream &os, const Color3 &c) {
  // format:  (x, y)
  return os << c.toString();
}

std::istream & operator>> ( std::istream &is, Color3 &c) {
  // format:  (x, y)
  char dummy;
  return is >> dummy >> c[0] >> dummy >> c[1] >> dummy >> c[2] >> dummy;
}

std::ostream & operator<< ( std::ostream &os, const Color4 &c) {
  // format:  (x, y)
  return os << c.toString();
}

std::istream & operator>> ( std::istream &is, Color4 &c) {
  // format:  (x, y)
  char dummy;
  return is >> dummy >> c[0] >> dummy >> c[1] >> dummy >> c[2] >> dummy >> c[3] >> dummy;
}

std::string matrix4ToString(Matrix4 m) {
  // format:  ((r1c1, r1c2, r1c3, r1c4), (r2c1, r2c2, r2c3, r2c4), etc.. )

  // in the past, this was output with only 2 decimal places of
  // precision.  that is BAD if you're using this routine to store
  // CoordinateFrames in XML.  So, changing it to output full
  // precision.

  return format("((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
    m[0][0], m[0][1], m[0][2], m[0][3],
    m[1][0], m[1][1], m[1][2], m[1][3],
    m[2][0], m[2][1], m[2][2], m[2][3],
    m[3][0], m[3][1], m[3][2], m[3][3]);
}

std::string coordinateFrameToString(CoordinateFrame cf) {
  return matrix4ToString(cf.toMatrix4());
}


#define BIGNUMBER 9999
int
iMinNonNeg(int i1, int i2)
{
  if (i1 < 0)
    i1 = BIGNUMBER;
  if (i2 < 0)
    i2 = BIGNUMBER;

  if (i1 < i2)
    return i1;
  else
    return i2;
}


bool
popNextToken(std::string &in, std::string &token, bool returnFalseOnSemiColon)
{
  in = trimWhitespace(in);

  // if no more tokens, return false
  if (in.size() == 0) {
    return false;
  }
  else if ((in[0] == ';') && (returnFalseOnSemiColon)) {
    in = in.substr(1);
    return false;
  }

  int end = in.size();
  end = iMinNonNeg(end, in.find(" "));
  end = iMinNonNeg(end, in.find('\t'));
  end = iMinNonNeg(end, in.find(";"));
  end = iMinNonNeg(end, in.find(","));
  end = iMinNonNeg(end, in.find('\n'));
  end = iMinNonNeg(end, in.find('\r'));

  token = in.substr(0,end);
  in = in.substr(end);
  return (token.size() > 0);
}

Array<std::string>
splitStringIntoArray(const std::string &in)
{
  Array<std::string> a;
  std::string s = in;
  std::string token;
  while (popNextToken(s, token, false)) {
    a.append(token);
  }
  return a;
}

std::string  decygifyPath(const std::string &in)
{
/*******************************  CYGWIN
  std::string input = in;
  int startofcyg = input.find("/cygdrive/");
  if (startofcyg >= 0) {
    std::string drive = input.substr(startofcyg + 10, 1);
    std::string newpath = input.substr(0,startofcyg) + drive +
      std::string(":") + input.substr(startofcyg + 11);
    // recursive call
    return decygifyPath(newpath);
  }
  else {
    return input;
  }
**********************************/
  return in;
}


std::string  replaceEnvVars(const std::string &in)
{
  // the special sequence $(NAME) gets replaced by the decygified value
  // of the environment variable NAME
  // If any variable is undefined then an empty string is returned.
  //
  std::string instr = in;

  //  int evstart = instr.find("\$(");
  int evstart = instr.find("$(");

  while (evstart >= 0)
  {
    std::string evandrest = instr.substr(evstart+2);
    int evend = evandrest.find(")");
    std::string ev = evandrest.substr(0,evend);
    const char *pevval = getenv( ev.c_str() );

    if ( pevval == NULL )
    {  instr = "";
       break;
    }

    std::string evval = pevval;
    evval = decygifyPath(evval);
    instr = instr.substr(0,evstart) + evval + evandrest.substr(evend+1);

    //    evstart = instr.find("\$(");
    evstart = instr.find("$(");
  }

  return instr;
}

std::string  intToString(int i)
{
  std::ostringstream ostr;
  // if using strstream rather than stringstream, the following call
  // requires a   << "\0"   at the end.
  ostr << i;

  return std::string(ostr.str());
}



int  stringToInt(const std::string &in)
{
  int i;
  std::istringstream istr(in.c_str());
  istr >> i;
  return i;
}



std::string  realToString(double r)
{
  std::ostringstream ostr;
  // if using strstream rather than stringstream, the following call
  // requires a   << "\0"   at the end.
  ostr << r;
  return std::string(ostr.str());
}


double  stringToReal(const std::string &in)
{
  double r;
  std::istringstream istr(in.c_str());
  istr >> r;
  return r;
}



} // end namespace
