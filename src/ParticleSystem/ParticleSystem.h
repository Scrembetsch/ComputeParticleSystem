#pragma once

#include <glm/glm.hpp>
#include "../glBasics/Shader.h"

class ParticleSystem
{
private:
	const unsigned int cNumParticles = 1024 * 124;
	const unsigned int cNumGroupSize = 128;

public:
	ParticleSystem();
	~ParticleSystem();

	void Init();
	void Update();
	void Render(glm::mat4 proj, glm::mat4 view);

private:
	GLuint mPosSsbo;
	GLuint mVelSsbo;
	GLuint mColSsbo;

	GLuint mVao;

	Shader mComputeShader;
	Shader mRenderShader;

	glm::vec3 mLocalWorkGroupSize;
};