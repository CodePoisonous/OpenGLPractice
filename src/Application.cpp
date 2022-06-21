#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// ���shader�����Ƿ�ɹ�
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));	// ��stack�Ͻ��ж�̬����
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " 
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			<< " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	// ��ʼ��glfw
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��������ģʽ���ڼ��� OpenGL ������
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// ʹwindow��Ϊ��ǰ��������
	glfwMakeContextCurrent(window);

	// ����ˢ����Ϊ60֡
	glfwSwapInterval(1);

	// �ж�glew��ʼ���Ƿ�ɹ�
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	// ��ȡgl�汾��
	std::cout << glGetString(GL_VERSION) << std::endl;

	{
		// ͼ�ζ˵�λ��xy����
		float positions[] = {
			-0.5f, -0.5f,	// 0
			 0.5f, -0.5f,	// 1 
			 0.5f,  0.5f,	// 2
			-0.5f,  0.5f,	// 3 
		};

		// ͼ�ζ˵���������
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
			
		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);			// positions Ԫ������Ϊ float, ����Ϊxy����Ԫ��һ��
		va.AddBuffer(vb, layout);
		
		IndexBuffer ib(indices, 6);

		// ��ȡshader�ļ�������shader program
		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		GLCall(glUseProgram(shader));

		// ����fragment shader ����ɫ
		GLCall(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);
		GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

		// ���
		va.Unbind();
		GLCall(glUseProgram(0));
		vb.Unbind();
		ib.Unbind();

		float r = 0.0f;
		float increment = 0.05f;
		// ѭ��ֱ���û��رմ���
		while (!glfwWindowShouldClose(window))
		{
			// ��Ⱦ
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			va.Bind();
			ib.Bind();

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			// ����ǰ�󻺳���
			glfwSwapBuffers(window);

			// ��ѯ�ʹ����¼�
			glfwPollEvents();
		}

		GLCall(glDeleteProgram(shader));
	}

	glfwTerminate();
	return 0;
}