#include <glm/glm.hpp>

#pragma once


struct Vertex
{
public:
	glm::vec3 Position;
	glm::vec3 Normals;
	glm::vec2 TexCoords;
	glm::vec3 vRGB;
};