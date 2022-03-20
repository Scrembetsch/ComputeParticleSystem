#include "Main.h"

#include <iostream>
#include <filesystem>

#include "glBasics/Texture2D.h"
#include "glBasics/Texture3D.h"

#include "Util/Util.h"

int main(int argc, char** argv)
{
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    SetupMembers();
    CreateWindow();

    while (!glfwWindowShouldClose(mWindow))
    {
        RenderLoop();
    }

    OnExit();
    return 0;
}

void SetupMembers()
{
    mCamera.Position = glm::vec3(-2.57005f, 16.0659f, -31.8907f);
    mCamera.Yaw = 90.0f;
    mCamera.Pitch = -18.0f;
    mCamera.updateCameraVectors();
    mShowFPS = 0;
    mFrameCount = 0;
    mFrameTime = 0;
    mLastFrameTime = 0;
    mFirstMouse = true;
}

int SetupOpenGL()
{
    // GLFW Init & Config
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW Window create
    mWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SPG", NULL, NULL);
    if (mWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, FramebufferSizeCallback);
    glfwSetCursorPosCallback(mWindow, MouseCallback);
    glfwSetScrollCallback(mWindow, ScrollCallback);
    glfwSetKeyCallback(mWindow, KeyCallback);
    glfwSetMouseButtonCallback(mWindow, MouseClickCallback);

    // Capture mouse
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: Load all OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    return 0;
}

bool SetupTextRenderer()
{
    mTextRenderer.LoadShader("shader/Text/text.vs", "shader/Text/text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    mTextRenderer.SetProjection(projection, "uProjection");
    return mTextRenderer.SetupFreetype(std::filesystem::absolute("data/fonts/Consolas.ttf").string());
}

void SetupCompute()
{
    // Get Work group count
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &mCompWorkGrpCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &mCompWorkGrpCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &mCompWorkGrpCount[2]);

    std::cout << "Work Group Count: " << mCompWorkGrpCount[0] << ", " << mCompWorkGrpCount[1] << ", " << mCompWorkGrpCount[2] << std::endl;

    // Get Work group size
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &mCompWorkGrpSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &mCompWorkGrpSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &mCompWorkGrpSize[2]);

    std::cout << "Work Group Size: " << mCompWorkGrpSize[0] << ", " << mCompWorkGrpSize[1] << ", " << mCompWorkGrpSize[2] << std::endl;

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &mComWorkInvokes);
    std::cout << "Max Invokes: " << mComWorkInvokes << std::endl;

    mCsParticles.Init();
}

void RenderLoop()
{
    // Per frame logic
    HandlePreFrameLogic();
    HandleInput();

    UpdateScene();

    RenderDefaultPass();

    DrawText();

    // Frame End Updates
    HandleEndFrameLogic();
}

void HandleEndFrameLogic()
{
    mKeyHandler.FrameUpdate();
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

void HandlePreFrameLogic()
{
    if (mLastFrameTime == 0)
    {
        mLastFrameTime = glfwGetTime();
        mDeltaTime = 0.0f;
        return;
    }
    double currentFrame = glfwGetTime();
    mDeltaTime = currentFrame - mLastFrameTime;
    mLastFrameTime = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Util::PrintErrors();
}

void HandleInput()
{
    ProcessInput(mWindow);
}

void UpdateScene()
{
    UpdateComputeShader();
}

void UpdateComputeShader()
{
    mCsParticles.Update();
}

void RenderDefaultPass()
{
    CAMERA_WIDTH = SCR_WIDTH;
    CAMERA_HEIGHT = SCR_HEIGHT;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderScene();
}

void DrawText()
{
    glEnable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    std::string text;
    text += "Camera Speed:  \t  \t" + std::to_string(mCamera.MovementSpeed) + "\n";
    text += "Current Height:\t  \t" + std::to_string(mCamera.Position.y) + "\n";
    mTextRenderer.RenderFormattedText(text, 5.0f, SCR_HEIGHT - 20.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f), 0.1f);

    if (mFrameTime >= 0.5)
    {
        mShowFPS = static_cast<int>(mFrameCount / mFrameTime);
        mFrameTime = 0;
        mFrameCount = 0;
    }
    mFrameTime += mDeltaTime;
    mFrameCount++;

    text = "FPS:" + std::to_string(mShowFPS);
    mTextRenderer.RenderText(text, SCR_WIDTH - 100.0f, SCR_HEIGHT - 20.0f, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));

    text = "+";
    mTextRenderer.RenderText(text, SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    glDisable(GL_BLEND);
}

