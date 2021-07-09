#include <Application/CameraApp.h>
#include <Object/Mesh.h>
#include <array>

namespace psi {

class TestApp : public CameraApp {
public:
    TestApp(int w, int h, const std::string& mTitle = "window", bool fullScreen = false) : CameraApp(w, h, mTitle, fullScreen) {
        camera = Camera(Vec3f(0.f, 2.f, 5.f), Vec3f(0.f, 1.f, 0.f), 180.f, 0.f);
    }
    virtual ~TestApp() = default;
private:
    Hashmap<std::string, Ptr<Mesh>> meshes;
    Hashmap<std::string, Ptr<Object>> objects;
    Hashmap<std::string, Ptr<Texture>> textures;
    Hashmap<std::string, Ptr<Shader>> shaders;

    virtual void InitContext() override {
        CameraApp::InitContext();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    virtual void InitData() override {

        // ========= Init data ============

        using Index = std::initializer_list<int>;

        std::vector<float> light_cube = {
            #include <Prefab/cube.data>
        };
        std::vector<float> box_cube = {
            #include <Prefab/cube_normal_uv.data>
        };
        std::vector<float> xz_rect = {
            #include <Prefab/xz_rect_normal_uv.data>
        };
        std::vector<float> screen = {
            #include <Prefab/screen.data>
        };
        std::vector<unsigned> rect_index = {
            #include <Prefab/rect_index.data>
        };

        // init frame buffer
        glGenFramebuffers(1, &depth_map_fbo);

        // depth cubemap
        unsigned depth_texture;
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_texture);
        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, depth_map_width, depth_map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


        textures["depth_map"] = New<Cubemap>(depth_texture);
        textures["floor"] = TextureLoader::LoadTexture("assets/Texture/marble.jpg", false);
        textures["box"] = TextureLoader::LoadTexture("assets/Texture/container.png", false);
        textures["box_specular"] = TextureLoader::LoadTexture("assets/Texture/container_specular.png", false);

        objects["light"] = New<Object>(light_cube, Index{ 3 });
        objects["box"] = New<Object>(box_cube, Index{ 3, 3, 2 });
        objects["floor"] = New<IndexObject>(xz_rect, Index{ 3, 3, 2 }, rect_index);
        objects["screen"] = New<IndexObject>(screen, Index{ 3, 2 }, rect_index);

        shaders["light"] = New<Shader>("Assets/Shader/light.vert", "Assets/Shader/light.frag");
        shaders["shadow"] = New<Shader>("Assets/Shader/ShadowMap/shadow_cube.vert", "Assets/Shader/ShadowMap/shadow_cube.frag");
        shaders["depth_map"] = New<Shader>("Assets/Shader/ShadowMap/depth_cubemap.vert", "Assets/Shader/ShadowMap/depth_cubemap.geom", "Assets/Shader/ShadowMap/depth_cubemap.frag");
        shaders["show_texture"] = New<Shader>("Assets/Shader/ShadowMap/show_texture.vert", "Assets/Shader/ShadowMap/show_texture_depth.frag");

        // ======= End init data ==========

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    unsigned depth_map_fbo = 0;
    int depth_map_width = 1024;
    int depth_map_height = 1024;

    virtual void RunLoop() override {

        // ========= Init params ===========

        Vec3f background(0.f, 0.f, 0.f);

        // light properties
        Vec3f light_pos(2.f, 4.f, -0.5f);
        Vec3f light_color_base(1.f, 1.f, 1.f);
        float light_intensity = 8.f;

        float shadow_bias = 0.0f;

        float lightFarPlane = 20.0f;
        // ======== End init params ========

        float lastTime = (float)glfwGetTime();
        while (!glfwWindowShouldClose(window)) {
            float curTime = (float)glfwGetTime();
            deltaTime = curTime - lastTime;
            lastTime = curTime;
            glfwPollEvents();
            // ======== Imgui settings ========

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            ImGui::ColorEdit3("background", background.data());
            ImGui::DragFloat3("light_pos", light_pos.data(), 0.1);
            ImGui::ColorEdit3("light_color_base", light_color_base.data());
            ImGui::DragFloat("light_intensity", &light_intensity, 0.1);
            ImGui::DragFloat("shadow_bias", &shadow_bias, 0.00001f, 0.0f, 0.05f);
            ImGui::End();

            ImGui::Begin("Information");
            ImGui::Text("Fps:\t%f", 1.f / deltaTime);
            ImGui::End();

            // ======= End imgui settings ======

            // ========= Rendering code ========

            // draw depth map
            {
                glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
                glViewport(0, 0, depth_map_width, depth_map_height);
                glClear(GL_DEPTH_BUFFER_BIT);
                glCullFace(GL_FRONT);

                Mat4f projection = Matrix::Projection(90.f, (float)depth_map_width / depth_map_height, 0.2f, lightFarPlane);

                std::array<Mat4f, 6> views = {
                    Matrix::LookAt(light_pos, light_pos + Vec3f(1, 0, 0), Vec3f(0, -1, 0)),
                    Matrix::LookAt(light_pos, light_pos + Vec3f(-1, 0, 0), Vec3f(0, -1, 0)),
                    Matrix::LookAt(light_pos, light_pos + Vec3f(0, 1, 0), Vec3f(0, 0, 1)),
                    Matrix::LookAt(light_pos, light_pos + Vec3f(0, -1, 0), Vec3f(0, 0, -1)),
                    Matrix::LookAt(light_pos, light_pos + Vec3f(0, 0, 1), Vec3f(0, -1, 0)),
                    Matrix::LookAt(light_pos, light_pos + Vec3f(0, 0, -1), Vec3f(0, -1, 0))
                };

                for (int i = 0; i < 6; ++i) {
                    shaders["depth_map"]->Use()
                        .SetMat4f("views[" + std::to_string(i) + "]", views[i])
                        .SetVec3f("lightPos", light_pos)
                        .SetFloat("farPlane", lightFarPlane);
                }
                // draw floor
                Mat4f model = Matrix::Scale(16.f);
                shaders["depth_map"]->SetMVP(model, Mat4f::Identity(), projection);
                objects["floor"]->Draw();
                // draw box
                model = Matrix::Scale(2.0f) * Matrix::Translation(0.f, 0.501f, 0.f);
                shaders["depth_map"]->SetMat4f("model", model);
                objects["box"]->Draw();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(0, 0, width, height);

                shaders["shadow"]->Use()
                    .SetInt("map_shadow", 3)
                    .SetFloat("lightFarPlane", lightFarPlane);
                textures["depth_map"]->Use(3);
            }

            glClearColor(background.x(), background.y(), background.z(), 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_BACK);
            // transforms
            Mat4f view = camera.ViewMatrix();
            Mat4f projection = Matrix::Projection(70.f, (float)width / (float)height, 0.1f, 1000.f);

            // draw light
            Mat4f model = Matrix::Translation(light_pos) * Matrix::Scale(0.1f);
            shaders["light"]->Use().SetMVP(model, view, projection)
                .SetVec3f("color", light_color_base);
            objects["light"]->Draw();

            // draw floor
            model = Matrix::Scale(16.f);
            shaders["shadow"]->Use().SetMVP(model, view, projection).SetMat4f("normalTransform", model)
                .SetVec3f("lightPos", light_pos).SetVec3f("lightColor", light_color_base * light_intensity)
                .SetVec3f("viewPos", camera.pos)
                .SetVec3f("Ka", Vec3f(0.03f, 0.03f, 0.03f)).SetVec3f("Ks", Vec3f(0.8f, 0.8f, 0.8f)).SetFloat("Ns", 32)
                .SetInt("UseKdMap", true).SetInt("map_Kd", 0)
                .SetInt("UseKsMap", false);
            textures["floor"]->Use(0);
            objects["floor"]->Draw();

            // draw box
            model = Matrix::Scale(2.0f) * Matrix::Translation(0.f, 0.501f, 0.f);
            shaders["shadow"]->SetMat4f("model", model)
                .SetInt("UseKsMap", true).SetInt("map_Ks", 1);
            textures["box"]->Use(0);
            textures["box_specular"]->Use(1);
            objects["box"]->Draw();

            // ====== End rendering code =======

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            ProcessInput();
        }
    }
};

}

int main() {
    psi::TestApp app(1280, 720);
    app.Init();
    app.Run();
}