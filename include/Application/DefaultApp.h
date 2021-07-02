#pragma once
#include "../Application.h"
#include "../Shader.h"
#include "../Matrix.h"
#include "../Camera.h"
#include "../Prefab.h"
#include "../Object/IndexObject.h"
#include "../Texture.h"


namespace psi {

class DefaultApp : public Application {
public:
    DefaultApp(int w, int h, const std::string& mTitle = "window", bool fullScreen = false) : Application(w, h, mTitle, fullScreen) {}
    virtual ~DefaultApp() = default;
private:
    virtual void InitContext() override {
        Application::InitContext();
        glEnable(GL_DEPTH_TEST);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    virtual void InitData() override {
        // load data
        float vertices[] = {
         0.5f,  0.5f, 0.0f, 1.f, 1.f, //  1.f, 1.f, 1.f,// top right
         0.5f, -0.5f, 0.0f, 1.f, 0.f, //  1.f, 0.5f, 0.f,// bottom right
        -0.5f, -0.5f, 0.0f, 0.f, 0.f, //  0.f, 0.f, 0.f,// bottom left
        -0.5f,  0.5f, 0.0f, 0.f, 1.f//,  0.f, 0.5f, 1.f// top left 
        };
        unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
        };

        objects.push_back(New<IndexObject>(vertices, sizeof(vertices), std::initializer_list<int>{3, 2}, indices, sizeof(indices)));
        objects.push_back(New<Object>(Prefab::cube, sizeof(Prefab::cube), std::initializer_list<int>{3}));
        objects.push_back(New<Object>(Prefab::cubeWithNormal, sizeof(Prefab::cubeWithNormal), std::initializer_list<int>{3, 3}));
        objects.push_back(New<Object>(Prefab::cubeWithNormalAndUV, sizeof(Prefab::cubeWithNormalAndUV), std::initializer_list<int>{3, 3, 2}));

        textures.push_back(New<Texture>("Texture/wall.jpg"));
        textures.push_back(New<Texture>("Texture/container.png"));
        textures.push_back(New<Texture>("Texture/container_specular.png", false));

        shaders.push_back(New<Shader>("Shader/default.vert", "Shader/default.frag"));
        shaders.push_back(New<Shader>("Shader/light.vert", "Shader/light.frag"));
        shaders.push_back(New<Shader>("Shader/BlinnPhong.vert", "Shader/BlinnPhong.frag"));
        shaders.push_back(New<Shader>("Shader/lightMap.vert", "Shader/lightMap.frag"));
        shaders.push_back(New<Shader>("Shader/moreLight.vert", "Shader/moreLight.frag"));
    }

    //unsigned VAO = 0;
    std::vector<Ptr<Object>> objects;
    std::vector<Ptr<Texture>> textures;
    std::vector<Ptr<Shader>> shaders;

    float deltaTime = 0.f;
    Camera camera = Camera(Vec3f(0.f, 0.f, 5.f), Vec3f(0.f, 1.f, 0.f), 180.f, 0.f);

