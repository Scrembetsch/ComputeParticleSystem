#pragma once

#include <glm/glm.hpp>
#include "../glBasics/Shader.h"
#include "../glBasics/Texture2D.h"

class ParticleSystem
{
private:
	const unsigned int cNumParticles = 1024 * 124;
	const unsigned int cNumGroupSize = 128;

public:
	ParticleSystem();
	~ParticleSystem();

	void Init();
	void SetMatrices(const glm::vec3& frontVec, const glm::vec3& upVec);
	void Update(float dt);
	void Render(glm::mat4 proj, glm::mat4 view);

private:
	GLuint mPosSsbo;
	GLuint mVelSsbo;
	GLuint mColSsbo;

	GLuint mVao;

	Texture2D mTex;

	Shader mComputeShader;
	Shader mRenderShader;

	glm::vec3 mQuad1;
	glm::vec3 mQuad2;

	glm::vec3 mLocalWorkGroupSize;
};