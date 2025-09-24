#pragma once

#include <vector>
#include <GL/glew.h>
#include "Render.h"

// (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
// glVertexAttribPointer()的参数
/**
    index：顶点内部的属性位置 0-position    1-texture
    size：这个属性有几个数
    stride：顶点的大小！！ 顶点里每个属性都相同
    
 */

/**
    假设一个顶点：
    struct Vertex {
        float position[3];   // x,y,z
        float texCoords[2];  // u,v
        unsigned char color[4]; // r,g,b,a
    };
    那么用这个 VertexBufferLayout 描述就是：
    VertexBufferLayout layout;
    layout.Push<float>(3);         // 位置: 3个float
    layout.Push<float>(2);         // 纹理坐标: 2个float
    layout.Push<unsigned char>(4); // 颜色: 4个unsigned char
    m_stride = 24
    这样 OpenGL 就知道：每个顶点大小 24 字节，前 12 字节是位置，接下来 8 字节是纹理坐标，最后 4 字节是颜色。
 **/

struct VertexBufferElement
{
	unsigned int type; // 
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout(): m_Stride(0) {}

    // 模板声明
    // 但这里故意写了 static_assert(false)，意思是：
    // 如果用户用未定义的类型调用 Push，比如 Push<double>()，直接编译报错！
	template<typename T>
	void Push(unsigned int count) 
    // 向 m_Elements 添加一个元素，类型是 GL_FLOAT，数量是 count，比如 Push<float>(3) 就表示顶点的某一部分有 3 个 float（比如位置 x,y,z）。
    // 更新 m_Stride，把这部分占的字节数加上。
	{
		static_assert(false);
	}

    // 模板特化
    // 有时候，你想 针对某个具体类型，写不同的实现。就会走 特化版本，而不是普通版本。
	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};