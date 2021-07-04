#pragma once
#include <Application.h>
#include <Shader.h>
#include <Matrix.h>
#include <Camera.h>
#include <Prefab.h>
#include <Object/IndexObject.h>
#include <Texture/TextureLoader.h>
#include <Object/Mesh.h>

namespace psi {

class TestApp : public Application {
public:
    TestApp(int w, int h, const std::string& mTitle = "window", bool fullScreen = false) : Application(w, h, mTitle, fullScreen) {}
    virtual ~TestApp() = default;
private:
    virtual void InitContext() override {
        Application::InitContext();
        glEnable(GL_DEPTH_TEST);

    }
    virtual void InitData() override {

        float xz_rect[] = {
             0.5f,  0.0f, 0.5f,  1.f, 1.f,
             0.5f,  0.0f, -0.5f, 1.f, 0.f,
            -0.5f,  0.0f, -0.5f, 0.f, 0.f,
            -0.5f,  0.0f, 0.5f,  0.f, 1.f
        };
        unsigned int rect_ind[] = {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };
        std::vector<std::string> cubemap_path = {
            "assets/Texture/learnopengl_skybox/right.jpg",
            "assets/Texture/learnopengl_skybox/left.jpg",
            "assets/Texture/learnopengl_skybox/top.jpg",
            "assets/Texture/learnopengl_skybox/bottom.jpg",
            "assets/Texture/learnopengl_skybox/front.jpg",
            "assets/Texture/learnopengl_skybox/back.jpg"
        };


        meshes.push_back(New<Mesh>("Assets/Mesh/mary/marry.obj"));
        shaders.push_back(New<Shader>("Assets/Shader/MeshBlinnPhong.vert", "Assets/Shader/MeshBlinnPhong.frag"));

        objects.push_back(New<Object>(Prefab::cube, sizeof(Prefab::cube), std::initializer_list<int>{3}));
        shaders.push_back(New<Shader>("Assets/Shader/light.vert", "Assets/Shader/light.frag"));

        objects.push_back(New<IndexObject>(xz_rect, sizeof(xz_rect), std::initializer_list<int>{3, 2}, rect_ind, sizeof(rect_ind)));
        textures.push_back(TextureLoader::LoadTexture("Assets/Texture/marble.jpg"));
        shaders.push_back(New<Shader>("Assets/Shader/default.vert", "Assets/Shader/default.frag"));

        textures.push_back(TextureLoader::LoadCubemap(cubemap_path));
        shaders.push_back(New<Shader>("Assets/Shader/Cubemap/skybox.vert", "Assets/Shader/Cubemap/skybox.frag"));

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    //unsigned VAO = 0;
    std::vector<Ptr<Mesh>> meshes;
    std::vector<Ptr<Object>> objects;
    std::vector<Ptr<Texture>> textures;
    std::vector<Ptr<Shader>> shaders;

    float deltaTime = 0.f;
    Camera camera = Camera(Vec3f(0.f, 2.f, 5.f), Vec3f(0.f, 1.f, 0.f), 180.f, 0.f);

    virtual void RunLoop() override {
        float fov = 70.0f;
        float lastTime = (float)glfwGetTime();

        Vec3f lightPos(0.f, 5.f, 0.f);
        Vec3f lightColorBase(1.f, 1.f, 1.f);
        float lightColorCoef = 2.5f;

        Vec3f background = Vec3f(0.f, 0.f, 0.f);
        //Vec3f foreground = Vec3f(0.3f, 0.4f, 0.8f);
        while (!glfwWindowShouldClose(window)) {
            float curTime = (float)glfwGetTime();
            deltaTime = curTime - lastTime;
            lastTime = curTime;
            glfwPollEvents();

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

            ImGui::Begin("light Properties");
            ImGui::DragFloat3("lightPos", lightPos.data(), 0.1f, -10.f, 10.f);
            ImGui::ColorEdit3("lightColorBase", lightColorBase.data());
            ImGui::DragFloat("lightColorCoef", &lightColorCoef, 0.1f, 0.f, 100.f);
            ImGui::End();


            glClearColor(background.x(), background.y(), background.z(), 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // rendering code
            Mat4f model = Matrix::Translation(Vec3f(0.f, 0.f, 0.f));
            Mat4f projection = Matrix::Projection(fov, (float)width / (float)height, 0.1f, 100.f);
            Mat4f normalTransform = model.inverse().transpose();

            // Draw skybox
            glDepthMask(GL_FALSE);
            shaders[3]->Use();
            shaders[3]->SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection);
            textures[1]->Use();
            objects[0]->Draw();
            glDepthMask(GL_TRUE);

            // Draw mesh
            shaders[0]->Use();
            shaders[0]->SetMat4f("model", model)
                .SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection)
                .SetMat4f("normalTransform", normalTransform)
                .SetVec3f("lightPos", lightPos)
                .SetVec3f("lightColor", lightColorBase * lightColorCoef)
                .SetVec3f("viewPos", camera.pos)
                .SetFloat("uKa", 0.05f);
            meshes[0]->Draw(*shaders[0]);

            // Draw floor
            model = Matrix::Scale(16.f);
            shaders[2]->Use();
            shaders[2]->SetMat4f("model", model)
                .SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection);
            textures[0]->Use(0);
            objects[1]->Draw();

            // Draw Light
            model = Matrix::Translation(lightPos) * Matrix::Scale(0.1f);
            shaders[1]->Use();
            shaders[1]->SetMat4f("model", model)
                .SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection)
                .SetVec3f("color", lightColorBase);
            objects[0]->Draw();



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

int main() {
    psi::TestApp app(1280, 720);
    app.Init();
    app.Run();
}