#pragma once

#include "Entity.h"

namespace Jellybunny
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnInit() {}
		virtual void OnTick(Timestep time) {}
		virtual void OnDefeat() {}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}