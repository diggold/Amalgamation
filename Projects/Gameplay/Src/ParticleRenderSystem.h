#pragma once
#include <EntitySystem.h>

using namespace std;

class GraphicsBackendSystem;
class AglParticleSystem;
// =======================================================================================
//                                      ParticleRenderSystem
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Brief
///        
/// # ParticleRenderSystem
/// Detailed description.....
/// Created on: 15-1-2013 
///---------------------------------------------------------------------------------------

class ParticleRenderSystem : public EntitySystem
{
public:
	ParticleRenderSystem( GraphicsBackendSystem* p_gfxBackend );
	~ParticleRenderSystem();

	void processEntities( const vector<Entity*>& p_entities );

private:
	void renderParticles(AglParticleSystem* particleSystem);
	void rebuildVertexBuffer(AglParticleSystem* particleSystem);
private:
	GraphicsBackendSystem* m_gfxBackend;
};

