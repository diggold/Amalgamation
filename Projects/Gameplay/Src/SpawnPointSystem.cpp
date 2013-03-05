#define FORCE_VS_DBG_OUTPUT

#include "SpawnPointSystem.h"
#include "SpawnPointSet.h"
#include "EntityFactory.h"
#include "LevelHandlerSystem.h"
#include "LevelPieceRoot.h"
#include "Transform.h"
#include "TimerSystem.h"

#include <ToString.h>
#include <DebugUtil.h>

SpawnPointSystem::SpawnPointSystem()
	: EntitySystem(SystemType::SpawnPointSystem, 3, ComponentType::SpawnPointSet, 
													ComponentType::LevelPieceRoot,
													ComponentType::Transform)
{
}

SpawnPointSystem::~SpawnPointSystem()
{

}

void SpawnPointSystem::initialize()
{
	m_levelHandler = static_cast<LevelHandlerSystem*>(
		m_world->getSystem(SystemType::LevelHandlerSystem));

	m_timerSystem = static_cast<TimerSystem*>(
		m_world->getSystem(SystemType::TimerSystem));

}

void SpawnPointSystem::clearSpawnPoints()
{
	m_shipSpawnPoints.clear();
	m_moduleSpawnPoints.clear();
}

void SpawnPointSystem::inserted( Entity* p_entity )
{
	auto spawnPointSet = static_cast<SpawnPointSet*>(
		p_entity->getComponent(ComponentType::SpawnPointSet));
	
	auto pieceRoot = static_cast<LevelPieceRoot*>(
		p_entity->getComponent(ComponentType::LevelPieceRoot));

	int pieceId = pieceRoot->pieceId;
	if (pieceId >= 0)
	{
		vector<ShipSpawnPointData> newShipSpawnPoints;
		vector<ModuleSpawnPointData> newModuleSpawnPoints;

		DEBUGPRINT(( (toString("Starting to read spawnpoint of chamber ") + toString(pieceId) + " " + p_entity->getName() + "\n").c_str()) );
		auto rootTransform = static_cast<Transform*>(
			p_entity->getComponent(ComponentType::Transform));

		// Resize the spawnpoint vectors to make this chamber fit.
		if (pieceRoot->pieceId >= m_shipSpawnPoints.size())
		{
			m_shipSpawnPoints.resize(pieceId + 1);
			m_moduleSpawnPoints.resize(pieceId + 1);
		}

		for (unsigned int i = 0; i < spawnPointSet->m_spawnPoints.size(); i++)
		{
			SpawnPoint spawnPoint = spawnPointSet->m_spawnPoints[i];
			// Transform spawnPoint to world space.
			spawnPoint.spTransform *= rootTransform->getMatrix();

			if (spawnPoint.spAction == "ship")
			{
				AglVector3 posV = spawnPoint.spTransform.GetTranslation();
				string posAsString = toString(posV.x) + " " + toString(posV.y) + " " + toString(posV.z) + "\n";

				//DEBUGPRINT(( (toString("Ship spawnpoint found at position ") + posAsString).c_str()) );
				newShipSpawnPoints.push_back(ShipSpawnPointData(spawnPoint.spTransform, pieceRoot->pieceId, newShipSpawnPoints.size()));
			}
			else if (spawnPoint.spAction == "module")
			{
				AglVector3 posV = spawnPoint.spTransform.GetTranslation();
				string posAsString = toString(posV.x) + " " + toString(posV.y) + " " + toString(posV.z) + "\n";

				//DEBUGPRINT(( (toString("Module spawnpoint found at position ") + posAsString).c_str()) );

				newModuleSpawnPoints.push_back(ModuleSpawnPointData(spawnPoint.spTransform, pieceRoot->pieceId, newModuleSpawnPoints.size()));
			}
		}
		m_shipSpawnPoints[pieceId]		= newShipSpawnPoints;
		m_moduleSpawnPoints[pieceId]	= newModuleSpawnPoints;
		
		// Remove the spawn point set, so that it cannot be used by other systems
		p_entity->removeComponent(ComponentType::SpawnPointSet);
		p_entity->applyComponentChanges();

		string debugPrintStr = "Found ";
		debugPrintStr += toString(newShipSpawnPoints.size());
		debugPrintStr += " ship spawnpoints.\nFound ";
		debugPrintStr += toString(newModuleSpawnPoints.size());
		debugPrintStr += " module spawnpoints.\n";

		DEBUGPRINT((debugPrintStr.c_str()));
	}
	else
	{
		DEBUGPRINT(("Warning: SpawnPointSystem received an entity that contains an invalid levelId.\n"));
	}
}

