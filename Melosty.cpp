#include "Melosty.h"

#ifdef _WIN32
#include <Windows.h>
#endif
const std::string MELOSTY_VERSION{"a0.0.1"};

bool windowFocus = true; // 窗口是否有焦点
bool showWindow = true; // 控制 ImGui(主窗口) 的显示状态

const double TARGET_FPS = 150.0; // 目标帧率
const double FRAME_TIME = 1.0 / TARGET_FPS; // 每帧所需时间

unsigned int WIDTH{1};
unsigned int HEIGHT{1};

void ImGuiShow(ImGuiIO& io, GLFWwindow*& window);
void ImGuiSetStyle();

void window_focus_callback(GLFWwindow* window, int focused) {
    if (focused) {
        windowFocus = true;
    }
    else {
        windowFocus = false;
    }
}

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

	std::cout << "Melosty Initializing..." << std::endl;
	glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    std::cout << "GLFW initialized." << std::endl;

    // OpenGL hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); //隐藏 GLFW 窗口

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFW window.
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Melosty", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "Profile is: OpenGL 3.3 Core." << std::endl;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    // Vsync controler. 1 for on, 0 for off. Note: If enable, it will cause some delay when you move the ImGui window, don't use it!
    glfwSetWindowFocusCallback(window, window_focus_callback); // 注册窗口焦点回调函数
    std::cout << "Window created." << std::endl;
    std::cout << "Vsync: Off." << std::endl;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // --- ImGui ---

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoAutoMerge = true;
    io.IniFilename = NULL; // 阻止 ImGui 创建或加载 imgui.ini 文件
    std::cout << "ImGui initialized." << std::endl;

    // Style
    ImGuiSetStyle();

    // Corresponds to OpenGL 3.3 Core
    const char* glsl_version = "#version 330"; 
    glsl_version = "#version 330";

    // Initialize ImGui's OpenGL backend
    ImGui_ImplGlfw_InitForOpenGL(window, true); // 第一个参数是 GLFW 窗口，第二个参数是启用键盘/鼠标回调
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Main Loop
    std::cout << "Melosty " << MELOSTY_VERSION << " startup!" << std::endl;
    while (!glfwWindowShouldClose(window) && showWindow)
    {
        auto currentFrameStartTime = std::chrono::high_resolution_clock::now();

        // 处理所有待处理事件
        // Process all pending events
        glfwPollEvents();

        if (!windowFocus) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // pause for a while to save resource
            continue;
        }

        // --- Start the ImGui frame ---

        ImGui_ImplOpenGL3_NewFrame(); // ImGui OpenGL 后端新帧 / ImGui OpenGL backend new frame
        ImGui_ImplGlfw_NewFrame(); // ImGui GLFW 后端新帧 / ImGui GLFW backend new frame
        ImGui::NewFrame(); // ImGui 核心新帧 / ImGui core new frame

        // --- 绘制 ImGui 窗口 ---
        // --- Draw ImGui windows ---

        // 创建主窗口
        ImGui::Begin("Melosty", &showWindow,
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoCollapse); // 窗口属性

        ImGuiShow(io, window);

        // 结束当前窗口
        // End the current window
        ImGui::End();

        // --- ImGui Rendering ---

        ImGui::Render(); // 收集绘制命令 / Gather draw commands
        // 获取绘制数据
        // Get draw data
        ImDrawData* draw_data = ImGui::GetDrawData();

        // 获取窗口大小（可能有视网膜屏等因素影响像素密度）
        // Get framebuffer size (might be affected by retina display etc.)
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h); // 设置视口 / Set viewport

        // 清除颜色缓冲区
        // Clear color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲区 / Clear the color buffer

        // 调用 ImGui 的 OpenGL 后端渲染绘制数据
        // Call ImGui's OpenGL backend to render the draw data
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);

        // 将焦点转移给 ImGui
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext(); // 备份当前主窗口的 OpenGL 上下文
            ImGui::UpdatePlatformWindows();      // 更新 ImGui 创建的原生 OS 窗口的状态
            ImGui::RenderPlatformWindowsDefault(); // 渲染这些原生 OS 窗口
            glfwMakeContextCurrent(backup_current_context); // 恢复主窗口的 OpenGL 上下文
        }

        // 交换前后缓冲区 (显示绘制好的图像)
        // Swap front and back buffers (display the rendered image)
        glfwSwapBuffers(window);

        // Manual framerate limiting
        auto currentFrameEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameDuration = currentFrameEndTime - currentFrameStartTime;
        auto targetFrameEndTime = currentFrameStartTime + std::chrono::duration<double>(FRAME_TIME);
        while (std::chrono::high_resolution_clock::now() < targetFrameEndTime) {
            // 忙等待，不做任何事情，持续消耗CPU直到时间点
        }
    }

    // --- Cleanup ---
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown(); // 关闭 ImGui OpenGL 后端 / Shutdown ImGui OpenGL backend
    ImGui_ImplGlfw_Shutdown(); // 关闭 ImGui GLFW 后端 / Shutdown ImGui GLFW backend
    ImGui::DestroyContext(); // 销毁 ImGui 上下文 / Destroy ImGui context

    // Cleanup GLFW
    glfwDestroyWindow(window); // 销毁窗口 / Destroy window
    glfwTerminate(); // 终止 GLFW / Terminate GLFW

    std::cout << "Melosty exited." << std::endl;
	return 0;
}

void ImGuiSetStyle() {
    ImGui::StyleColorsDark();
}

void ImGuiShow(ImGuiIO& io, GLFWwindow*& window) {
    static float f = 0.0f; // 静态变量，值会保留 / Static variable, value is retained
    static int counter = 0; // 静态变量，值会保留 / Static variable, value is retained

    // 窗口内的文本
    // Text inside the window
    ImGui::Text("This is some useful text."); // 文本内容 / Text content

    // 滑动条
    // Slider
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // 滑动条 / Slider

    // 按钮
    // Button
    if (ImGui::Button("Button")) // 按钮文本 / Button text
        counter++; // 增加计数器 / Increment counter

    // 在同一行显示文本
    // Display text on the same line
    ImGui::SameLine(); // 在同一行继续 / Continue on the same line
    ImGui::Text("counter = %d", counter); // 计数器文本 / Counter text

    // 设置清屏颜色（ImGui 窗口中的控制）
    // Set clear color (control from ImGui window)
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate); // 帧率信息 / Framerate info
}
