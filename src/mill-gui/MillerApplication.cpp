#include "MillerApplication.hpp"
#include "Shaders.hpp"
#include "ImGuiBinding.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <cmath>

using namespace std;

MillerApplication::MillerApplication() : 
    _mouseSensitivity(0.05f)
{
}

MillerApplication::~MillerApplication()
{
}

void MillerApplication::onCreate()
{
    ImGuiBinding::initialize(_window, false);

    shared_ptr<VertexShader> vs = make_shared<VertexShader>(
        RESOURCE("shaders/MVPTransformTexCoord.vert")
    );

    shared_ptr<FragmentShader> fs = make_shared<FragmentShader>(
        RESOURCE("shaders/SingleTexture.frag")
    );

    _program = make_shared<ShaderProgram>();
    _program->attach(vs.get());
    _program->attach(fs.get());
    _program->link();

    _texture = loadTextureFromFile(RESOURCE("textures/wefi_cat.jpg"));

    const int heightmapWidth = 64;
    const int heightmapHeight = 64;
    vector<float> heightmap = generateHeightmap(
        heightmapWidth, heightmapHeight
    );

    _heightmapGeo.create(heightmapWidth, heightmapHeight);
    _heightmapGeo.setHeightmap(heightmap);

    _lastMousePosition = getMousePosition();
}

void MillerApplication::onDestroy()
{
    ImGuiBinding::shutdown();
}

void MillerApplication::onUpdate()
{
    ImGuiBinding::newFrame();
    handleInput();

    static bool booleanset;
    
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Simulation"))
        {
            ImGui::MenuItem("Clear simulation", NULL, &booleanset);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ImGui::CollapsingHeader("Window options"))
    {
        static bool flag;
        ImGui::Checkbox("No titlebar", &flag);
    }

    ImGui::End();

    static float f = 0.0f;
    ImGui::Text("Hello world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    
    const int heightmapWidth = 64;
    const int heightmapHeight = 64;
    vector<float> heightmap = generateHeightmap(
        heightmapWidth, heightmapHeight
    );

    _heightmapGeo.setHeightmap(heightmap);
}

void MillerApplication::onRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glUniform1i(glGetUniformLocation(_program->getId(), "TextureSlot1"), 0);

    glUseProgram(_program->getId());

    glm::mat4 model, view, projection;

    float aspectRatio = (float)display_w/display_h;
    model = glm::rotate(
        model,
        (float)glfwGetTime(), 
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    view = _camera.getViewMatrix();
    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);

    GLint modelLoc = glGetUniformLocation(_program->getId(), "model");
    GLint viewLoc = glGetUniformLocation(_program->getId(), "view");
    GLint projLoc = glGetUniformLocation(_program->getId(), "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    _heightmapGeo.render();

    ImGui::Render();
}

void MillerApplication::onMouseButton(int button, int action, int mods)
{
    ImGuiBinding::mouseButtonCallback(_window, button, action, mods);
}

void MillerApplication::onScroll(double xoffset, double yoffset)
{
    ImGuiBinding::scrollCallback(_window, xoffset, yoffset);
}

void MillerApplication::onKey(int key, int scancode, int action, int mods)
{
    ImGuiBinding::keyCallback(_window, key, scancode, action, mods);
}

void MillerApplication::onChar(unsigned int c)
{
    ImGuiBinding::charCallback(_window, c);
}

vector<float> MillerApplication::generateHeightmap(int width, int height)
{
    float phaseShift = glfwGetTime();

    vector<float> heightmap;

    for (int y = 0; y < height; ++y)
    {
        float yfactor = sinf(y*0.2f+phaseShift*0.8f);
        for (int x = 0; x < width; ++x)
        {
            float xfactor = sinf(x*0.1f+phaseShift*1.6f);
            float factor = 0.1f *((xfactor+yfactor) / 2.0f + 1.0f);
            heightmap.push_back(factor);
        }
    }

    return heightmap;
}

void MillerApplication::handleInput()
{
    ImGuiIO &io = ImGui::GetIO();

    auto mousePosition = getMousePosition();
    auto movement = (mousePosition - _lastMousePosition)*_mouseSensitivity;
    _lastMousePosition = mousePosition;

    if (!io.WantCaptureMouse && 
        GLFW_PRESS == glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT))
    {
        _camera.rotate(movement.y, movement.x);
    }
}