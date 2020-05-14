/**
 * \author Marcus H. Ewert
 *
 * \file  TexPerFrameSMesh.H
 * \brief SMesh with per vertex per frame Texture coordinates (For Tooth Coloring)
 */



 #ifndef TEXPERFRAMESMESH_H
 #define TEXPERFRAMESMESH_H

#include "SMesh.H"

typedef G3D::ReferenceCountedPointer<class TexPerFrameSMesh> TexPerFrameSMeshRef;

/**
 */
class TexPerFrameSMesh : public SMesh
{
public:
//texCoords should be [frame][vertex] accesible texture coordinates texture key is the 
//texture key on the GfXMgrRef passed at render time
//startframe and stopframe are for if you don't want to load all the frames to the graphics card
  TexPerFrameSMesh(const G3D::Array<G3D::Vector3> &verts, const G3D::Array<G3D::Vector3> &normals,
    const G3D::Array<int> &indices, const G3D::Array<G3D::Array<float> > &texCoords, const std::string &textureKey,
    unsigned int startFrame = 0, unsigned int stopFrame = ~0);


  virtual ~TexPerFrameSMesh();

  virtual void draw(G3D::RenderDevice *rd, int frame, MinVR::GfxMgrRef gfxMgr);

  void GetTexCoords(G3D::Array<G3D::Array<float> > &texCoords);

  G3D::Array<G3D::Array<float> > m_texCoords;

protected:

  G3D::Array<G3D::VAR> m_texCoordVAR;
  std::string m_texKey;
  int m_startFrame, m_stopFrame;

};

 #endif
