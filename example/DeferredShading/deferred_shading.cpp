#include <Application/CameraApp.h>
#include <Object/Mesh.h>
#include <FrameBuffer.h>

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
    Ptr<FrameBuffer> gBuffer;

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


        gBuffer = New<FrameBuffer>();

        textures["floor"] = TextureLoader::LoadTexture("assets/Texture/marble.jpg", false);
        textures["box"] = TextureLoader::LoadTexture("assets/Texture/container.png", false);
        textures["box_specular"] = TextureLoader::LoadTexture("assets/Texture/container_specular.png", false);

        textures["gPosition"] = TextureLoader::FrameBufferTarget(width, height, GL_RGBA16F, GL_FLOAT);
        textures["gNormal"] = TextureLoader::FrameBufferTarget(width, height, GL_RGBA16F, GL_FLOAT);
        textures["gColorSpec"] = TextureLoader::FrameBufferTarget(width, height);

        objects["light"] = New<Object>(light_cube, Index{ 3 });
        objects["box"] = New<Object>(box_cube, Index{ 3, 3, 2 });
        objects["floor"] = New<IndexObject>(xz_rect, Index{ 3, 3, 2 }, rect_index);
        objects["screen"] = New<IndexObject>(screen, Index{ 3, 2 }, rect_index);

        shaders["light"] = New<Shader>("Assets/Shader/light.vert", "Assets/Shader/light.frag");
        shaders["deferred_shading"] = New<Shader>("Assets/Shader/DeferredShading/DeferredShading.vert", "Assets/Shader/DeferredShading/DeferredShading.frag");
        shaders["default"] = New<Shader>("Assets/Shader/FrameBuffer/direct.vert", "Assets/Shader/FrameBuffer/direct.frag");   // for test
        shaders["lighting"] = New<Shader>("Assets/Shader/DeferredShading/Lighting.vert", "Assets/Shader/DeferredShading/Lighting.frag");

        gBuffer->Use()
            .AttachColorBuffer(textures["gPosition"]->texture)
            .AttachColorBuffer(textures["gNormal"]->texture)
            .AttachColorBuffer(textures["gColorSpec"]->texture)
            .EndAttach();

        unsigned int depth;
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        FrameBuffer::UseDefault();

        // ======= End init data ==========

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }


    virtual void RunLoop() override {

        // ========= Init params ===========

        Vec3f background(0.f, 0.f, 0.f);

        // light properties
        Vec3f light_pos(2.f, 4.f, -0.5f);
        Vec3f light_color_base(1.f, 1.f, 1.f);
        float light_intensity = 64.f;

        float shadow_bias = 0.0f;

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
            ImGui::DragFloat("light_intensity", &light_intensity, 1);
            ImGui::End();

            ImGui::Begin("Information");
            ImGui::Text("Fps:\t%f", 1.f / deltaTime);
            ImGui::End();

            // ======= End imgui settings ======

            // ========= Rendering code ========

            gBuffer->Use();

            glClearColor(background.x(), background.y(), background.z(), 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_BACK);
            // transforms
            Mat4f view = camera.ViewMatrix();
            Mat4f projection = Matrix::Projection(70.f, (float)width / (float)height, 0.1f, 1000.f);
            Mat4f model = Mat4f::Identity();
            shaders["deferred_shading"]->Use().SetMVP(model, view, projection).SetMat4f("normalTransform", model)
                .SetInt("diffuseMap", 0).SetInt("specularMap", 1);
            textures["box"]->Use(0);
            textures["box_specular"]->Use(1);

            // draw boxes
            for (int i = -2; i < 3; ++i) {
                for (int j = -2; j < 3; ++j) {
                    model = Matrix::Translation(2.f * i, 0.f, 2.f * j);
                    shaders["deferred_shading"]->SetMat4f("model", model).SetMat4f("normalTransform", model);
                    objects["box"]->Draw();
                }
            }


            FrameBuffer::UseDefault();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            //shaders["default"]->Use().SetInt("tex", 0);
            //textures["gColorSpec"]->Use(0);
            //objects["screen"]->Draw();

            shaders["lighting"]->Use()
                .SetInt("gPosition", 0).SetInt("gNormal", 1).SetInt("gColorSpec", 2)
                .SetVec3f("viewPos", camera.pos)
                .SetVec3f("lightPos", light_pos).SetVec3f("lightColor", light_color_base).SetFloat("lightIntensity", light_intensity)
                .SetVec3f("ka", Vec3f(0.05f, 0.05f, 0.05f)).SetFloat("shininess", 128);
            textures["gPosition"]->Use(0);
            textures["gNormal"]->Use(1);
            textures["gColorSpec"]->Use(2);
            objects["screen"]->Draw();

            glEnable(GL_DEPTH_TEST);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            model = Matrix::Translation(light_pos) * Matrix::Scale(0.1f);
            shaders["light"]->Use().SetMVP(model, view, projection)
                .SetVec3f("color", light_color_base);
            objects["light"]->Draw();

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