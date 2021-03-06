#include "CompoundBody.h"

void CompoundBody::ComputeInertia()
{
	//http://en.wikipedia.org/wiki/Parallel_axis_theorem
	//Uses the parallel axis theorem to compute a combined
	//Inertia tensor in world space around the center of mass
	//for the entire compound body

	//Skew-symmetric matrices are used as a supplement to
	//cross products.

	AglMatrix inertia = AglMatrix::zeroMatrix();
	AglMatrix id = AglMatrix::identityMatrix();
	for (unsigned int i = 0; i < mChildren.size(); i++)
	{
		AglMatrix orientation = mChildren[i]->GetWorld();
		orientation[15] = 1;
		orientation[14] = 0;
		orientation[13] = 0;
		orientation[12] = 0;
		AglMatrix InertiaWorld = AglMatrix::transpose(orientation) * mChildren[i]->GetLocalInertia() * orientation;
		AglVector3 r = mChildren[i]->GetLocalCenterOfMass() - GetCenterOfMass();

		//OuterProduct
		AglMatrix outer(r.x * r.x, r.x * r.y, r.x * r.z, 0, r.y * r.x, r.y * r.y, r.z * r.x, 0,
						r.z * r.x, r.z * r.y, r.z * r.x, 0, 0, 0, 0, 0);

		inertia += InertiaWorld + (id*AglVector3::dotProduct(r, r) - outer) * mChildren[i]->GetLocalMass();
	}
	mInertiaWorld = inertia;
	mInverseInertiaWorld = AglMatrix::inverse(mInertiaWorld);
}
AglBoundingSphere CompoundBody::MergeSpheres(AglBoundingSphere pS1, AglBoundingSphere pS2)
{
	AglVector3 dVec = pS2.position - pS1.position;
	float dist2 = AglVector3::dotProduct(dVec, dVec);
	if ((pS2.radius-pS1.radius)*(pS2.radius-pS1.radius) >= dist2)
	{
		if (pS1.radius > pS2.radius)
			return pS1;
		else
			return pS2;
	}
	else
	{
		AglBoundingSphere s;
		float dist = sqrt(dist2);
		s.radius = (dist+pS1.radius+pS2.radius)*0.5f;
		s.position = pS1.position;
		s.position += dVec * ((s.radius-pS1.radius) / dist);
		return s;
	}
}

CompoundBody::CompoundBody(AglMatrix pTransform)
{
	mLocalTransform = mPreviousLocalTransform = pTransform;
	mInertiaTensor = AglMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	mVelocity = mPreviousVelocity = mForce = mAngularVelocity = mPreviousAngularVelocity = AglVector3(0, 0, 0);
	mStatic = false;
	mMass = 0;
	mInvMass = FLT_MAX;
	mTempStatic = false;
}
CompoundBody::~CompoundBody()
{

}
void CompoundBody::AddChild(RigidBody* pRigidBody)
{
	if (pRigidBody->IsStatic() != mStatic)
	{
		//CANNOT COMBINE STATIC AND NON-STATIC COMPONENTS
		float k = 0;
		k = 1.0f / k;
		return;
	}
	mChildren.push_back(pRigidBody);
	pRigidBody->SetParent(this);
}
void CompoundBody::AddChild(RigidBody* pRigidBody, AglMatrix pLocalTransform)
{
	if (pRigidBody->IsStatic() != mStatic)
	{
		//CANNOT COMBINE STATIC AND NON-STATIC COMPONENTS
		float k = 0;
		k = 1.0f / k;
		return;
	}
	mChildren.push_back(pRigidBody);
	pRigidBody->SetParent(this, pLocalTransform);
}
void CompoundBody::DetachChild(RigidBody* pRigidBody)
{
	for (unsigned int i = 0; i < mChildren.size(); i++)
	{
		if (mChildren[i] == pRigidBody)
		{
			mChildren[i] = mChildren.back();
			mChildren.pop_back();
			pRigidBody->SetParent(NULL);
			break;
		}
	}
}
float CompoundBody::GetMass() const
{
	float m = 0;
	for (unsigned int i = 0; i < mChildren.size(); i++)
		m += mChildren[i]->GetLocalMass();
	return m;
}
float CompoundBody::GetInvMass()
{
	return 1.0f / GetMass();
}
AglMatrix CompoundBody::GetWorld() const
{
	return mLocalTransform;
}
AglVector3 CompoundBody::GetVelocity() const
{
	return mVelocity;
}
AglVector3 CompoundBody::GetAngularVelocity() const
{
	return mAngularVelocity;
}
AglVector3 CompoundBody::GetCenterOfMass()
{
	AglVector3 com(0, 0, 0);
	float m = 0;
	for (unsigned int i = 0; i < mChildren.size(); i++)
	{
		com += mChildren[i]->GetLocalCenterOfMass() * mChildren[i]->GetLocalMass();
		m += mChildren[i]->GetLocalMass();
	}
	com /= m;
	return com;
}

bool CompoundBody::IsCompoundBody()
{
	return true;
}

void CompoundBody::AddImpulse(AglVector3 pImpulse)
{
	if (!mStatic && !mTempStatic)
		mVelocity += pImpulse * GetInvMass();
}
void CompoundBody::AddAngularImpulse(AglVector3 pAngularImpulse, bool p_propagate)
{
	if (!mStatic && !mTempStatic)
		mAngularVelocity += pAngularImpulse;
}
void CompoundBody::RevertVelocity()
{
	if (!mStatic && !mTempStatic)
	{
		mVelocity = mPreviousVelocity;
		mAngularVelocity = mPreviousAngularVelocity;
	}
}

void CompoundBody::RevertPosition()
{
	if (!mStatic && !mTempStatic)
		mLocalTransform = mPreviousLocalTransform;
}

AglMatrix CompoundBody::GetInverseInertiaWorld()
{
	return mInverseInertiaWorld;
}
AglBoundingSphere CompoundBody::GetBoundingSphere()
{
	if (mChildren.size() > 0)
	{
		AglBoundingSphere bs = mChildren[0]->GetBoundingSphere();
		for (unsigned int i = 1; i < mChildren.size(); i++)
		{
			bs = MergeSpheres(bs, mChildren[i]->GetBoundingSphere());
		}
		return bs;
	}
	else
	{
		return AglBoundingSphere(GetWorld().GetTranslation(), 0);
	}
}