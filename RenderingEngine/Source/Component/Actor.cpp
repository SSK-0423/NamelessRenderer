#include "Actor.h"

namespace NamelessEngine
{
	Actor::Actor(const std::string& name) : _actorName(name)
	{
	}
	Actor::~Actor()
	{
		for (auto component : _components) {
			delete component;
		}
	}

	void Actor::Update(float deltaTime)
	{
		for (auto component : _components) {
			component->Update(deltaTime);
		}
	}

	void Actor::Draw(DX12API::RenderingContext& renderContext)
	{
		for (auto component : _components) {
			component->Draw(renderContext);
		}
	}
}