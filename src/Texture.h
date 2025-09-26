#pragma once

#include "Render.h"

class Texture
{
private:
	unsigned int m_RendererID; // OpenGL 生成的纹理对象 ID
	std::string m_FilePath; // 图片路径
	unsigned char* m_LocalBuffer; // 图片在内存中的buffer
	int m_Width, m_Height, m_BPP; // 宽度，高度，每像素字节数
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};