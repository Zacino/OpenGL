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


struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource parse_shader(const std::string& filepath)
{
    std::ifstream stream(filepath); 
    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

/*
 * 函数: compile_shader
 * 描述: 编译一个单个的OpenGL着色器。
 * 参数: type - 着色器类型 (GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER)
 * source - 包含着色器源代码的字符串
 * 返回: 编译成功的着色器ID，失败则返回0。
*/
static GLuint compile_shader(GLuint type, const std::string& source)
{
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // 检查着色器编译状态
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    
    return id;
}

/*
 * 函数: create_shader
 * 描述: 创建一个完整的着色器程序，包含顶点和片元着色器。
 * 参数: vertex_shader - 顶点着色器源代码
 * fragment_shader - 片元着色器源代码
 * 返回: 链接成功的着色器程序ID，失败则返回0。
 */
static GLuint create_shader(const std::string&  vertex_shader, const std::string&  fragment_shader)
{
    GLuint program = glCreateProgram();
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    if (vs == 0 || fs == 0)
    {
        // 如果任何一个着色器编译失败，则直接返回
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // 检查程序链接状态
    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cerr << "Failed to link shader program!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    // 验证程序（可选，但推荐）
    glValidateProgram(program);

    // 链接成功后，可以删除单独的着色器对象
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static void test_file(std::string path) {
    std::ifstream stream(path); 
    if (!stream.is_open()) {
        std::cerr << "错误: 无法打开着色器文件: " << std::endl;
    } else {
        std::string line;
        std::cout << "文件内容：" << std::endl;
        std::cout << "------------------------" << std::endl;
        while (std::getline(stream, line)) {
            std::cout << line << std::endl;
        }
        std::cout << "------------------------" << std::endl;
    }
    stream.close();
}

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

    // 针对 macOS 的兼容性设置
    // #ifdef __APPLE__
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // #endif
    
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


    // 顶点数据，包含三个顶点的x和y坐标
    float positions[] = {
        -0.5f, -0.5f,  // 0
         0.5f,  -0.5f, // 1
         0.5f, 0.5f,   // 2
         -0.5f, 0.5f,  // 3
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
    // 顶点缓冲对象 (VBO)
    VertexBuffer vb(positions, sizeof(positions));
    VertexBufferLayout layout;
    layout.Push<float>(2); // 2d坐标，两个元素
    va.AddBuffer(vb, layout);

    // 指定顶点属性的内存布局
    // 第一个参数 '0' 对应着色器中 layout(location = 0) 的属性
    va.Bind();
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    

    // 索引缓冲对象 IBO
    // 存储顶点索引
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    IndexBuffer ib(indices, 6);

    // 创建并使用着色器程序
    ShaderProgramSource source = parse_shader("res/Basic.shader"); // 路径默认是当前目录开始
    GLuint shader = create_shader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    // 统一变量
    int location = glGetUniformLocation(shader, "u_color");
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f)); // 修改

    // 解绑
    // glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 清空颜色缓冲
        glClear(GL_COLOR_BUFFER_BIT);

        // 重新绑定
        // glBindBuffer(GL_ARRAY_BUFFER, buffer);
        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        va.Bind();


        // 绘制三角形
        /*
            glDrawArrays(GLenum mode, GLint first, GLsizei count);
                这个函数以线性、连续的方式从顶点缓冲（VBO）中读取数据。
            glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);
                这个函数通过一个额外的索引缓冲（EBO）以间接、非连续的方式从VBO中读取数据。
        */
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));



        // 交换前后缓冲
        glfwSwapBuffers(window);
        
        // 处理所有待处理的事件
        glfwPollEvents();
    }
    
    std::cout << "Shutting down OpenGL" << std::endl;
    glfwTerminate();
    return 0;
}