#pragma once

#include "Core.h"

namespace Jellybunny
{
	class JELLYBUNNY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// defined in CLIENT
	Application* CreateApplication();
}