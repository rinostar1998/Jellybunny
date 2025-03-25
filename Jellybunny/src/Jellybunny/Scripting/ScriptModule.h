#pragma once

namespace Jellybunny
{
	class ScriptModule
	{
	public:
		static void Init();
		static void Die();
	private:
		static void InitMono();
		static void KillMono();
	};
}