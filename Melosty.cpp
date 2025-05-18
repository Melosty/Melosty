#include "Melosty.h"

#include "glad/glad.h"
#include "glfw/include/GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

unsigned int WIDTH{800};
unsigned int HEIGHT{600};

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
	std::cout << "Hello CMake." << std::endl;
	glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFW window.
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Melosty (Parent Window)", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // --- ImGui ---

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Style
    ImGui::StyleColorsDark();

    // Corresponds to OpenGL 3.3 Core
    const char* glsl_version = "#version 330"; 
    glsl_version = "#version 330";

    // Initialize ImGui's OpenGL backend
    ImGui_ImplGlfw_InitForOpenGL(window, true); // 第一个参数是 GLFW 窗口，第二个参数是启用键盘/鼠标回调
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Window propertity
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true;

    // Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // 处理所有待处理事件
        // Process all pending events
        glfwPollEvents();

        // --- Start the ImGui frame ---

        ImGui_ImplOpenGL3_NewFrame(); // ImGui OpenGL 后端新帧 / ImGui OpenGL backend new frame
        ImGui_ImplGlfw_NewFrame(); // ImGui GLFW 后端新帧 / ImGui GLFW backend new frame
        ImGui::NewFrame(); // ImGui 核心新帧 / ImGui core new frame

        // --- 绘制 ImGui 窗口 ---
        // --- Draw ImGui windows ---

        // 示例：显示一个 ImGui 示例窗口
        // Example: Show a simple ImGui demo window
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 示例：绘制一个自定义窗口
        // Example: Draw a custom window
        {
            static float f = 0.0f; // 静态变量，值会保留 / Static variable, value is retained
            static int counter = 0; // 静态变量，值会保留 / Static variable, value is retained

            // 开始一个新窗口
            // Begin a new window
            ImGui::Begin("Hello, world!"); // 窗口标题 / Window title

            // 窗口内的文本
            // Text inside the window
            ImGui::Text("This is some useful text."); // 文本内容 / Text content

            // 复选框，链接到 show_demo_window 变量
            // Checkbox linked to show_demo_window variable
            ImGui::Checkbox("Demo Window", &show_demo_window); // 窗口内容 / Window content

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
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // 编辑清屏颜色 / Edit clear color

            // 结束当前窗口
            // End the current window
            ImGui::End();
        }


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
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲区 / Clear the color buffer

        // 调用 ImGui 的 OpenGL 后端渲染绘制数据
        // Call ImGui's OpenGL backend to render the draw data
        ImGui_ImplOpenGL3_RenderDrawData(draw_data);

        // 交换前后缓冲区 (显示绘制好的图像)
        // Swap front and back buffers (display the rendered image)
        glfwSwapBuffers(window);
    }

    // --- Cleanup ---
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown(); // 关闭 ImGui OpenGL 后端 / Shutdown ImGui OpenGL backend
    ImGui_ImplGlfw_Shutdown(); // 关闭 ImGui GLFW 后端 / Shutdown ImGui GLFW backend
    ImGui::DestroyContext(); // 销毁 ImGui 上下文 / Destroy ImGui context

    // Cleanup GLFW
    glfwDestroyWindow(window); // 销毁窗口 / Destroy window
    glfwTerminate(); // 终止 GLFW / Terminate GLFW

	return 0;
}
