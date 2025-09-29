#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Render.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID)); /* 生成顶点数组 */
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];

		GLCall(glEnableVertexAttribArray(i)); /* 启用指定索引i的常规顶点属性 */
		// void* 是通用指针，它可以指向任何类型的数据，但你不能直接解引用它，因为编译器不知道它指向的数据是什么类型。需要强转回来才能用
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)(uintptr_t)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}

}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}