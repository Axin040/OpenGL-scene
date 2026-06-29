// GLAD: 管理 OpenGL 函数指针的库，必须在 GLFW 之前包含
#include<glad/glad.h>
// GLFW: 窗口创建和输入处理的库
#include<GLFW/glfw3.h>
#include<iostream>
#include<Shader.h>

// stb_image: 纹理加载库，STB_IMAGE_IMPLEMENTATION 确保只编译一次实现
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

// 窗口大小变化时的回调函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 处理键盘输入
void processInput(GLFWwindow* window);

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

    // 初始化 GLAD：加载当前 OpenGL 上下文的所有函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // 四边形（两个三角形组成）的顶点数据：位置 + 颜色 + 纹理坐标
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // 右上
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // 左下
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f   // 左上
    };
    unsigned int indices[] = {
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };

    // 创建 VAO、VBO、EBO（索引缓冲对象）
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 绑定 VAO → 之后的所有顶点属性配置都记录到此 VAO
    glBindVertexArray(VAO);

    // 绑定 VBO，上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定 EBO，上传索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 位置属性 (location = 0): 3 个 float，步长 8 个 float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性 (location = 1): 3 个 float，偏移 3 个 float
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标属性 (location = 2): 2 个 float，偏移 6 个 float
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 加载并创建纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

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

    // 解绑 VBO（EBO 记录在 VAO 中不需要解绑）
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑 VAO（渲染循环中再绑定）
    glBindVertexArray(0);

    // 主渲染循环：每帧执行一次，直到窗口关闭
    while (!glfwWindowShouldClose(window))
    {
        // 处理输入（如 ESC 键退出）
        processInput(window);

        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绑定纹理到纹理单元 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // 激活着色器程序并绑定 VAO，然后绘制四边形
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 交换前后缓冲区
        glfwSwapBuffers(window);
        // 处理窗口事件
        glfwPollEvents();
    }

    // 程序结束，清理 GPU 资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);

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
}
