#pragma once

namespace Jellybunny
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f, float globalTime = 0.0f) : m_Time(time), m_GlobalTime(globalTime)
		{
		}
		float DeltaTime() const { return m_Time; }
		float DeltaMilliseconds() const { return m_Time * 1000.0f; }
		float GlobalSeconds() const { return m_GlobalTime; }
		float GlobalMilliseconds() const { return m_Time * 1000.0f; }
		float GetFPS() const { return 60.0f / m_Time; }
	private:
		float m_Time;
		float m_GlobalTime;
	};
}