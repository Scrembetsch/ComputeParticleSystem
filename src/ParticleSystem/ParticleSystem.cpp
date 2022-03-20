#include "ParticleSystem.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

ParticleSystem::ParticleSystem()
	: mPosSsbo(0)
	, mVelSsbo(0)
	, mColSsbo(0)
	, mVao(0)
	, mLocalWorkGroupSize(cNumGroupSize, 1, 1)
{

}

ParticleSystem::~ParticleSystem()
{
	if (mPosSsbo != 0)
	{
		glDeleteBuffers(1, &mPosSsbo);
		mPosSsbo = 0;
	}
	if (mVelSsbo != 0)
	{
		glDeleteBuffers(1, &mVelSsbo);
		mVelSsbo = 0;
	}
	if (mColSsbo != 0)
	{
		glDeleteBuffers(1, &mColSsbo);
		mColSsbo = 0;
	}
	if (mVao != 0)
	{
		glDeleteVertexArrays(1, &mVao);
		mVao = 0;
	}
}

void ParticleSystem::Init()
{
	std::cout << "Size: " << sizeof(glm::vec4) << std::endl;

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mPosSsbo);
	glBindVertexArray(mVao);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPosSsbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cNumParticles * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

	glm::vec4* points = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, cNumParticles * sizeof(glm::vec4), bufMask);
	for (int i = 0; i < cNumParticles; i++)
	{
		points[i].x = 0.0f;
		points[i].y = 0.0f;
		points[i].z = 0.0f;
		points[i].w = 1.0f;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &mVelSsbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVelSsbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cNumParticles * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);

	glm::vec4* vels = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, cNumParticles * sizeof(glm::vec4), bufMask);
	for (int i = 0; i < cNumParticles; i++)
	{
		vels[i].x = float(rand()) / RAND_MAX * 5.0f - 2.5f;
		vels[i].y = float(rand()) / RAND_MAX * 5.0f - 2.5f;
		vels[i].z = float(rand()) / RAND_MAX * 5.0f - 2.5f;
		vels[i].w = 1.0f;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &mColSsbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mColSsbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cNumParticles * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);

	glm::vec4* cols = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, cNumParticles * sizeof(glm::vec4), bufMask);
	for (int i = 0; i < cNumParticles; i++)
	{
		cols[i].x = float(rand()) / RAND_MAX;
		cols[i].y = float(rand()) / RAND_MAX;
		cols[i].z = float(rand()) / RAND_MAX;
		cols[i].w = 1.0f;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	mComputeShader.loadCompute("shader/Compute/basic.cs", "LOCAL_WORK_GROUP_SIZE", mLocalWorkGroupSize);
	mRenderShader.load("shader/Compute/render.vs", "shader/Compute/render.fs");
}

void ParticleSystem::Update()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, mPosSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, mVelSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, mColSsbo);

	mComputeShader.use();
	glDispatchCompute(cNumParticles / mLocalWorkGroupSize.x, 1, 1);

	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::Render(glm::mat4 proj, glm::mat4 view)
{
	mRenderShader.use();
	mRenderShader.setMat4("uProjection", proj);
	mRenderShader.setMat4("uView", view);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mPosSsbo);
	//glVertexPointer(4, GL_FLOAT, 0, (void*)0);
	//glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, cNumParticles);
	//glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
