// =======================================================================================
//                                      CompoundBody
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Brief
///        
/// # Header
/// Detailed description.....
/// Created on: 5-12-2012 
///---------------------------------------------------------------------------------------
#ifndef HEADER_H
#define HEADER_H

#include "RigidBody.h"

class CompoundBody : public Body
{
private:
	vector<RigidBody*> mChildren;

	AglMatrix mInertiaWorld;
	AglMatrix mInverseInertiaWorld;

private:
	void ComputeInertia();
	AglBoundingSphere MergeSpheres(AglBoundingSphere pS1, AglBoundingSphere pS2);
public:
	CompoundBody(AglMatrix pTransform);
	virtual ~CompoundBody();

	void AddChild(RigidBody* pRigidBody);
	void AddChild(RigidBody* pRigidBody, AglMatrix pLocalTransform);
	void DetachChild(RigidBody* pRigidBody);
	float GetMass() const;
	float GetInvMass();
	AglMatrix GetWorld() const;
	AglVector3 GetVelocity() const;
	AglVector3 GetAngularVelocity() const;
	AglVector3 GetCenterOfMass();

	bool IsCompoundBody();

	void AddImpulse(AglVector3 pImpulse);
	void AddAngularImpulse(AglVector3 pAngularImpulse, bool p_propagate = true);

	void RevertVelocity();
	void RevertPosition();

	AglMatrix GetInverseInertiaWorld();

	vector<RigidBody*> GetChildren(){ return mChildren; }

	AglBoundingSphere GetBoundingSphere();
};

#endif // HEADER_H