void RenderScene()
{
    glm::mat4 projection = glm::perspective(glm::radians(mCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
    glm::mat4 view = mCamera.GetViewMatrix();

    RenderComputeShader(projection, view);
}

void RenderComputeShader(const glm::mat4& projection, const glm::mat4& view)
{
    mCsParticles.Render(projection, view);
}

void CreateWindow()
{
    SetupOpenGL();

    SetupCompute();

    SetupTextRenderer();
}

void OnExit()
{
    glfwTerminate();
}

// Process input
void ProcessInput(GLFWwindow* window)
{
    float delta = static_cast<float>(mDeltaTime);

    // Close window
    if (mKeyHandler.WasKeyPressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Handle Camera movment
    if (mKeyHandler.IsKeyDown(GLFW_KEY_W))
    {
        mCamera.ProcessKeyboard(Camera::Camera_Movement::FORWARD, delta);
    }
    if (mKeyHandler.IsKeyDown(GLFW_KEY_S))
    {
        mCamera.ProcessKeyboard(Camera::Camera_Movement::BACKWARD, delta);
    }
    if (mKeyHandler.IsKeyDown(GLFW_KEY_A))
    {
        mCamera.ProcessKeyboard(Camera::Camera_Movement::LEFT, delta);
    }
    if (mKeyHandler.IsKeyDown(GLFW_KEY_D))
    {
        mCamera.ProcessKeyboard(Camera::Camera_Movement::RIGHT, delta);
    }
    if (mKeyHandler.IsKeyDown(GLFW_KEY_SPACE))
    {
        mCamera.ProcessKeyboard(Camera::Camera_Movement::UP, delta);
    }
    if (mKeyHandler.IsKeyDown(GLFW_KEY_LEFT_CONTROL))
    {
        mCamera.ProcessKeyboard(Camera::Camera_Movement::DOWN, delta);
    }

    if (mKeyHandler.IsKeyDown(GLFW_KEY_KP_ADD))
    {
        mCamera.MovementSpeed += delta * 5;
    }

    if (mKeyHandler.IsKeyDown(GLFW_KEY_KP_SUBTRACT))
    {
        mCamera.MovementSpeed -= delta * 5;
    }

    if (mKeyHandler.WasKeyReleased(GLFW_KEY_I))
    {
        std::cout << mCamera.Position.x << "f, " << mCamera.Position.y << "f, " <<mCamera.Position.z << "f" << std::endl;
        std::cout << mCamera.Rotation.w << "f, " << mCamera.Rotation.x << "f, " << mCamera.Rotation.y << "f, " << mCamera.Rotation.z << "f" << std::endl;
        std::cout << "Yaw: " << mCamera.Yaw << ", Pitch: " << mCamera.Pitch << std::endl;
    }
}

void MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (action)
    {
        case GLFW_PRESS:
            mKeyHandler.KeyPressed(key);
            break;

        case GLFW_RELEASE:
            mKeyHandler.KeyReleased(key);
            break;

        default:
            break;
    }
}

// GLFW Callback when window changes
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// GLFW Callback when mouse moves
void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (mFirstMouse)
    {
        mLastX = xpos;
        mLastY = ypos;
        mFirstMouse = false;
    }

    double xoffset = xpos - mLastX;
    double yoffset = mLastY - ypos; // Reverse: y-coordinates go from bottom to top

    mLastX = xpos;
    mLastY = ypos;

    mCamera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

// GLFW Callback when mousewheel is used
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    mCamera.ProcessMouseScroll(static_cast<float>(yoffset));
}
