#include "Melosty.h"

const std::string MELOSTY_VERSION{ "a0.0.1" };

bool windowFocus = true; // 窗口是否有焦点
bool windowExist = true; // 控制 ImGui(主窗口) 的开关状态

double TARGET_FPS; // 目标帧率，在初始化时确认该值
double FRAME_TIME; // 每帧所需时间，在初始化时确认该值

bool firstLoopState{ true };

int workAreaX, workAreaY, workAreaWidth, workAreaHeight;

unsigned int WIDTH{ 340 };
unsigned int HEIGHT{ 115 };

void whenFirstLoop(ImGuiPlatformIO& platform_io, ImGuiViewport*& viewport);

void ImGuiShow(ImGuiIO& io, GLFWwindow*& window);
void ImGuiSetStyle();

void checkWindowFocus(ImGuiPlatformIO& platform_io, ImGuiViewport*& viewport) {
    if (firstLoopState)
        return;
    if (platform_io.Platform_GetWindowFocus(viewport))
    {
        windowFocus = true;
    }
    else {
        windowFocus = false;
    }
}

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "[ERROR] GLFW Error %d: %s\n", error, description);
}

#ifdef _WIN32
void SetSystemTimerResolution(DWORD resolution_ms) {
    TIMECAPS tc;
    if (timeGetDevCaps(&tc, sizeof(tc)) == MMSYSERR_NOERROR) {
        timeBeginPeriod(std::min(static_cast<UINT>(resolution_ms), tc.wPeriodMin)); // 设置更高的计时精度时不能超过硬件支持的值
    }
}

void RestoreSystemTimerResolution() {
    timeEndPeriod(0); // 恢复到默认值。传入 0 会恢复到 timeBeginPeriod 设置的原始值。
}
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 防止控制台输出中文乱码
    SetSystemTimerResolution(1);
