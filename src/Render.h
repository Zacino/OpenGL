#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


// 错误检查
#define ASSERT(x) if(!(x)) __builtin_trap();  // 宏替换的细节 (x)
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);