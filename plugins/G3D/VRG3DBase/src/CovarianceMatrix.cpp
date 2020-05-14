/*


   Author:  Trevor O'Brien

*/

#include <G3D/G3D.h>
#include <GLG3D/GLG3D.h>
#include "../include/CovarianceMatrix.H"

using namespace G3D;

CovarianceMatrix::CovarianceMatrix(const Array<Vector3> &samples)
{

  for (int i = 0; i < samples.size(); i++) {

    Array<double> col;

    // cout << "Vertices arrived" << endl;
    // cout << samples[i].x << " " << samples[i].y << " " << samples[i].z << endl;

    col.append(samples[i].x);
    col.append(samples[i].y);
    col.append(samples[i].z);

    m_sampleMatrix.append(col);
  }

  /*for(int i = 0; i < m_sampleMatrix.size(); i++) {
    cout << "Sample matrix" << endl;
    cout << m_sampleMatrix[i][0] << " "
    << m_sampleMatrix[i][1] << " "
    << m_sampleMatrix[i][2] << endl;
    }
  */
  
  ComputeCovarianceMatrix(m_covMatrix);

  m_covMatrix.eigenSolveSymmetric(m_eigenVals, m_eigenVecs);

  /*  for (int i = 0; i < 3; i++) {
      cout << "Eigenvalue :" << m_eigenVals[i] << " Eigenvector: " << m_eigenVecs[i] << endl;
      }
  */
}


void CovarianceMatrix::SubtractMean(Array< Array<double> > &sampleMinusMean)
{
  double meanX = 0;
  double meanY = 0;
  double meanZ = 0;

  sampleMinusMean = m_sampleMatrix;

  for (int i = 0; i < sampleMinusMean.size(); i++) {
    meanX += sampleMinusMean[i][0];
    meanY += sampleMinusMean[i][1];
    meanZ += sampleMinusMean[i][2];
  }

  meanX = meanX/sampleMinusMean.size();
  meanY = meanY/sampleMinusMean.size();
  meanZ = meanZ/sampleMinusMean.size();

  m_meanVec = Vector3(meanX, meanY, meanZ);

  for (int i = 0; i < sampleMinusMean.size(); i++) {
    sampleMinusMean[i][0] = sampleMinusMean[i][0] - meanX;
    sampleMinusMean[i][1] = sampleMinusMean[i][1] - meanY;
    sampleMinusMean[i][2] = sampleMinusMean[i][2] - meanZ;
  } 
}


void CovarianceMatrix::MultByTranspose(const Array< Array<double> > &matrix,
				       Matrix3 &sqMatrix)
{

  /* cout << "Mean deviation matrix" << endl;
     for(int i = 0; i < matrix.size(); i++) {
     cout << matrix[i][0] << " "
     << matrix[i][1] << " "
     << matrix[i][2] << endl;
     }
  */
  
  Array< Array<double> > transpose;
  // Create transpose matrix.
  for (int j = 0; j < 3; j++) {
    Array <double> row;
    for (int i = 0; i < matrix.size(); i++) {
      row.append(matrix[i][j]);
    }
    transpose.append(row);
  }
  
  
  /*  cout << "Transpose matrix" << endl;
      for (int i = 0; i < transpose.size(); i++) {
      Array<double> row;
      row = transpose[i];
      
      for (int j = 0; j < row.size(); j++) {
      cout << row[j] << " ";
      }
      cout << endl;
      }
  */
  
  float squareMatrix[3][3];
  // Compute square matrix.
  for (int i = 0; i < transpose.size(); i++) {
    for (int k = 0; k < transpose.size(); k++) {
   
      Array<double> row = transpose[i];
      Array<double> col = transpose[k];

      double entry = 0;
      for (int j = 0; j < row.size(); j++) {
	entry += (row[j] * col[j]);
      }
      squareMatrix[i][k] = entry;
      //cout << entry << " ";
    }
    //cout << endl;
  }

  sqMatrix = Matrix3(squareMatrix);
  
}


void CovarianceMatrix::ComputeCovarianceMatrix(Matrix3 &covMatrix)
{
  Array< Array<double> > samplesMinusMean;
  SubtractMean(samplesMinusMean);

  Array< Array<double> > meanCovSamples;
  MultByTranspose(samplesMinusMean, covMatrix);  

  float scaleFactor = 1.0/(samplesMinusMean.size() - 1);

  /*for (int i = 0; i < 3; i++) {
    cout << covMatrix[i][0] << endl;
    cout << covMatrix[i][1] << endl;
    cout << covMatrix[i][2] << endl;
    }
  */
  
  covMatrix = covMatrix * scaleFactor;
}


void CovarianceMatrix::GetPrincipleComponents(float eigenVals[3], Vector3 eigenVecs[3])
{
  eigenVals = m_eigenVals;
  eigenVecs = m_eigenVecs;

  /*cout << "GetPrinc" << endl;
    for (int i = 0; i < 3; i++) {
    cout << "Eigenvalue :" << eigenVals[i] << " Eigenvector: " << eigenVecs[i] << endl;
    }
  */
}


void CovarianceMatrix::GetCovarianceMatrix(Matrix3 &covMatr)
{
  covMatr = m_covMatrix;
}


void CovarianceMatrix::GetCenterOfMass(Vector3 &center)
{
  center = m_meanVec;
}