void SpawnPointSystem::processEntities( const vector<Entity*>& p_entities )
{
	if (m_timerSystem->checkTimeInterval(TimerIntervals::HalfSecond))
	{
		for (unsigned int spawnPointId = 0; spawnPointId < m_refreshingSpawnPoints.size(); spawnPointId++)
		{
			m_refreshingSpawnPoints[spawnPointId].currentCooldown -= 0.5f;
			
		}
	}
}

const AglMatrix& SpawnPointSystem::getRandomFreeShipSpawnPoint()
{
	auto selectId = getRandomFreeSpawnPoint(m_shipSpawnPoints);

	if (selectId.first >= 0 && selectId.second >= 0)
	{
		setSpawnPointState(m_shipSpawnPoints, selectId.first, selectId.second, SPAWNPOINTSTATE_OCCUPIED);
		return m_shipSpawnPoints[selectId.first][selectId.second].transform;
	}
	else
	{
		//DEBUGPRINT(("Warning: Found no available ship spawnpoint. An invalidSpawnPoint is returned.\n"));
		return invalidSpawnPoint();
	}
}

const AglMatrix& SpawnPointSystem::getRandomFreeModuleSpawnPoint()
{
	return getRandomFreeModuleSpawnPointData().transform;
}

ModuleSpawnPointData SpawnPointSystem::getRandomFreeModuleSpawnPointData()
{
	auto selectId = getRandomFreeSpawnPoint(m_moduleSpawnPoints);

	if (selectId.first >= 0 && selectId.second >= 0)
	{
		setSpawnPointState(m_moduleSpawnPoints, selectId.first, selectId.second, SPAWNPOINTSTATE_OCCUPIED);
		return m_moduleSpawnPoints[selectId.first][selectId.second];
	}
	else
	{
		//DEBUGPRINT(("Warning: Found no available module spawnpoint data. An invalidSpawnPointData is returned.\n"));
		return s_invalidSpawnPointData;
	}
}

pair<int, int> SpawnPointSystem::getRandomFreeSpawnPoint( 
	const vector<vector<ModuleOrShipSpawnPointData>>& p_fromSpawnPoints ) const
{
	vector<pair<int, int>> selection;

	for (int chamber = 0; chamber < p_fromSpawnPoints.size(); chamber++)
	{
		for (int point = 0; point < p_fromSpawnPoints[chamber].size(); point++)
		{
			if (p_fromSpawnPoints[chamber][point].state == SPAWNPOINTSTATE_FREE)
			{
				selection.push_back(make_pair(chamber, point));
			}
		}
	}

	if (selection.size() > 0)
	{
		int randIndex = rand() % selection.size();
		return make_pair(selection[randIndex].first, selection[randIndex].second);
	}
	else
		return make_pair(-1, -1);
}

void SpawnPointSystem::setSpawnPointState( 
	vector<vector<ModuleOrShipSpawnPointData>>& p_inSpawnPoints, 
	int p_inChamber, int p_inPoint, SpawnPointState p_newState )
{
	p_inSpawnPoints[p_inChamber][p_inPoint].state = p_newState;
}

bool SpawnPointSystem::isSpawnPointsReady() const
{
	return m_levelHandler->hasLevel();
}

const AglMatrix& SpawnPointSystem::invalidSpawnPoint()
{
	return s_invalidSpawnPoint;
}

void SpawnPointSystem::applyResetCooldown( int p_atChamber, int p_atSpawnPoint )
{
	ModuleSpawnPointData spawnPoint = m_moduleSpawnPoints[p_atChamber][p_atSpawnPoint];
	spawnPoint.currentCooldown = 10.0f;
	m_refreshingSpawnPoints.push_back(spawnPoint);
}




const AglMatrix SpawnPointSystem::s_invalidSpawnPoint(AglVector3::one(), 
													AglQuaternion::identity(), 
													AglVector3(FLT_MAX, FLT_MAX, FLT_MAX));

const ModuleOrShipSpawnPointData SpawnPointSystem::s_invalidSpawnPointData(s_invalidSpawnPoint, -1, -1);