#endif

    std::cout << "[INFO] Melosty Initializing..." << std::endl;
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::cerr << "[FATAL ERROR] Failed to initialize GLFW" << std::endl;
        return -1;
    }
    std::cout << "[INFO] GLFW initialized." << std::endl;

    // OpenGL hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); //隐藏 GLFW 窗口

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFW window.
    GLFWwindow* window = glfwCreateWindow(1, 1, "Melosty", NULL, NULL); // 设置为 0 会导致 GLFW 窗口不能创建
    if (window == NULL) {
        std::cerr << "[FATAL ERROR] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "[INFO] Profile is: OpenGL 3.3 Core." << std::endl;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    // Vsync controler. 1 for on, 0 for off. Note: If enable, it will cause some delay when you move the ImGui window, don't use it!
    std::cout << "[INFO] GLFWInvisibleWindow created." << std::endl;
    std::cout << "[INFO] Vsync: Off." << std::endl;
    std::cout << "[INFO] Max fps: " << TARGET_FPS << std::endl;

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    if (primaryMonitor) {
        // 获取除任务栏以外的屏幕分辨率

        glfwGetMonitorWorkarea(primaryMonitor, &workAreaX, &workAreaY, &workAreaWidth, &workAreaHeight);

        std::cout << "[INFO] Primary Monitor Work Area (excluding taskbar/systemUI):" << std::endl;
        std::cout << "[INFO]   X position: " << workAreaX << std::endl;
        std::cout << "[INFO]   Y position: " << workAreaY << std::endl;
        std::cout << "[INFO]   Width: " << workAreaWidth << std::endl;
        std::cout << "[INFO]   Height: " << workAreaHeight << std::endl;
        
        if (!mode) {
            std::cerr << "[FATAL ERROR] Failed to get video mode for primary monitor" << std::endl;
            glfwTerminate();
            return -1;
        }
        std::cout << "[INFO] Primary monitor refresh rate: " << mode->refreshRate << " Hz" << std::endl;
        
        TARGET_FPS = (mode->refreshRate) * 2;
        if (TARGET_FPS > 300)
            TARGET_FPS = 300;
    }
    else {
        std::cerr << "[FATAL ERROR] Failed to get primary monitor." << std::endl;
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[FATAL ERROR] Failed to initialize GLAD" << std::endl;
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
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    ImGuiViewport* viewport = nullptr;
    std::cout << "[INFO] ImGui initialized." << std::endl;

    // Style
    ImGuiSetStyle();

    // Corresponds to OpenGL 3.3 Core
    const char* glsl_version = "#version 330";
    glsl_version = "#version 330";

    // Initialize ImGui's OpenGL backend
    ImGui_ImplGlfw_InitForOpenGL(window, true); // 第一个参数是 GLFW 窗口，第二个参数是启用键盘/鼠标回调
    ImGui_ImplOpenGL3_Init(glsl_version);

    // --- Main Loop ---
    double FRAME_TIME = 1.0 / TARGET_FPS;
    std::cout << "[INFO] Melosty " << MELOSTY_VERSION << " startup!" << std::endl;
    while (!glfwWindowShouldClose(window) && windowExist)
    {
        auto currentFrameStartTime = std::chrono::high_resolution_clock::now();

        // 处理所有待处理事件
        // Process all pending events
        glfwPollEvents();

        checkWindowFocus(platform_io, viewport);
        if (!windowFocus) {
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // pause for a while to save resource
        }

        // --- Start the ImGui frame ---

        ImGui_ImplOpenGL3_NewFrame(); // ImGui OpenGL 后端新帧 / ImGui OpenGL backend new frame
        ImGui_ImplGlfw_NewFrame(); // ImGui GLFW 后端新帧 / ImGui GLFW backend new frame
        ImGui::NewFrame(); // ImGui 核心新帧 / ImGui core new frame
        ImGui::SetNextWindowPos(ImVec2(workAreaWidth / 2.0f - WIDTH / 2.0f + workAreaX, workAreaHeight / 2.0f - HEIGHT / 2.0f + workAreaY), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT), ImGuiCond_Appearing); // 启动时设置窗口大小并将窗口居中

        // --- 绘制 ImGui 窗口 ---
        // --- Draw ImGui windows ---

        // 创建主窗口
        ImGui::Begin("Melosty", &windowExist,
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

        // 调用 ImGui 的 OpenGL 后端渲染绘制数据
        // Call ImGui's OpenGL backend to render the draw data
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);

        ImGui::UpdatePlatformWindows();      // 更新 ImGui 创建的原生 OS 窗口的状态
        ImGui::RenderPlatformWindowsDefault(); // 渲染这些原生 OS 窗口   

        whenFirstLoop(platform_io, viewport);

        // --- Manual framerate limiting ---
        auto currentFrameEndTime = std::chrono::high_resolution_clock::now();
        auto targetFrameEndTime = currentFrameStartTime + std::chrono::duration<double>(FRAME_TIME);
        std::chrono::duration<double> remaining_time = targetFrameEndTime - currentFrameEndTime;

        if (remaining_time.count() > 0) { // 只有在有剩余时间时才进行休眠/让出
            // 如果剩余时间明显较长，使用 `sleep_until`
            // `sleep_until` 通常比 `sleep_for` 更适合用于精确达到目标时间点。
            double SLEEP_THRESHOLD_MS = 1.0;
            if (remaining_time.count() * 1000.0 > SLEEP_THRESHOLD_MS) {
                // 休眠直到目标结束时间前一点点
                std::this_thread::sleep_until(targetFrameEndTime - std::chrono::milliseconds((long long)(SLEEP_THRESHOLD_MS)));
            }

            // 对于剩余的短时间，使用带 `yield` 的忙等待来确保精度
            while (std::chrono::high_resolution_clock::now() < targetFrameEndTime) {
                std::this_thread::yield(); // 让出 CPU 时间片给其他线程
            }
        }
        
    }

    // --- Cleanup ---
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown(); // 关闭 ImGui OpenGL 后端 / Shutdown ImGui OpenGL backend
    ImGui_ImplGlfw_Shutdown(); // 关闭 ImGui GLFW 后端 / Shutdown ImGui GLFW backend
    ImGui::DestroyContext(); // 销毁 ImGui 上下文 / Destroy ImGui context
    std::cout << "[INFO] ImGui exited." << std::endl;

    // Cleanup GLFW
    glfwDestroyWindow(window); // 销毁窗口 / Destroy window
    glfwTerminate(); // 终止 GLFW / Terminate GLFW
    std::cout << "[INFO] GLFW exited." << std::endl;

    std::cout << "[INFO] Melosty exited." << std::endl;

#ifdef _WIN32
    RestoreSystemTimerResolution();
#endif

	return 0;
}

void whenFirstLoop(ImGuiPlatformIO& platform_io, ImGuiViewport*& viewport) {
    // 遍历 ImGui 的所有平台视口
    if (!firstLoopState)
        return;

    firstLoopState = false;

    for (int n = 0; n < platform_io.Viewports.Size; n++)
    {
        viewport = platform_io.Viewports[n];
        // 确保 Platform_GetWindowFocus 已经被设置（不是 NULL）
        // 并且该视口已经创建了实际的平台窗口
        // 并且该视口不是 GLFW 视口
        if (platform_io.Platform_GetWindowFocus && viewport->PlatformWindowCreated && !(viewport->Flags & ImGuiViewportFlags_OwnedByApp))
        {
            platform_io.Platform_SetWindowFocus(viewport);
            break;
        }
    }
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

void Doc_ThisIsAnimationEngineExample() {
    // This Function will never called.
    // This Function is a temporary documentation for using animationEngine
    // It will be move to another place later, but for now it will be save here.
    // Notice: All component from animationEngine are save in this namespace: "animationEngine"
}