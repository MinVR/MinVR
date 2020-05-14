#include "../include/TexPerFrameSMesh.H"


using namespace G3D;

TexPerFrameSMesh::TexPerFrameSMesh
    (const Array<Vector3> &verts, const Array<Vector3> &normals, const Array<int> &indices,
    const Array<Array<float> > &texCoords, const std::string &textureKey, unsigned int startFrame, unsigned int stopFrame):
    SMesh(verts, normals, indices, false)
{
  m_texCoords = texCoords;
  m_texKey = textureKey;
  m_startFrame = startFrame;
  m_stopFrame = stopFrame;
  if(m_startFrame > texCoords.size())
    m_startFrame = texCoords.size();
  if(m_stopFrame > texCoords.size())
    m_stopFrame = texCoords.size();

  size_t sizeNeeded = 8 + sizeof(Vector3)*verts.size() +
                      8 + sizeof(Vector3)*normals.size() +
                     (8 + sizeof(float)*texCoords[0].size())*(m_stopFrame-m_startFrame);

  m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
/*  Array< Array<short> > charTex(texCoords.size());
  for(int i = 0; i < texCoords.size(); i++){
    charTex[i] = Array<short>(texCoords[i].size());
    for(int j = 0; j < texCoords[i].size(); j++){
      if(texCoords[i][j] == 2*FLT_MAX)
        charTex[i][j] = 0xFFFF;
      else
        charTex[i][j] = (unsigned short)(65535.0f * texCoords[i][j]);
    }
  }
  Just sillyness for debugging... */

  if(m_varArea.isNull()) {
    cerr << "Error: Out of VARArea room!" << endl;
  }
  else {
    m_varArea->reset();
    m_vertexVAR = VAR(verts, m_varArea);
    m_normalVAR = VAR(normals, m_varArea);
    for(int i = m_startFrame; i < m_stopFrame; i++){
      m_texCoordVAR.append(VAR(texCoords[i], m_varArea));
//      m_texCoordVAR.append(VAR(charTex[i], m_varArea));
    }
  }
}

void
TexPerFrameSMesh::draw(RenderDevice *rd, int frame, MinVR::GfxMgrRef gfxMgr)
{
  bool frameTexLoaded = (frame >= m_startFrame && frame < m_stopFrame);
  rd->pushState();
  rd->setShadeMode(RenderDevice::SHADE_SMOOTH);
  if(frameTexLoaded){
    rd->setTexture(0, gfxMgr->getTexture(m_texKey));
    rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA, RenderDevice::BLENDEQ_ADD);
  }
  rd->beginIndexedPrimitives();
  rd->setNormalArray(m_normalVAR);
  rd->setVertexArray(m_vertexVAR);
  if(frameTexLoaded)
    rd->setTexCoordArray(0, m_texCoordVAR[frame-m_startFrame]);
  rd->sendIndices(PrimitiveType::TRIANGLES, m_indices);
  rd->endIndexedPrimitives();
  rd->popState();
}

TexPerFrameSMesh::~TexPerFrameSMesh(){}

void
TexPerFrameSMesh::GetTexCoords(Array<Array<float> > &texCoords)
{
  texCoords = m_texCoords;
}

