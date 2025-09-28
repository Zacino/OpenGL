#include "Render.h"
#include "Shader.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR) {}
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): "
            << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;   
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind(); /* 为GPU绑定着色器程序 */
    va.Bind(); /* 包含实际处理数据的数组 */
    ib.Bind();

    // glDrawArrays(GL_TRIANGLES, 0, 3); # 以线性、连续的方式从顶点缓冲（VBO）中读取数据。
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); # 通过一个额外的索引缓冲（EBO）以间接、非连续的方式从VBO中读取数据。
    // GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}