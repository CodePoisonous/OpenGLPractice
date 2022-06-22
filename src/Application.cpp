#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;
		float r = 0.0f;
		float increment = 0.05f;
		// ѭ��ֱ���û��رմ���
		while (!glfwWindowShouldClose(window))
		{
			// ��Ⱦ
			renderer.Clear();
			// GLCall(glClear(GL_COLOR_BUFFER_BIT));

			shader.Bind();			
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);	

			renderer.Draw(va, ib, shader);
			// va.Bind();
			// ib.Bind();
			// GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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
	}

	glfwTerminate();
	return 0;
}