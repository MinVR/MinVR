#ifndef COVARIANCEMATRIX_H
#define COVARIANCEMATRIX_H


//#include <VRG3D.h>


class CovarianceMatrix
{
public:

  CovarianceMatrix(const G3D::Array<G3D::Vector3> &samples);
  virtual ~CovarianceMatrix() {};

  void GetPrincipleComponents(float eigenVals[3], G3D::Vector3 eigenVecs[3]);

  void GetCovarianceMatrix(G3D::Matrix3 &covMatr);

  void GetCenterOfMass(G3D::Vector3 &center);
protected:

  void SubtractMean(G3D::Array< G3D::Array<double> > &sampleMinusMean);

  void MultByTranspose(const G3D::Array< G3D::Array<double> > &matrix,
		       G3D::Matrix3 &sqMatrix);

  void ComputeCovarianceMatrix(G3D::Matrix3 &covMatrix);

  G3D::Array< G3D::Array<double> >   m_sampleMatrix;
  
  G3D::Matrix3   m_covMatrix;
  float     m_eigenVals[3];
  G3D::Vector3   m_eigenVecs[3];

  G3D::Vector3   m_meanVec;
};

#endif
