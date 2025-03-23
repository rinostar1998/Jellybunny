#pragma once

#include "Jellybunny/Core/Core.h"
#include "Jellybunny/Core/KeyCodes.h"


namespace Jellybunny
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

		static void CaptureMouseCursor();
		static void ReleaseMouseCursor();
	};
}