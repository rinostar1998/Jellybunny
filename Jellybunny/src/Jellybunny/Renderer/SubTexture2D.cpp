#include "jbpch.h"
#include "SubTexture2D.h"

namespace Jellybunny
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture)
	{
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& atlas, const glm::vec2& coord, const glm::vec2 cellSize, const glm::vec2& spriteSize)
	{
		glm::vec2 min = { (coord.x * cellSize.x) / atlas->GetWidth(), (coord.y * cellSize.y) / atlas->GetHeight() };
		glm::vec2 max = { ((coord.x + spriteSize.x) * cellSize.x) / atlas->GetWidth(), ((coord.y + spriteSize.y) * cellSize.y) / atlas->GetHeight() };
		return CreateRef<SubTexture2D>(atlas, min, max);
	}

}