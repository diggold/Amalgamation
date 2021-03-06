#include "AglQuaternion.h"

const float AglQuaternion::minslerpdiff = 0.005f;

AglQuaternion::AglQuaternion(AglVector3 p_u, float p_v)
{
	u = p_u;
	v = p_v;
}
AglQuaternion::AglQuaternion(float p_x, float p_y, float p_z, float p_w)
{
	u = AglVector3(p_x, p_y, p_z);
	v = p_w;
}
AglQuaternion::AglQuaternion()
{
	u = AglVector3(0, 0, 0);
	v = 1.0f;
}

AglQuaternion AglQuaternion::operator*(const AglQuaternion& p_other) const
{
	AglVector3 uvec = p_other.u;
	AglVector3 mulU = AglVector3::crossProduct(this->u, p_other.u) + 
							uvec * this->v + this->u * p_other.v;
	float mulV = this->v*p_other.v - AglVector3::dotProduct(this->u, p_other.u);
	AglQuaternion quat(mulU, mulV); 
	return quat;
}
AglQuaternion& AglQuaternion::operator*=(const AglQuaternion& p_other)
{
	AglVector3 uvec = p_other.u;
	AglVector3 mulU = AglVector3::crossProduct(this->u, p_other.u) + 
		uvec * this->v + this->u * p_other.v;
	float mulV = this->v*p_other.v - AglVector3::dotProduct(this->u, p_other.u);
	u = mulU;
	v = mulV;
	return (*this);
}
AglQuaternion AglQuaternion::operator*(const float& p_scalar) const
{
	return AglQuaternion(u * p_scalar, v * p_scalar);
}
AglQuaternion AglQuaternion::operator+(const AglQuaternion& p_other) const
{
	return AglQuaternion(u + p_other.u, v + p_other.v);
}
bool AglQuaternion::operator==(const AglQuaternion& p_other) const
{
	float epsilon = 0.0001f;
	if(u == p_other.u &&
		fabs(v - p_other.v) < epsilon)
		return true;
	return false;
}

AglQuaternion AglQuaternion::identity()
{
	return AglQuaternion(0, 0, 0, 1);
}
AglQuaternion AglQuaternion::constructFromAngularVelocity(const AglVector3& p_angVel)
{
	float a = AglVector3::length(p_angVel);
	if (a < 0.0000001f)
		return AglQuaternion::identity();
	AglVector3 axis = p_angVel / a;
	float cosV = cos(a * 0.5f);
	float sinV = sin(a * 0.5f);
	AglQuaternion quat(axis * sinV, cosV);
	return quat;
}

///-----------------------------------------------------------------------------------
/// Desc
/// \param p_axis MUST BE NORMALIZED or the transformation will be skewed!
/// \param p_angle
/// \return AglQuaternion
///-----------------------------------------------------------------------------------
AglQuaternion AglQuaternion::constructFromAxisAndAngle(const AglVector3& p_axis, const float& p_angle)
{
	float cosV = cos(p_angle * 0.5f);
	float sinV = sin(p_angle * 0.5f);
	AglQuaternion quat(p_axis * sinV, cosV);
	return quat;
}
AglQuaternion AglQuaternion::rotateToFrom(AglVector3 p_from, AglVector3 p_to)
{
	AglVector3 axis = AglVector3::crossProduct(p_from, p_to);
	
	//Parallell vectors. Any orthogonal vector will do
	if (AglVector3::lengthSquared(axis) == 0)
	{
		if (p_from.x != 0)
			axis = AglVector3(-p_from.y, p_from.x, 0);
		else if (p_from.y != 0)
			axis = AglVector3 (0, -p_from.z, p_from.y);
		else
			axis = AglVector3(1, 0, 0);
	}
	AglVector3::normalize(p_from);
	AglVector3::normalize(p_to);
	AglVector3::normalize(axis);

	float dot = AglVector3::dotProduct(p_from, p_to);
	float angle = 0;
	if (dot < 1.0f)
		angle = acos(dot);
	return constructFromAxisAndAngle(axis, angle);
}

AglQuaternion AglQuaternion::conjugate() const
{
	return AglQuaternion(-u, v);
}
void AglQuaternion::normalize()
{
	float frac = 1.0f / (sqrt(u.x * u.x + u.y * u.y + u.z * u.z + v * v));
	u.x *= frac;
	u.y *= frac;
	u.z *= frac;
	v *= frac;
}
void AglQuaternion::transformVector(AglVector3& p_vector) const
{
	AglQuaternion quat(p_vector, 0);

	quat = this->operator*(quat);
	quat = quat.operator*(this->conjugate());

	p_vector = quat.u;
}

//Static functions

float AglQuaternion::dotProduct( const AglQuaternion& pV1, const AglQuaternion& pV2 )
{
	// added by Jarl 2013-02-01
	return pV1.u.x * pV2.u.x + pV1.u.y * pV2.u.y + pV1.u.z * pV2.u.z + pV1.v * pV2.v;
}

AglQuaternion AglQuaternion::lerp(const AglQuaternion& p_q1, const AglQuaternion& p_q2, 
								  const float& p_t)
{
	AglQuaternion q = p_q1 * (1.0f - p_t) + p_q2 * p_t;
	q.normalize();
	return q;
}
AglQuaternion AglQuaternion::slerp(const AglQuaternion& p_q1, const AglQuaternion& p_q2, 
								   const float& p_t, const bool p_shortestWay)
{
	float phi = p_q1.u.x*p_q2.u.x + p_q1.u.y*p_q2.u.y + p_q1.u.z*p_q2.u.z + p_q1.v*p_q2.v;

	float denom = sin(phi);

	float factor1=0.0f;
	float factor2=0.0f;
	if (denom > minslerpdiff) // added by Jarl 2013-02-01
	{
		factor1 = sin(phi*(1.0f-p_t)) / denom;
		factor2 = sin(phi*p_t) / denom;
	}
	else
	{
		// Added by Jarl 2013-02-01, almost the same, so lerp
		factor1 = 1.0f - p_t;
		factor2 = p_t;
	}

	// added by Jarl 2013-02-01
	// slerp the shortest way
	if (p_shortestWay && dotProduct(p_q1,p_q2)<0.0f)
	{
		factor2 = -factor2;
	}

	return p_q1 * factor1 + p_q2 * factor2;
}