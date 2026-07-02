// GLAD: 管理 OpenGL 函数指针的库，必须在 GLFW 之前包含
#include<glad/glad.h>
// GLFW: 窗口创建和输入处理的库
#include<GLFW/glfw3.h>
#include<iostream>
#include<Shader.h>

// stb_image: 纹理加载库，STB_IMAGE_IMPLEMENTATION 确保只编译一次实现
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include <glm/gtc/type_ptr.hpp>
//摄像机
#include<Camera.h>

// 窗口大小变化时的回调函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 处理键盘输入
void processInput(GLFWwindow* window);
// 处理鼠标移动输入
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
// 处理鼠标滚轮输入
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 窗口初始宽度和高度
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//设置摄像机初始位置
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0; // 窗口宽度的一半
float lastY = 600.0f / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; 

//初始化窗口
int main()
{
    // 初始化 GLFW，并设置 OpenGL 版本为 3.3 Core Profile
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口，宽 800，高 600
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 将窗口的 OpenGL 上下文设为当前线程的主上下文
    glfwMakeContextCurrent(window);
    // 注册窗口大小变化的回调
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 告诉 GLFW 捕获鼠标光标（隐藏并锁定在窗口内）
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // 初始化 GLAD：加载当前 OpenGL 上下文的所有函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	glEnable(GL_DEPTH_TEST); // 启用深度测试
    Shader ourShader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // 四边形（两个三角形组成）的顶点数据：位置 + 颜色 + 纹理坐标
    float vertices[] = {
     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // 创建 VAO、VBO、EBO（索引缓冲对象）
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 绑定 VAO → 之后的所有顶点属性配置都记录到此 VAO
    glBindVertexArray(VAO);
    // 绑定 VBO，上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 位置属性 (location = 0): 3 个 float，步长 5 个 float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性 (location = 2): 2 个 float，偏移 3 个 float
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 加载并创建纹理
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // 纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // stb_image 翻转 y 轴（OpenGL 纹理坐标原点在左下角）
    stbi_set_flip_vertically_on_load(true);

    // 加载纹理图片
    int width, height, nrChannels;
    unsigned char* data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //加载第二张纹理
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // 解绑 VBO（EBO 记录在 VAO 中不需要解绑）
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑 VAO（渲染循环中再绑定）
    glBindVertexArray(0);

    // 主渲染循环：每帧执行一次，直到窗口关闭
    while (!glfwWindowShouldClose(window))
    {
		// 计算每帧时间差
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入（如 ESC 键退出）
        processInput(window);

        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绑定纹理到纹理单元 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // 激活着色器程序并绑定 VAO，然后绘制四边形
        ourShader.use();
        ourShader.setInt("Texture1", 0);
        ourShader.setInt("Texture2", 1);

        //变换矩阵
       glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(50.0f) * (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		// 将变换矩阵传递给着色器
		ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
        

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0, 36);

        // 交换前后缓冲区
        glfwSwapBuffers(window);
        // 处理窗口事件
        glfwPollEvents();
    }

    // 程序结束，清理 GPU 资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);

    glfwTerminate();
    return 0;
}

// 窗口大小变化时，调整 OpenGL 视口大小匹配新窗口
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 检测键盘输入：按下 ESC 则通知 GLFW 关闭窗口
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
