
#include "../include/SMesh.H"
#include "../include/CovarianceMatrix.H"

using namespace G3D;

SMesh::SMesh(const Array<Vector3> &verts, const Array<Vector3> &normals, 
             const Array<int> &indices, bool initVAR)
{
  m_perVertexColor = false;
  m_indices = indices;
  m_vertices = verts;
  m_normals = normals;
  m_bTextured = false;
  m_triTreeDirty = true;

  if(initVAR){
    size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 8 + sizeof(Vector3)*m_normals.size();
    m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
    if (m_varArea.isNull()) {
      cerr << "Error: Out of VARArea room!" << endl;
    }
    else {
      m_varArea->reset();
      m_vertexVAR = VAR(m_vertices, m_varArea);
      m_normalVAR = VAR(m_normals, m_varArea);
    }
  }

  m_boundingBox = new AABox();
  m_boundingSphere = new Sphere();
  // Compute bounding box and sphere.
  MeshAlg::computeBounds(m_vertices, *m_boundingBox, *m_boundingSphere);

  
  //PerformPCA();
}

SMesh::SMesh(const Array<Vector3> &verts, const Array<Vector3> &normals, 
             const Array<int> &indices, const Array<Vector2> &textureCoord, bool initVAR)
{
  m_perVertexColor = false;
  m_indices = indices;
  m_vertices = verts;
  m_normals = normals;
  m_textureCoord.set(0,textureCoord);
  m_bTextured = true;
  m_triTreeDirty = true;

  if(initVAR){
    size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 8 + sizeof(Vector3)*m_normals.size() + 8 + sizeof(Vector2)*m_textureCoord[0].size();
    m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
    if (m_varArea.isNull()) {
      cerr << "Error: Out of VARArea room!" << endl;
    }
    else {
      m_varArea->reset();
      m_vertexVAR = VAR(m_vertices, m_varArea);
      m_normalVAR = VAR(m_normals, m_varArea);
	    m_textureCoordVAR.set(0, VAR(m_textureCoord[0], m_varArea));
    }
  }

  m_boundingBox = new AABox();
  m_boundingSphere = new Sphere();
  // Compute bounding box and sphere.
  MeshAlg::computeBounds(m_vertices, *m_boundingBox, *m_boundingSphere);

  //PerformPCA();
}


SMesh::SMesh(const Array<Vector3> &verts, const Array<Vector3> &normals, 
             const Array<Color3> &colors, const Array<int> &indices)
{
  m_perVertexColor = true;
  m_indices = indices;
  m_vertices = verts;
  m_normals = normals;
  m_colors = colors;
  m_bTextured = false;
  m_triTreeDirty = true;
  
  size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 
                      8 + sizeof(Vector3)*m_normals.size() +
                      8 + sizeof(Vector3)*m_colors.size();
  m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
  if (m_varArea.isNull()) {
    cerr << "Error: Out of VARArea room!" << endl;
  }
  else {
    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    m_normalVAR = VAR(m_normals, m_varArea);
    m_colorVAR  = VAR(m_colors, m_varArea);
  }
  m_boundingBox = new AABox();
  m_boundingSphere = new Sphere();
  // Compute bounding box and sphere.
  MeshAlg::computeBounds(m_vertices, *m_boundingBox, *m_boundingSphere);

  //PerformPCA();
}

SMesh::SMesh(const Array<Vector3> &verts, const Array<Vector3> &normals, 
             const Array<Color3> &colors, const Array<Vector2> &textureCoord, const Array<int> &indices)
{
  m_perVertexColor = true;
  m_indices = indices;
  m_vertices = verts;
  m_normals = normals;
  m_colors = colors;
  m_textureCoord.set(0,textureCoord);
  m_bTextured = true;
  m_triTreeDirty = true;
  
  size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 
                      8 + sizeof(Vector3)*m_normals.size() +
                      8 + sizeof(Vector3)*m_colors.size() +
                      8 + sizeof(Vector2)*m_textureCoord[0].size();
  m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
  if (m_varArea.isNull()) {
    cerr << "Error: Out of VARArea room!" << endl;
  }
  else {
    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    m_normalVAR = VAR(m_normals, m_varArea);
    m_colorVAR  = VAR(m_colors, m_varArea);
    m_textureCoordVAR.set(0, VAR(m_textureCoord[0], m_varArea));
  }
  m_boundingBox = new AABox();
  m_boundingSphere = new Sphere();
  // Compute bounding box and sphere.
  MeshAlg::computeBounds(m_vertices, *m_boundingBox, *m_boundingSphere);

  //PerformPCA();
}

SMesh::~SMesh()
{
}

