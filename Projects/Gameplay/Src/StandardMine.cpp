#include "StandardMine.h"

StandardMine::StandardMine()
	: Component( ComponentType::StandardMine )
{
	m_age = 0;
	m_ownerId=-1;
	range = 80.0f;
}
StandardMine::~StandardMine()
{

}