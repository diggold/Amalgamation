#include "CameraInfo.h"

CameraInfo::CameraInfo( float p_aspectRatio )
{
	m_projMat = AglMatrix::identityMatrix();
	createPerspectiveMatrix(p_aspectRatio);
}

void CameraInfo::createPerspectiveMatrix( float p_aspectRatio , 
										 float p_nearClip /*= 0.1f*/, 
										 float p_farClip /*= 100.0f*/ )
{
	float fovAsRadians = 3.14f/2.0f;	
	m_projMat = AglMatrix::createPerspectiveMatrix(fovAsRadians,p_aspectRatio,
		p_nearClip,
		p_farClip);
}
