#pragma once
#include "Jellybunny/Core/Layer.h"

#include "Jellybunny/Event/ApplicationEvent.h"
#include "Jellybunny/Event/KeyEvent.h"
#include "Jellybunny/Event/MouseEvent.h"

namespace Jellybunny
{
	class JELLYBUNNY_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();
	private:
		float m_time = 0;
	};
}