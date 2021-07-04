#pragma once
#include <Application.h>
#include <Shader.h>
#include <Matrix.h>
#include <Camera.h>
#include <Prefab.h>
#include <Object/IndexObject.h>
#include <Texture/TextureLoader.h>
#include <Object/Mesh.h>
#include <random>

namespace psi {

class TestApp : public Application {
public:
    TestApp(int w, int h, const std::string& mTitle = "window", bool fullScreen = false) : Application(w, h, mTitle, fullScreen) {}
    virtual ~TestApp() = default;
private:
    virtual void InitContext() override {
        Application::InitContext();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    virtual void InitData() override {
        meshes.push_back(New<Mesh>("Assets/Mesh/rock/rock.obj"));
        meshes.push_back(New<Mesh>("Assets/Mesh/planet/planet.obj"));
        shaders.push_back(New<Shader>("Assets/Shader/Instancing/simple_mesh.vert", "Assets/Shader/Instancing/simple_mesh.frag"));
        shaders.push_back(New<Shader>("Assets/Shader/Instancing/instance_mesh.vert", "Assets/Shader/Instancing/simple_mesh.frag"));

        std::random_device dev;
        std::default_random_engine e(dev());
        std::uniform_real_distribution<float> u(0.f, 1.f);
        std::normal_distribution<float> n(0.0, 1.0);
       

        for (int i = 0; i < amount; ++i) {
            // scale: 0.05 - 1.0
            float scaleCoef = std::max(0.5f * n(e) + 0.5f, 0.05f);
            Mat4f scale = Matrix::Scale(scaleCoef);

            // rotation: random axis and random angle
            Vec3f axis = Vec3f(u(e) + kEpsilon, u(e) + kEpsilon, u(e) + kEpsilon);
            Mat4f rotation = Matrix::Rotation(axis, u(e) * 360.0f);

            // translation
            Mat4f translation = Matrix::Translation(n(e) * 50.f + 300.f, n(e) * 20.f, n(e) * 20.f);

            // revolution: random angle at specific axis
            Vec3f fix_axis = Vec3f(0.f, 0.8f, 0.2f);
            Mat4f revolution = Matrix::Rotation(fix_axis, u(e) * 360.0f);

            models.push_back(revolution * translation * rotation * scale);
        }

        // bind models as vertex attribute 

        // vertex buffer object
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(Mat4f), models.data(), GL_STATIC_DRAW);

        size_t vec4Size = sizeof(Vec4f);

        for (auto& part : meshes[0]->parts) {
            glBindVertexArray(part->VAO);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

            glVertexAttribDivisor(3, 1);    // get next value per 1 instance
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    int amount = 300000;
    std::vector<Ptr<Mesh>> meshes;
    std::vector<Ptr<Object>> objects;
    std::vector<Ptr<Texture>> textures;
    std::vector<Ptr<Shader>> shaders;
    std::vector<Mat4f> models;

    float deltaTime = 0.f;
    Camera camera = Camera(Vec3f(-500.f, 50.f, 5.f), Vec3f(0.f, 1.f, 0.f), 90.f, 0.f);

    virtual void RunLoop() override {
        camera.cameraSpeed = 80.f;
        float fov = 70.0f;
        float lastTime = (float)glfwGetTime();

        Vec3f background = Vec3f(0.f, 0.f, 0.f);
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

            ImGui::Begin("Information");
            ImGui::Text("Fps:\t%f", 1.f / deltaTime);
            ImGui::End();

            // rendering code

            // Draw planet
            Mat4f projection = Matrix::Projection(fov, (float)width / (float)height, 0.1f, 1000.f);

            shaders[0]->Use();
            shaders[0]->SetMat4f("model", Matrix::Scale(20.f))
                .SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection);
            meshes[1]->Draw(*shaders[0]);

            // Draw rock
            shaders[1]->Use();
            shaders[1]->SetMat4f("view", camera.ViewMatrix())
                .SetMat4f("projection", projection);
            meshes[0]->textures.begin()->second->Use();
            for (auto& part : meshes[0]->parts) {
                glBindVertexArray(part->VAO);
                glDrawElementsInstanced(GL_TRIANGLES, part->vertexNum, GL_UNSIGNED_INT, 0, amount);
            }


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