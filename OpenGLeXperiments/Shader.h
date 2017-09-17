#pragma once

#include "stdafx.h"

class Shader
{
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geomPath, const GLchar* tcsPath, const GLchar* tesPath);
	void Use() const;

	
};

extern const GLuint PositionSlot;
