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
        std::vector<float> wall_rect = {
            #include <Prefab/xz_rect_normal_uv.data>
        };
        std::vector<unsigned> wall_index = {
            #include <Prefab/rect_index.data>
        };

        textures["wall"] = TextureLoader::LoadTexture("assets/Texture/bricks2.jpg", false);
        textures["wall_normal"] = TextureLoader::LoadTexture("assets/Texture/bricks2_normal.jpg", false);
        textures["wall_depth"] = TextureLoader::LoadTexture("assets/Texture/bricks2_disp.jpg", false);

        objects["light"] = New<Object>(light_cube, Index{ 3 });
        objects["cube"] = New<Object>(box_cube, Index{ 3, 3, 2 }, true);
        objects["wall"] = New<IndexObject>(wall_rect, Index{ 3, 3, 2 }, wall_index, true);

        shaders["light"] = New<Shader>("Assets/Shader/light.vert", "Assets/Shader/light.frag");
        shaders["parallaxMap"] = New<Shader>("Assets/Shader/Mapping/ParallaxMap.vert", "Assets/Shader/Mapping/ParallaxMap.frag");
        shaders["steepParallaxMap"] = New<Shader>("Assets/Shader/Mapping/ParallaxMap.vert", "Assets/Shader/Mapping/SteepParallaxMap.frag");
        shaders["parallaxOcclusionMap"] = New<Shader>("Assets/Shader/Mapping/ParallaxMap.vert", "Assets/Shader/Mapping/ParallaxOcclusionMap.frag");


        // ======= End init data ==========

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    virtual void RunLoop() override {

        // ========= Init params ===========

        Vec3f background(0.f, 0.f, 0.f);

        // light properties
        Vec3f light_pos(2.f, 4.f, -0.5f);
        Vec3f light_color_base(1.f, 1.f, 1.f);
        float light_intensity = 8.f;

        float depthScale = 0.05f;
        int numLayers = 10;

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
            ImGui::DragFloat("depth_scale", &depthScale, 0.001);
            ImGui::DragInt("num_layers", &numLayers, 1.0, 1);
            ImGui::End();

            ImGui::Begin("Information");
            ImGui::Text("Fps:\t%f", 1.f / deltaTime);
            ImGui::End();

            // ======= End imgui settings ======

            // ========= Rendering code ========


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

            // case1: Ô­Ê¼µÄÊÓ²îÌùÍ¼

            // draw box
            model = Matrix::Translation(0.f, 0.5f, 0.f);
            shaders["parallaxMap"]->Use().SetMVP(model, view, projection).SetMat4f("normalTransform", model)
                .SetVec3f("ka", Vec3f(0.1f, 0.1f, 0.1f)).SetVec3f("ks", Vec3f(0.1f, 0.1f, 0.1f)).SetFloat("shininess", 32)
                .SetInt("diffuseMap", 0).SetInt("normalMap", 1).SetInt("depthMap", 2)
                .SetVec3f("lightPos", light_pos).SetVec3f("lightColor", light_color_base * light_intensity)
                .SetVec3f("viewPos", camera.pos)
                .SetFloat("depthScale", depthScale);
            textures["wall"]->Use(0);
            textures["wall_normal"]->Use(1);
            textures["wall_depth"]->Use(2);
            objects["cube"]->Draw();

            // draw wall
            model = Matrix::Translation(1.f, 0.f, 0.f);
            shaders["parallaxMap"]->SetMat4f("model", model).SetMat4f("normalTransform", model);
            objects["wall"]->Draw();

            // case2: ¶¸ÇÍÊÓ²îÓ³Éä

            // draw box
            model = Matrix::Translation(0.f, 0.5f, 2.f);
            shaders["steepParallaxMap"]->Use().SetMVP(model, view, projection).SetMat4f("normalTransform", model)
                .SetVec3f("ka", Vec3f(0.1f, 0.1f, 0.1f)).SetVec3f("ks", Vec3f(0.1f, 0.1f, 0.1f)).SetFloat("shininess", 32)
                .SetInt("diffuseMap", 0).SetInt("normalMap", 1).SetInt("depthMap", 2)
                .SetVec3f("lightPos", light_pos).SetVec3f("lightColor", light_color_base * light_intensity)
                .SetVec3f("viewPos", camera.pos)
                .SetFloat("depthScale", depthScale).SetInt("numLayers", numLayers);
            textures["wall"]->Use(0);
            textures["wall_normal"]->Use(1);
            textures["wall_depth"]->Use(2);
            objects["cube"]->Draw();

            // draw wall
            model = Matrix::Translation(1.f, 0.f, 2.f);
            shaders["steepParallaxMap"]->SetMat4f("model", model).SetMat4f("normalTransform", model);
            objects["wall"]->Draw();

            // case3: ÊÓ²îÕÚµ²

            // draw box
            model = Matrix::Translation(0.f, 0.5f, 4.f);
            shaders["parallaxOcclusionMap"]->Use().SetMVP(model, view, projection).SetMat4f("normalTransform", model)
                .SetVec3f("ka", Vec3f(0.1f, 0.1f, 0.1f)).SetVec3f("ks", Vec3f(0.1f, 0.1f, 0.1f)).SetFloat("shininess", 32)
                .SetInt("diffuseMap", 0).SetInt("normalMap", 1).SetInt("depthMap", 2)
                .SetVec3f("lightPos", light_pos).SetVec3f("lightColor", light_color_base * light_intensity)
                .SetVec3f("viewPos", camera.pos)
                .SetFloat("depthScale", depthScale).SetInt("numLayers", numLayers);
            textures["wall"]->Use(0);
            textures["wall_normal"]->Use(1);
            textures["wall_depth"]->Use(2);
            objects["cube"]->Draw();

            // draw wall
            model = Matrix::Translation(1.f, 0.f, 4.f);
            shaders["parallaxOcclusionMap"]->SetMat4f("model", model).SetMat4f("normalTransform", model);
            objects["wall"]->Draw();

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