    virtual void RunLoop() override {
        float fov = 70.0f;
        float lastTime = (float)glfwGetTime();

        Vec3f ka(0.03f, 0.03f, 0.03f);
        Vec3f ks(0.8f, 0.8f, 0.8f);
        Vec3f kd(1.0f, 0.5f, 0.31f);
        float shininess = 64;
        
        Vec3f lightPos(0.f, 3.f, 0.f);
        Vec3f lightColorBase(1.f, 1.f, 1.f);
        float lightColorCoef = 2.5f;

        Vec3f background = Vec3f(0.f, 0.f, 0.f);
        Vec3f skyDir = Vec3f(0.f, -1.f, 0.1f);
        float skyColorCoef = 2.5f;

        bool enableSpotLight = true;
        Vec3f spotColor = Vec3f(1.f, 1.f, 1.f);
        float spotColorCoef = 2.5f;
        float innerAngle = 25.f;
        float outerAngle = 30.f;
        //Vec3f foreground = Vec3f(0.3f, 0.4f, 0.8f);
        while (!glfwWindowShouldClose(window)) {
            float curTime = (float)glfwGetTime();
            deltaTime = curTime - lastTime;
            lastTime = curTime;
            glfwPollEvents();
            glClearColor(background.x(), background.y(), background.z(), 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // set imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            ImGui::DragFloat("moving speed", &camera.cameraSpeed, 0.01f, 0.1f, 10.f);
            ImGui::DragFloat("mouse sensitivity", &camera.mouseSensitivity, 0.005f, 0.0f, 0.3f);
            ImGui::DragFloat("fov", &fov, 1.0f, 1.f, 179.0f);
            //ImGui::ColorEdit3("foreground color", (float*)&foreground);
            ImGui::End();

            ImGui::Begin("Information");
            ImGui::Text("Fps:\t%f", 1.f / deltaTime);
            ImGui::End();

            ImGui::Begin("Object Properties");
            ImGui::DragFloat3("ka", ka.data(), 0.01f, 0.f, 1.f);
            //ImGui::ColorEdit3("kd", kd.data());
            //ImGui::DragFloat3("ks", ks.data(), 0.01f, 0.f, 1.f);
            ImGui::DragFloat("shininess", &shininess, 1.f, 0.f, 1024.f);
            ImGui::DragFloat3("lightPos", lightPos.data(), 0.1f, -10.f, 10.f);
            ImGui::ColorEdit3("lightColorBase", lightColorBase.data());
            ImGui::DragFloat("lightColorCoef", &lightColorCoef, 0.1f, 0.f, 100.f);
            ImGui::DragFloat3("skyLightDir", skyDir.data(), 0.01f, -1.f, 1.f);
            ImGui::ColorEdit3("skyLightColor", background.data());
            ImGui::DragFloat("skyLightCoef", &skyColorCoef, 0.1f, 0.f, 100.f);
            ImGui::Checkbox("enableSpotLight", &enableSpotLight);
            ImGui::ColorEdit3("spotLightColor", spotColor.data());
            ImGui::DragFloat("spotLightCoef", &spotColorCoef, 0.1f, 0.f, 100.f);
            ImGui::DragFloat("innerAngle", &innerAngle, 0.1f, 0.f, 180.f);
            ImGui::DragFloat("outerAngle", &outerAngle, 0.1f, 0.f, 180.f);
            ImGui::End();

            // rendering code

            Mat4f model = Matrix::Translation(Vec3f(0.f, 0.f, -1.f)) * Matrix::Rotation(Vec3f(0.f, 1.f, 0.f), 45.f);
            Mat4f projection = Matrix::Projection(fov, (float)width / (float)height, 0.1f, 100.f);
            Mat4f normalTransform = model.inverse().transpose();

            shaders[4]->Use();
            shaders[4]->SetMat4f("model", model)
                .SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection)
                .SetMat4f("normalTransform", normalTransform)
                .SetVec3f("ka", ka)
                .SetVec3f("ks", ks)
                .SetFloat("shininess", shininess)
                .SetVec3f("pointLight.pos", lightPos)
                .SetVec3f("pointLight.color", lightColorBase * lightColorCoef)
                .SetVec3f("skyLight.dir", skyDir)
                .SetVec3f("skyLight.color", background * skyColorCoef)
                .SetVec3f("spotLight.pos", camera.pos)
                .SetVec3f("spotLight.dir", camera.front)
                .SetVec3f("spotLight.color", spotColor * spotColorCoef)
                .SetFloat("spotLight.innerCos", cos(deg2rad(innerAngle)))
                .SetFloat("spotLight.outerCos", cos(deg2rad(outerAngle)))
                .SetInt("enableSpotLight", enableSpotLight)
                .SetVec3f("viewPos", camera.pos)
                .SetInt("kd", 0)
                .SetInt("ks", 1);
            textures[1]->Use(0);
            textures[2]->Use(1);
            objects[3]->Draw();

            model = Matrix::Translation(lightPos) * Matrix::Scale(Vec3f(0.1f, 0.1f, 0.1f));
            shaders[1]->Use();
            shaders[1]->SetMat4f("model", model)
                .SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection)
                .SetVec3f("color", lightColorBase);
            objects[1]->Draw();



            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);    // ½»»»Ë«»º³å
            
            ProcessInput();
        }
    }

    bool EnableViewRotation = true;
    bool isInputModeChanged = false;
    void ProcessInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (!isInputModeChanged) {
                int inputMode = glfwGetInputMode(window, GLFW_CURSOR);
                if (inputMode == GLFW_CURSOR_DISABLED) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    camera.isMouseCaptured = false;
                    EnableViewRotation = false;
                }
                else if (inputMode == GLFW_CURSOR_NORMAL) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    EnableViewRotation = true;
                }
                isInputModeChanged = true;
            }
        }
        if (isInputModeChanged && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            isInputModeChanged = false;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.Move(Camera::Direction::forward, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.Move(Camera::Direction::backward, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.Move(Camera::Direction::left, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.Move(Camera::Direction::right, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.Move(Camera::Direction::up, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.Move(Camera::Direction::down, deltaTime);
        }
    }

    
    virtual void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) override {
        if (EnableViewRotation) {
            camera.RotateByMouse(xpos, ypos);
        }
    }

};

}