#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "global.h"
#include <string>

namespace psi {

class Application {
public:
    GLFWwindow* window = nullptr;
    int width, height;
    std::string title;
    bool isFullScreen;

    Application(int w, int h, const std::string& mTitle = "window", bool fullScreen = false) 
        : width(w), height(h), title(mTitle), isFullScreen(fullScreen){ }

    virtual void Init() {
        InitContext();
        InitData();
    }

    virtual void Run() {
        RunLoop();
        CleanUp();
    }
    virtual ~Application() = default;

protected:
    // override these to impl specific application
    virtual void InitData() = 0;
    virtual void RunLoop() = 0;
    //{    // example code
    //    while (!glfwWindowShouldClose(window)) {
    //        glfwPollEvents();
    //        glClear(GL_COLOR_BUFFER_BIT);

    //        ImGui_ImplOpenGL3_NewFrame();
    //        ImGui_ImplGlfw_NewFrame();
    //        ImGui::NewFrame();

    //        // imgui code here
    //        
    //        // working code here

    //        ImGui::Render();
    //        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //        glfwSwapBuffers(window);
    //        // ProcessInput();  // add this function if needed
    //    }
    //}




    virtual void InitContext() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Set Window attribute like
        // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        // Init Window
        // �ı�������������Էֱ�����ȫ���͹�����
        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        // TODO: fullScreen
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(window); // ȷ����ǰ������
        // ���ô������Żص�����
        //glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
        // ͬ��������ø���ص�����

        // init opengl
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(-1);
        }

        // ��ʼ��gl�ӿڣ�������viewport transformation
        // ǰ����������ʾ���½����꣨��������Ĵ��ڣ�
        // ������������ʾgl�ӿڴ�С��û����Ժ�window��һ����
        glViewport(0, 0, width, height);

        //��������ImGui
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        ImGui::StyleColorsClassic();

        app = this;
        glfwSetWindowSizeCallback(window, StaticWindowSizeCallback);
        glfwSetCursorPosCallback(window, StaticCursorPosCallback);
    }

    virtual void CleanUp() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwTerminate();
    }

    static Application* app;
    static void StaticWindowSizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
        app->WindowSizeCallback(window, newWidth, newHeight);
    }
    virtual void WindowSizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
        glViewport(0, 0, newWidth, newHeight);
        width = newWidth;
        height = newHeight;
    }
    static void StaticCursorPosCallback(GLFWwindow* window, double x, double y) {
        app->CursorPosCallback(window, x, y);
    }
    virtual void CursorPosCallback(GLFWwindow* window, double x, double y) {}
};

inline Application* Application::app = nullptr;

}