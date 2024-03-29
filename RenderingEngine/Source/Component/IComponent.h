#pragma once

namespace NamelessEngine::DX12API {
	class RenderingContext;
}

namespace NamelessEngine::Component
{
	enum class COMPONENTID
	{
		DRAW = 0,
		TRANSFORM = 1,
		COLLISION = 2
	};

	class IComponent {
	public:
		virtual ~IComponent() {};
		virtual void Update(float deltatime) = 0;
		virtual void Draw(DX12API::RenderingContext& renderContext) = 0;
	};
}