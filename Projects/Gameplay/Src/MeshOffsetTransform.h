#pragma once

#include <Component.h>
#include <AglMatrix.h>

class MeshOffsetTransform: public Component
{
public:
	MeshOffsetTransform(AglMatrix p_offset)
		: Component( ComponentType::MeshOffsetTransform )
	{
		offset = p_offset;
	}

	~MeshOffsetTransform()
	{

	}

	AglMatrix offset;

};