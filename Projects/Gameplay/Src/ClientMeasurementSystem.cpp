#include "ClientMeasurementSystem.h"
#include <AntTweakBarWrapper.h>
#include "MeshRenderSystem.h"

ClientMeasurementSystem::ClientMeasurementSystem()
	: EntitySystem( SystemType::ClientMeasurementSystem )
{
}

ClientMeasurementSystem::~ClientMeasurementSystem()
{
}

void ClientMeasurementSystem::process()
{
}

void ClientMeasurementSystem::initialize()
{
	initMeasuredSystems();

	AntTweakBarWrapper::getInstance()->modifyTheRefreshRate(
		AntTweakBarWrapper::MEASUREMENT, 0.1f );
	AntTweakBarWrapper::getInstance()->defineParametersForABar(
		AntTweakBarStrings::measurement.c_str(), " valueswidth=120");

	vector<EntitySystem*> systems = m_world->getSystems()->getSystemList();
	for(unsigned int i=0; i<m_measuredSystems.size(); i++)
	{
		AntTweakBarWrapper::getInstance()->addReadOnlyVariable(
			AntTweakBarWrapper::MEASUREMENT, m_measuredSystems[i].second.c_str(),
			TwType::TW_TYPE_DOUBLE, &m_measuredSystems[i].first->getTimeUsedPerSecond(),
			"group='Per second ratio' precision='6'" );
		AntTweakBarWrapper::getInstance()->addReadOnlyVariable(
			AntTweakBarWrapper::MEASUREMENT, (m_measuredSystems[i].second+" ").c_str(),
			TwType::TW_TYPE_DOUBLE, &m_measuredSystems[i].first->getAverageExecutionTime(),
			"group='Average process execution time (ms)' precision='4'" );
	}

	//Culling
	MeshRenderSystem* mrs = static_cast<MeshRenderSystem*>(m_world->getSystem(SystemType::RenderPrepSystem));

	AntTweakBarWrapper::getInstance()->addReadOnlyVariable(
		AntTweakBarWrapper::MEASUREMENT, "Rendered: ",
		TwType::TW_TYPE_UINT32, mrs->getRenderedCountPtr(),
		"group='Culling'" );

	AntTweakBarWrapper::getInstance()->addReadOnlyVariable(
		AntTweakBarWrapper::MEASUREMENT, "Culled: ",
		TwType::TW_TYPE_UINT32, mrs->getCulledCountPtr(),
		"group='Culling'" );
	 
	AntTweakBarWrapper::getInstance()->addReadOnlyVariable(
		AntTweakBarWrapper::MEASUREMENT, "Culled Frac: ",
		TwType::TW_TYPE_FLOAT, mrs->getCulledFractionPtr(),
		"group='Culling'" );
}

void ClientMeasurementSystem::initMeasuredSystems()
{
	m_measuredSystems.push_back(pair<EntitySystem*, string>(
		m_world->getSystem(SystemType::ClientPacketHandlerSystem),
		"ClientPacketHandler"));
	m_measuredSystems.push_back(pair<EntitySystem*, string>(
		m_world->getSystem(SystemType::GraphicsRendererSystem),
		"GraphicsRenderer"));
	m_measuredSystems.push_back(pair<EntitySystem*, string>(
		m_world->getSystem(SystemType::RenderPrepSystem),
		"InstanceBaking"));
	m_measuredSystems.push_back(pair<EntitySystem*, string>(
		m_world->getSystem(SystemType::ShieldPlatingSystem),
		"ShieldPlating"));
	m_measuredSystems.push_back(pair<EntitySystem*, string>(
		m_world->getSystem(SystemType::TransformParentHandlerSystem),
		"TransformHierarchy"));
}