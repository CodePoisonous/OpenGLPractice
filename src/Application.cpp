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
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
			-0.5f, -0.5f, 0.0f, 0.0f,	// 0
			 0.5f, -0.5f, 1.0f, 0.0f,	// 1 
			 0.5f,  0.5f, 1.0f, 1.0f,	// 2
			-0.5f,  0.5f, 0.0f, 1.0f	// 3 
		};

		// ͼ�ζ˵���������
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
				
		GLCall(glEnable(GL_BLEND));									// ���û�ϣ�����src��destination��alphaֵ���û�Ϻ��RGBAֵ
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));	// ���㷽����dest RGBA = src RGBA * src A + dest RGBA * (1 - src A)

		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);			// positions Ԫ������Ϊ float, ����Ϊxy����Ԫ��һ��
		layout.Push<float>(2);			// texture
		va.AddBuffer(vb, layout);
		
		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);		// orthographic matrix ��������

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		//shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", proj);

		Texture texture("res/textures/ChernoLogo.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
		
		Renderer renderer;
		// ѭ��ֱ���û��رմ���
		while (!glfwWindowShouldClose(window))
		{
			// ��Ⱦ
			renderer.Clear();
			
			renderer.Draw(va, ib, shader);

			// ����ǰ�󻺳���
			glfwSwapBuffers(window);

			// ��ѯ�ʹ����¼�
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}