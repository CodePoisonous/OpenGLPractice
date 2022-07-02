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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

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
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
			-50.0f, -50.0f, 0.0f, 0.0f,	// 0
			 50.0f, -50.0f, 1.0f, 0.0f,	// 1 
			 50.0f,  50.0f, 1.0f, 1.0f,	// 2
			-50.0f,  50.0f, 0.0f, 1.0f	// 3 
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

		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);		// orthographic matrix ��������
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		Texture texture("res/textures/ChernoLogo.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		// Setup ImGui binding
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);

		// Setup style
		ImGui::StyleColorsDark();

		// ѭ��ֱ���û��رմ���
		glm::vec3 translationA(200, 200, 0);
		glm::vec3 translationB(400, 200, 0);
		while (!glfwWindowShouldClose(window))
		{
			// ��Ⱦ
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();
			
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);

			}

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				//renderer.Draw(va, ib, shader);
				shader.SetUniformMat4f("u_MVP", mvp);
			}

			renderer.Draw(va, ib, shader);

			// Show a simple window.
			{
				ImGui::SliderFloat3("translation A", &translationA.x, 0.0f, 960.0f);
				ImGui::SliderFloat3("translation B", &translationB.x, 0.0f, 960.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


			// ����ǰ�󻺳���
			glfwSwapBuffers(window);

			// ��ѯ�ʹ����¼�
			glfwPollEvents();
		}
	}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}