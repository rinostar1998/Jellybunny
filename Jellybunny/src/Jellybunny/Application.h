#pragma once

#include "Core.h"
#include "Event/Event.h"
#include "Window.h"

namespace Jellybunny
{
	class JELLYBUNNY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// defined in CLIENT
	Application* CreateApplication();
}