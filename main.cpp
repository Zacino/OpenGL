#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src/Render.h"
#include "src/VertexBuffer.h"
#include "src/IndexBuffer.h"
#include "src/VertexArray.h"
#include "src/VertexBufferLayout.h"
#include "src/Shader.h"
#include "src/Texture.h"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"


int main() {
    GLFWwindow* window;
    
    if (!glfwInit()) 
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 显式请求 OpenGL 核心模式，以确保着色器能够正常工作
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // 创建一个窗口和它的 OpenGL 上下文
    window = glfwCreateWindow(960, 540, "Hello World", nullptr, nullptr);
    if (!window)
    {           
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 将窗口的上下文设置为当前线程的上下文
    glfwMakeContextCurrent(window);

    // 启用垂直同步
    glfwSwapInterval(1);

    // 初始化 GLEW，加载所有 OpenGL 函数指针
    // std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error: Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // 顶点数据，包含三个顶点的x和y坐标
    // float positions[] = {
    //     -0.5f, -0.5f,  0.0f, 0.0f, // 0
    //      0.5f,  -0.5f, 1.0f, 0.0f,// 1
    //      0.5f, 0.5f,  1.0f, 1.0f,// 2
    //      -0.5f, 0.5f,  0.0f, 1.0f,// 3
    // };
    float positions[] = {
        100.0f, 100.0f,  0.0f, 0.0f, // 0
         200.0f,  100.0f, 1.0f, 0.0f,// 1
         200.0f, 200.0f,  1.0f, 1.0f,// 2
         100.0f, 200.0f,  0.0f, 1.0f,// 3
    };

    /*
        把OpenGL想象成一个工作台，上面有很多专用的“工位”。
        例如，有一个“专门处理顶点数据”的工位，一个“专门处理顶点绘制顺序”的工位等等。 
        调用draw指令的时候就会从设置的工位中画。
        glBindBuffer(GLenum target, GLuint buffer)
        函数的作用就是：把一个指定的“零件盒”（缓冲对象）放到一个指定的“工位”上。
        GLenum target:指定缓冲对象要绑定到的“工位”，即绑定点。这个参数告诉OpenGL你打算如何使用这个缓冲对象。
            GL_ARRAY_BUFFER: 这是最常用的目标。它用于指定包含顶点属性数据的缓冲。
            GL_ELEMENT_ARRAY_BUFFER: 它用于指定包含顶点索引的缓冲。
    */

    // 创建并绑定一个顶点数组对象 (VAO)
    VertexArray va;
    va.Bind();
    // 顶点缓冲对象 (VBO)
    VertexBuffer vb(positions, sizeof(positions));// // 构造函数里面有bind，和填充data
    // 内存布局
    VertexBufferLayout layout;
    layout.Push<float>(2); // 2d坐标，两个元素
    layout.Push<float>(2); // 2个纹理坐标
    // 向va提交记录 即attr 和 point
    va.AddBuffer(vb, layout);
    // 索引缓冲对象 IBO 存储顶点索引
    unsigned int indices[] = {0, 1, 2,2, 3, 0};
    IndexBuffer ib(indices, 6); // 构造函数里面有bind，和填充data


    /**
     * 局部空间：模型内部空间
     * 世界空间：整个场景的通用坐标系
     * 视图空间：相机视角
     * 裁剪空间：视图空间通过投影矩阵转到标准坐标系，并进行裁剪，剔除视野范围外的模型
     * 标准化设备空间NDC：裁剪空间的标准化[-1, 1]
     * 屏幕空间：NDC映射到最终屏幕
     * 
     * 虚拟窗口 (ortho) → 压缩到 NDC [-1,1] → 映射到真实窗口 (glViewport)。
     * 如果 ortho 的宽高比 = 窗口宽高比 → 图形不会被拉伸。
     * 
     * 模型矩阵 (Model Matrix)
     *      从局部空间转换到世界空间。
     *      由物体的平移 (Translation)、旋转 (Rotation) 和缩放 (Scaling) 操作组合而成。
     * 
     * 视图矩阵 (View Matrix)
     *      将物体从世界空间转换到视图空间。它模拟了摄像机（观察者）的位置和朝向。
     *      它是摄像机位置和朝向的逆变换。
     * 
     * 投影矩阵 (Projection Matrix)
     *      从视图空间转换到裁剪空间。
     *      透视投影 (Perspective Projection) / 正交投影 (Orthographic Projection)
     */
    glm::vec3 translation(200, 200, 0);
    glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f); //正交投影矩阵，定义了虚拟窗口的像素，会映射到屏幕上的窗口
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));// 视图矩阵
    
    /**
     * 混合：当一个片段着色器输出颜色 srcColor 时，OpenGL 不会直接写到帧缓冲区，
     *          而是会跟缓冲区里原本的颜色 dstColor 按照规则混合，生成新的颜色。
     * 步骤：
     *      1:启动  glEnable(GL_BLEND);
     *      2:glBlendFunc(src, dst);    // 设置源和目标因子
     *                          GL_SRC_ALPHA → 值 = 源颜色的 alpha   
     *                          GL_ONE_MINUS_SRC_ALPHA → 值 = 1 - 源 alpha
     *      3:glBlendEquation(GL_FUNC_ADD); // 设置计算方法
                                GL_FUNC_SUBTRACT → src - dst
                                GL_FUNC_REVERSE_SUBTRACT → dst - src
     */
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // 创建并使用着色器程序
    Shader shader("res/Basic.shader");
    shader.Bind();
    // 统一变量
    shader.SetUniform4f("u_color", 0.8f, 0.3f, 0.8f, 1.0f);
    // shader.SetUniformMat4f("u_MVP", mvp);

    Texture texture("res/logo.png");
    texture.Bind(0);
    shader.SetUniform1i("u_Texture", 0); // 设置插槽位置，这里的value要和bind的相同


    // 解绑
    // shader.Unbind();
    // va.Unbind();
    // vb.UnBind();
    // ib.Unbind();

    // 渲染循环
    float r = 0.0f;
    float increament = 0.005f;

    Renderer render;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    // 需要指定GLSL版本, 也就是shader中的version
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    while (!glfwWindowShouldClose(window))
    {
        // 清空颜色缓冲
        render.Clear(); 

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 重新绑定
        // va.Bind(); // 顶点数组记录了 VBO EBO  attr  pointer
        shader.Bind();
        // shader.SetUniform4f("u_color", r, 0.3f, 0.8f, 1.0f);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);// 模型矩阵
        glm::mat4 mvp = proj * view * model;
        shader.SetUniformMat4f("u_MVP", mvp);

        // 绘制
        GLCall(render.Draw(va, ib, shader));
        

        if (r > 1.0f)
            increament = -0.05f;
        else if (r < 0.0f)
            increament = 0.005f;
        r += increament;

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::SliderFloat("translation", &translation.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // 交换前后缓冲
        glfwSwapBuffers(window);
        
        // 处理所有待处理的事件
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Shutting down OpenGL" << std::endl;
    return 0;
}