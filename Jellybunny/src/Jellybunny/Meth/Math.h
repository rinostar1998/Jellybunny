#pragma once
#include <glm/glm.hpp>

namespace Jellybunny { namespace Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale);
} }