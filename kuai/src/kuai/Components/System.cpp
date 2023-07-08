#include "kpch.h"
#include "System.h"

#include "Entity.h"

namespace kuai {

	void System::insertEntity(EntityID id)
	{
		entities.push_back(Entity(ECS, id));
	}

	void System::removeEntity(EntityID id)
	{
		for (size_t i = 0; i < entities.size(); i++)
		{
			if (entities[i].getId() == id)
			{
				entities.erase(entities.begin() + i);
				break;
			}
		}
	}

	void System::acceptSubset(bool val)
	{
		acceptsSubset = val;
	}

	std::vector<Entity>& System::getEntities()
	{
		return entities;
	}

	bool System::hasEntity(EntityID id)
	{
		for (size_t i = 0; i < entities.size(); i++)
		{
			if (entities[i].getId() == id)
			{
				return true;
			}
		}
		return false;
	}
}