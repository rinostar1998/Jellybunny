#include "jbpch.h"
#include "Entity.h"

namespace Jellybunny
{
	Entity::Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene)
	{
	}
}