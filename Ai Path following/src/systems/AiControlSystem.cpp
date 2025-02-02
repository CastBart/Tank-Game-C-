#include "systems/AiControlSystem.h"


AiControlSystem::AiControlSystem()
{
}

void AiControlSystem::configure(entityx::EventManager& events)
{
	events.subscribe<EvReportPlayerId>(*this);
	events.subscribe<entityx::ComponentAddedEvent<Ai>>(*this);
	events.subscribe<entityx::ComponentAddedEvent<Wall>>(*this);
	events.subscribe<entityx::ComponentAddedEvent<PathPoint>>(*this);
}

void AiControlSystem::receive(const EvReportPlayerId& e)
{
	m_playerId = e.m_playerId;
}

void AiControlSystem::receive(const entityx::ComponentAddedEvent<Ai>& e)
{
    Ai::Handle ai = e.component;
    m_tankAi.reset(new TankAi(m_obstacles, m_pathPoints, ai->m_id));	// id of turret
}

void AiControlSystem::receive(const entityx::ComponentAddedEvent<Wall>& e)
{
	entityx::Entity ent = e.entity;
	Volume::Handle wallVol = ent.component<Volume>();
	Position::Handle wallPos = ent.component<Position>();
	Display::Handle wallDisplay = ent.component<Display>();
	
	sf::CircleShape circle(wallVol->m_box.getRect().width * 1.5f);
	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(wallPos->m_position);
	m_obstacles.push_back(circle);	
}

void AiControlSystem::receive(const entityx::ComponentAddedEvent<PathPoint>& e)
{
	entityx::Entity ent = e.entity;
	Position::Handle pathPointPos = ent.component<Position>();
	PathPoint::Handle pathPoint = e.component;

	sf::CircleShape circle(pathPoint->m_radius);
	//circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(pathPointPos->m_position);
	m_pathPoints.push_back(circle);
}



void AiControlSystem::update(entityx::EntityManager& entities,
                             entityx::EventManager& events,
                             double dt)
{
   Ai::Handle ai;
   for (entityx::Entity entity : entities.entities_with_components(ai))
   {
	   m_tankAi->update(m_playerId, 
		                    entity.id(),
							events,
							entities, 
							dt);
  
   }
}
