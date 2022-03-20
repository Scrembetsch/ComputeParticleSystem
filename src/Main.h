#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "glBasics/Camera.h"
#include "glBasics/Material.h"
#include "glBasics/TextRenderer.h"

#include "Device/KeyHandler.h"

#include "Primitives/Plane.h"
#include "ParticleSystem/ParticleSystem.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
void ProcessInput(GLFWwindow* window);

void SetupMembers();
int SetupOpenGL();
void SetupCompute();
bool SetupTextRenderer();

void HandleInput();

void HandlePreFrameLogic();
void RenderLoop();
void HandleEndFrameLogic();

void UpdateScene();
void RenderDefaultPass();
void DrawText();

void UpdateComputeShader();

void RenderScene();
void RenderComputeShader(const glm::mat4& projection, const glm::mat4& view);

void OnExit();
void CreateWindow();

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int CAMERA_WIDTH = 0;
unsigned int CAMERA_HEIGHT = 0;

Camera mCamera;
double mLastX = SCR_WIDTH / 2.0;
double mLastY = SCR_HEIGHT / 2.0;
bool mFirstMouse;

double mDeltaTime;
double mLastFrameTime;

int mShowFPS;

int mFrameCount;
double mFrameTime;

GLFWwindow* mWindow;

TextRenderer mTextRenderer;
KeyHandler mKeyHandler;

ParticleSystem mCsParticles;
int mCompWorkGrpCount[3];
int mCompWorkGrpSize[3];
int mComWorkInvokes;

unsigned int mEmptyVao;