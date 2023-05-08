#include "kpch.h"
#include "Layer.h"

#include "kuai/Components/Components.h"

namespace kuai {

	Layer::Layer(const std::string& debugName)
	{
		this->name = debugName;

		ECS = MakeBox<EntityComponentSystem>();

		ECS->registerComponent<Transform>();
		ECS->registerComponent<Cam>();
		ECS->registerComponent<Light>();
		ECS->registerComponent<MeshRenderer>();
		ECS->registerComponent<AudioListener>();
		ECS->registerComponent<AudioSourceComponent>();
	}

	Layer::~Layer()
	{

	}

}