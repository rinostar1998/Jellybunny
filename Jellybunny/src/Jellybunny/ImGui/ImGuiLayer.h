#pragma once
#include "Jellybunny/Core/Layer.h"

#include "Jellybunny/Event/ApplicationEvent.h"
#include "Jellybunny/Event/KeyEvent.h"
#include "Jellybunny/Event/MouseEvent.h"

namespace Jellybunny
{
	class  ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
		float m_time = 0;
	};
}