void
SMesh::draw(RenderDevice *rd, int frame, MinVR::GfxMgrRef gfxMgr, RenderDevice::ShadeMode shadeMode, bool outline, Color3 outlineColor, bool ignoreMaterial)
{
  rd->pushState();
  rd->setShadeMode(shadeMode);
  
  int iMax = 1;
  if (outline) {
    iMax = 2;
  }
  
  rd->setPolygonOffset(1);
  for(int i=0; i<iMax;i++) {
    if (i==1) {
        rd->setRenderMode(RenderDevice::RENDER_WIREFRAME);
        rd->setColor(outlineColor);
        rd->setLineWidth(0.5);
    }
    rd->beginIndexedPrimitives();
    rd->setNormalArray(m_normalVAR);
    rd->setVertexArray(m_vertexVAR);
    if (i==0 && !ignoreMaterial && m_perVertexColor) {
      rd->setColorArray(m_colorVAR);
    }
    if (i==0 && !ignoreMaterial && m_bTextured) {
      Table<int, VAR>::Iterator texcoords = m_textureCoordVAR.begin();
      for(texcoords = m_textureCoordVAR.begin();
          texcoords != m_textureCoordVAR.end();
          ++texcoords){
        if(m_textureRefs.containsKey(texcoords->key)){
          rd->setTexture(texcoords->key, m_textureRefs[texcoords->key]);
          rd->setTexCoordArray(texcoords->key,texcoords->value);
        }
      }
    }
    rd->sendIndices(PrimitiveType::TRIANGLES, m_indices);
    rd->endIndexedPrimitives();
  }
  rd->popState();
}

void
SMesh::drawWireFrame(RenderDevice *rd, int frame, MinVR::GfxMgrRef gfxMgr, bool ignoreMaterial)
{
  rd->pushState();
  rd->setRenderMode(RenderDevice::RENDER_WIREFRAME);
  rd->setLineWidth(2);
  rd->beginIndexedPrimitives();
  rd->setNormalArray(m_normalVAR);
  rd->setVertexArray(m_vertexVAR);
  if (m_perVertexColor && !ignoreMaterial) {
    rd->setColorArray(m_colorVAR);
  }
  if (m_bTextured && !ignoreMaterial) {
    Table<int, VAR>::Iterator texcoords = m_textureCoordVAR.begin();
    for(texcoords = m_textureCoordVAR.begin();
        texcoords != m_textureCoordVAR.end();
        ++texcoords){
      if(m_textureRefs.containsKey(texcoords->key)){
        rd->setTexture(texcoords->key, m_textureRefs[texcoords->key]);
        rd->setTexCoordArray(texcoords->key,texcoords->value);
      }
    }
  }
  rd->sendIndices(PrimitiveType::TRIANGLES, m_indices);
  rd->endIndexedPrimitives();
  rd->popState();
}


void
SMesh::drawFlatGeometry(RenderDevice *rd)
{
  rd->beginIndexedPrimitives();
  rd->setVertexArray(m_vertexVAR);
  rd->sendIndices(PrimitiveType::TRIANGLES, m_indices);
  rd->endIndexedPrimitives();
}


void SMesh::GetIndices(Array<int> &indices)
{
  indices = m_indices;
}


void SMesh::GetVertices(Array<Vector3> &vertices)
{
  vertices = m_vertices;
}


void SMesh::GetNormals(Array<Vector3> &normals)
{
  normals = m_normals;
}

void SMesh::GetAdjacencyArray(Array<MeshAlg::Face> &faces, Array<MeshAlg::Edge> &edges,
			      Array<MeshAlg::Vertex> &vertices)
{
  MeshAlg::computeAdjacency(m_vertices, m_indices, faces, edges, vertices);
}


void SMesh::GetMeshStats(double &minEdge, double &meanEdge, double &medianEdge,double &maxEdge,
		  double &minFaceArea, double &meanFaceArea, double &medianFaceArea,
		  double &maxFaceArea)
{
  MeshAlg::computeAreaStatistics(m_vertices, m_indices, minEdge, meanEdge, medianEdge, 
				 maxEdge, minFaceArea, meanFaceArea, medianFaceArea,
				 maxFaceArea);
}


Box SMesh::GetBoundingBox(void)
{
  return *m_boundingBox;
}
 

Sphere SMesh::GetBoundingSphere(void)
{
  return *m_boundingSphere;
}


double SMesh::GetSurfaceArea(void)
{
  double surfArea = 0;

  for (int i = 0; i < m_indices.size() - 2; i = i + 3) {

    Triangle tri(m_vertices[m_indices[i]], 
		 m_vertices[m_indices[i + 1]], 
		 m_vertices[m_indices[i + 2]]);

    surfArea += tri.area();
  }

  return surfArea;
}


