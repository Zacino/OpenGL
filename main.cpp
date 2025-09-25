#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>

#include "src/Render.h"
#include "src/VertexBuffer.h"
#include "src/IndexBuffer.h"
#include "src/VertexArray.h"
#include "src/VertexBufferLayout.h"
#include "src/Shader.h"
#include "src/Texture.h"

// test branch


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
    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
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

    // only for test

    // 顶点数据，包含三个顶点的x和y坐标
    float positions[] = {
        -0.5f, -0.5f,  0.0f, 0.0f, // 0
         0.5f,  -0.5f, 1.0f, 0.0f,// 1
         0.5f, 0.5f,  1.0f, 1.0f,// 2
         -0.5f, 0.5f,  0.0f, 1.0f,// 3
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
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    IndexBuffer ib(indices, 6); // 构造函数里面有bind，和填充data
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // 创建并使用着色器程序
    Shader shader("res/Basic.shader");
    shader.Bind();
    // 统一变量
    shader.SetUniform4f("u_color", 0.8f, 0.3f, 0.8f, 1.0f);

    Texture texture("res/logo.png");
    texture.Bind(0);
    shader.SetUniform1i("u_Texture", 0); // 和bind相同


    // 解绑
    shader.Unbind();
    va.Unbind();
    vb.UnBind();
    ib.Unbind();

    // 渲染循环
    float r = 0.0f;
    float increament = 0.005f;

    Renderer render;
    
    while (!glfwWindowShouldClose(window))
    {
        Renderer render;
        // 清空颜色缓冲
        // glClear(GL_COLOR_BUFFER_BIT);
        render.Clear();

        // 重新绑定
        // va.Bind(); // 顶点数组记录了 VBO EBO  attr  pointer
        shader.Bind();
        shader.SetUniform4f("u_color", r, 0.3f, 0.8f, 1.0f);

        GLCall(render.Draw(va, ib, shader));

        // 绘制
        // glDrawArrays(GL_TRIANGLES, 0, 3); # 以线性、连续的方式从顶点缓冲（VBO）中读取数据。
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); # 通过一个额外的索引缓冲（EBO）以间接、非连续的方式从VBO中读取数据。
        // GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        

        if (r > 1.0f)
            increament = -0.05f;
        else if (r < 0.0f)
            increament = 0.005f;
        r += increament;
        
        // 交换前后缓冲
        glfwSwapBuffers(window);
        
        // 处理所有待处理的事件
        glfwPollEvents();
    }
    
    std::cout << "Shutting down OpenGL" << std::endl;
    glfwTerminate();
    return 0;
}