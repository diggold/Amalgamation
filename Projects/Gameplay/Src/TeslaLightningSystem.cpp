#include "TeslaLightningSystem.h"
#include "TeslaEffectPiece.h"
#include "Transform.h"
#include <RandomUtil.h>
#include "RenderInfo.h"

TeslaLightningSystem::TeslaLightningSystem()
	: EntitySystem(SystemType::TeslaLightningSystem, 3,
	ComponentType::TeslaEffectPiece, ComponentType::Transform,
	ComponentType::RenderInfo)
{
}

void TeslaLightningSystem::processEntities( const vector<Entity*>& p_entities )
{
	for(unsigned int i=0; i<p_entities.size(); i++)
	{
		TeslaEffectPiece* effectPiece = static_cast<TeslaEffectPiece*>(p_entities[i]->
			getComponent(ComponentType::TeslaEffectPiece));
		RenderInfo* renderInfo = static_cast<RenderInfo*>(p_entities[i]->getComponent(
			ComponentType::RenderInfo));
		effectPiece->lifeTime -= m_world->getDelta();
		if(effectPiece->lifeTime <= 0.0f)
		{
			//m_world->deleteEntity(p_entities[i]);
			renderInfo->m_shouldBeRendered = false;
		}
		else
		{
			Transform* pieceTransform = static_cast<Transform*>(p_entities[i]->
				getComponent(ComponentType::Transform));
			float scaleFactor = 0.6f;
			if(effectPiece->damages)
			{
				scaleFactor = RandomUtil::randomInterval(1.0f, 5.0f);
			}
			AglVector3 upScale = AglVector3::right() * scaleFactor;
			AglVector3 rightScale = AglVector3::forward() * scaleFactor;
			AglVector3 scale = effectPiece->forwardScale + upScale + rightScale;
			pieceTransform->setScale(scale);
			renderInfo->m_shouldBeRendered = true;
		}
	}
}