void SMesh::SetVertices(const Array<Vector3> &newVerts)
{
  if (m_varArea.isNull()) {
    cerr << "Error: Out of VARArea room!" << endl;
  }
  else {
    m_varArea->reset();
    m_vertexVAR = VAR(newVerts, m_varArea);
    m_normalVAR = VAR(m_normals, m_varArea);
  }

  m_vertices = newVerts;

  // Compute bounding box and sphere.
  MeshAlg::computeBounds(m_vertices, *m_boundingBox, *m_boundingSphere);

  m_triTreeDirty = true;

  //PerformPCA();

}

void SMesh::PerformPCA(void)
{
  CovarianceMatrix covMatr(m_vertices);

  Matrix3 matrix;
  covMatr.GetCovarianceMatrix(matrix);

  /*cout << "Covariance matrix" << endl;
    for (int i = 0; i < 3; i++) {
    cout << matrix.getRow(i) << endl;
    }
  */

  matrix.eigenSolveSymmetric(m_eigenVals, m_eigenVecs);

  covMatr.GetCenterOfMass(m_center);

  /*  cout << "PCA" << endl;

  for (int i = 0; i < 3; i++) {
  cout << "Eigenvalue :" << eigenVals[i] << " Eigenvector: " << eigenVecs[i] << endl;
  
  }
  */

  float max = 0;

  // Sort our eigenvals and eigenvecs.  

  bool bubbled = false;

  do {

    bubbled = false;
    for (int i = 0; i < 2; i++) {
      if (m_eigenVals[i] < m_eigenVals[i + 1]) {
	// swap eigenvals.
	float temp = m_eigenVals[i];
	m_eigenVals[i] = m_eigenVals[i + 1];
	m_eigenVals[i + 1] = temp;
	
	// swap eigenvecs too.
	Vector3 tempVec = m_eigenVecs[i];
	m_eigenVecs[i] = m_eigenVecs[i + 1];
	m_eigenVecs[i + 1] = m_eigenVecs[i];
	
	bubbled = true;
      }
    }
  } while (bubbled == true); 
  
  if (m_eigenVals[0] >= m_eigenVals[1] * (1.1f))
    max = m_eigenVals[0];

  m_princCompDir = m_eigenVecs[0];
  m_princCompMag = max;

  //cout << "Dir: " << m_primaryGlobalDir << " Mag: " << m_primaryGlobalMag << endl;
}

void SMesh::transformMesh(CoordinateFrame f)
{
    for (int i=0;i<m_vertices.size();i++) {
        m_vertices[i] = f.pointToObjectSpace(m_vertices[i]);
    }
    for(int i=0; i <m_normals.size();i++) {
        m_normals[i] = f.normalToObjectSpace(m_normals[i]);
    }
    
    if (m_varArea.isNull()) {
        cerr << "Error: Out of VARArea room!" << endl;
    }
    else {
        m_varArea->reset();
        m_vertexVAR = VAR(m_vertices, m_varArea);
        m_normalVAR = VAR(m_normals, m_varArea);
    }

    // Compute bounding box and sphere.
    MeshAlg::computeBounds(m_vertices, *m_boundingBox, *m_boundingSphere);
    m_triTreeDirty = true;
    //PerformPCA();
}

bool SMesh::Intersection(Ray r, float &iTime, Vector3 &iPoint)
{
  Vector3 iNormal;
  return Intersection(r, iTime, iPoint, iNormal);
}

bool SMesh::Intersection(Ray r, float &iTime, Vector3 &iPoint, Vector3 &iNormal)
{
  if(m_triTreeDirty) {
    BuildTriTree();
    m_triTreeDirty = false;
  }

  iTime = inf();
  
  Tri::Intersector intersector;
  if (m_triTree.intersectRay(r, intersector, iTime)) {
     Vector2 texCoord;
     intersector.getResult(iPoint, iNormal, texCoord);
     return true;
  }
  else {
    return false;
  }
}

bool SMesh::BacksideIntersection(Ray r, float &iTime, Vector3 &iPoint, Vector3 &iNormal)
{
  if(m_triTreeDirty) {
    BuildTriTree();
    m_triTreeDirty = false;
  }

  iTime = inf();
  
  Tri::Intersector intersector;
  if (m_backsideTriTree.intersectRay(r, intersector, iTime)) {
     Vector2 texCoord;
     intersector.getResult(iPoint, iNormal, texCoord);
     return true;
  }
  else {
    return false;
  }
}


