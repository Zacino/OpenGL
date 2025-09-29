#shader vertex
 #version 330 core
 layout(location = 0) in vec4 position; //通过在顶点数组指定的layout取出这些值
 layout(location = 1) in vec2 texCoord; 

out vec2 v_TexCoord; //着色器阶段之间的数据通道，out传递给下个着色器，即顶点着色器--->片段着色器

uniform mat4 u_MVP;

 void main()   
 {   
     gl_Position = u_MVP * position; // OpenGL 内置的特殊变量
     v_TexCoord = texCoord;
 }

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;   

in vec2 v_TexCoord;

uniform vec4 u_color;
uniform sampler2D u_Texture;
void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = texColor;   
}