Array<double>
SMesh::CalcVertexDistsToOtherMesh(CoordinateFrame myFrame, SMeshRef m2, CoordinateFrame frame2, 
                                 Vector3 rayDir)
{
  Array<double> distances;
  for (int i=0;i<m_vertices.size();i++) {
    Vector3 vWorld = myFrame.pointToWorldSpace(m_vertices[i]);
    Vector3 vM2 = frame2.pointToObjectSpace(vWorld);
    Vector3 rayDirM2 = frame2.vectorToObjectSpace(rayDir);
    Ray rForward = Ray::fromOriginAndDirection(vM2, rayDirM2);
    Ray rReverse = Ray::fromOriginAndDirection(vM2, -rayDirM2);
    float t;
    Vector3 pM2, normal;
    if (m2->Intersection(rForward, t, pM2, normal)) {
      distances.append((pM2 - vM2).length());
    }
    else if (m2->Intersection(rReverse, t, pM2, normal)) {
      distances.append(- (pM2 - vM2).length());
    }
    else {
      distances.append(0.0);
    }
  }
  return distances;
}

void
SMesh::ApplyColoring(Array<Color3> &colors)
{
  m_perVertexColor = true;
  m_bTextured = false;
  
  m_colors = colors;
  
  size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 
                      8 + sizeof(Vector3)*m_normals.size() +
                      8 + sizeof(Vector3)*colors.size();
  m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
  if (m_varArea.isNull()) {
    cerr << "Error: Out of VARArea room!" << endl;
  }
  else {
    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    m_normalVAR = VAR(m_normals, m_varArea);
    m_colorVAR  = VAR(colors, m_varArea);
  }  
}

bool
SMesh::GetColoring(Array<Color3> &colors)
{
    if (!m_perVertexColor) {
        return false;
    }
    colors = m_colors;
    return true;
}

void
SMesh::ApplyTexturing(Texture::Ref texture, Array<Vector2> &textureCoord,
                      int textureImageUnit)
{
  m_bTextured = true;
  m_perVertexColor = false;


  m_textureCoord.remove(textureImageUnit);
  m_textureCoord.set(textureImageUnit, textureCoord);
  
  size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 
                      8 + sizeof(Vector3)*m_normals.size();

  Table<int, Array<Vector2> >::Iterator texcoords = m_textureCoord.begin();
  for(texcoords = m_textureCoord.begin();
      texcoords != m_textureCoord.end();
      ++texcoords){
    sizeNeeded += 8 + sizeof(Vector2)*texcoords->value.size();
  }
  m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
  if (m_varArea.isNull()) {
    cerr << "Error: Out of VARArea room!" << endl;
  }
  else {
    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    m_normalVAR = VAR(m_normals, m_varArea);
    for(texcoords = m_textureCoord.begin();
        texcoords != m_textureCoord.end();
        ++texcoords){
      m_textureCoordVAR.remove(texcoords->key);
      m_textureCoordVAR.set(texcoords->key, VAR(texcoords->value, m_varArea));
    }
	  m_textureRefs.set(textureImageUnit, texture);
  }  
}

//if this method is used it will use multitexturing but reuse the 
//same texture coordinates as in the m_textureCoordVAR[0]
void
SMesh::ApplyTexturing(Texture::Ref texture, int textureImageUnit)
{
  if(m_textureCoordVAR.size() == 0) return;

  m_bTextured = true;
  m_perVertexColor = false;

  m_textureCoord.set(textureImageUnit, m_textureCoord[0]);
  m_textureCoordVAR.set(textureImageUnit, m_textureCoordVAR[0]);
  m_textureRefs.set(textureImageUnit, texture);
}

void
SMesh::SetTexture(Texture::Ref texture, int textureImageUnit)
{
	m_textureRefs.set(textureImageUnit, texture);
}

void
SMesh::EnableTexture()
{
  m_bTextured = true;
}

void
SMesh::DisableTexture()
{
  m_bTextured = false;
}


AABox SMesh::GetAABoundingBox()
{
  AABox box(m_vertices[0], m_vertices[0]);
  for (int i=1; i < m_vertices.size(); i++) {
    box = AABox(m_vertices[i].min(box.low()), m_vertices[i].max(box.high()));
  }
  return box;
}

void
SMesh::BuildTriTree()
{
  m_triArray.clear();
  m_triTree.clear();
  m_backsideTriArray.clear();
  m_backsideTriArray.clear();
  for (int i=0; i<m_indices.size(); i+=3) {
    Tri t(m_vertices[m_indices[i]], m_vertices[m_indices[i+1]], m_vertices[m_indices[i+2]], m_normals[m_indices[i]], m_normals[m_indices[i+1]], m_normals[m_indices[i+2]]);
    m_triArray.append(t);
    m_backsideTriArray.append(t.otherSide());
  }
  TriTree::Settings s;
  s.algorithm = TriTree::MEAN_EXTENT;
  m_triTree.setContents(m_triArray, s);
  m_backsideTriTree.setContents(m_backsideTriArray, s);
}
