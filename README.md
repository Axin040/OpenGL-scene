# learnOpenGL 学习教程

这个项目是从零开始学习 OpenGL 的完整实践记录，基于 [learnopengl.com](https://learnopengl.com)（[中文站](https://learnopengl-cn.github.io)）系列教程。项目使用 **Visual Studio 2022** (v143) + **C++17** 构建，采用 **Core Profile** 模式。

---

## 项目结构

```
learnOpenGL/
├── src/                  # 源代码
│   ├── main.cpp          # 程序入口
│   ├── Shader.h          # 着色器工具类
│   ├── Camera.h          # 自由视角摄像机
│   └── stb_image.h       # 图像加载库
├── shaders/              # GLSL 着色器文件
│   ├── vertex.glsl       # 基础顶点着色器
│   ├── fragment.glsl     # Phong 光照片元着色器
│   ├── lightvert.glsl    # 光源可视化顶点着色器
│   └── lightfrag.glsl    # 光源可视化片元着色器
├── include/              # 第三方库头文件
│   ├── glad/             # OpenGL 函数指针加载器
│   ├── KHR/              # 平台相关 OpenGL 扩展
│   └── glm/              # GLM 数学库
├── lib/                  # 编译好的库文件
│   └── glfw3.lib         # GLFW 窗口管理库
├── resources/textures/   # 纹理资源
│   ├── container.jpg
│   ├── container2.png
│   ├── container2_specular.png
│   └── awesomeface.png
├── learnOpenGL.sln       # VS 解决方案
└── learnOpenGL.vcxproj   # VS 项目文件
```

---

## 环境搭建

### 前置依赖

| 依赖         | 用途                  | 说明                                     |
| ------------ | --------------------- | ---------------------------------------- |
| **GLFW**     | 窗口创建 + 输入管理   | 已预编译为 `lib/glfw3.lib`               |
| **GLAD**     | OpenGL 函数加载器     | 源码在 `include/glad.c`，已加入项目编译  |
| **GLM**      | 数学库                | 仅头文件，`include/glm/` 直接引用        |
| **stb_image**| 纹理图像加载          | 单头文件库，`src/stb_image.h`            |

项目已配置好 `AdditionalIncludeDirectories` 和 `AdditionalLibraryDirectories`，Visual Studio 中直接按 **F5** 即可编译运行。Post-build 事件会自动把 `shaders/` 和 `resources/` 复制到输出目录。

---

## 代码结构规范

所有 OpenGL 程序都遵循同一个结构分界线：**创建放 main、绑定和绘制放循环**。

### main 函数（只执行一次）

```
1. 初始化窗口和 OpenGL
   glfwInit -> glfwCreateWindow -> gladLoadGL -> ...

2. 创建着色器程序
   Shader("vertex.glsl", "fragment.glsl")

3. 上传顶点数据到 GPU
   vertices / indices -> VAO / VBO / EBO
   glVertexAttribPointer / glEnableVertexAttribArray

4. 准备纹理等固定资源
   glGenTextures -> glTexParameteri
   stbi_load -> glTexImage2D -> glGenerateMipmap
```

### 渲染循环（每帧执行一次）

```
while (!glfwWindowShouldClose(window)) {
    1. 输入处理
       processInput(window)

    2. 清屏
       glClear(GL_COLOR_BUFFER_BIT)

    3. 绑定每帧不变的资源
       glActiveTexture -> glBindTexture

    4. 设置着色器 + 每帧变化的 uniform
       ourShader.use()
       setInt / setMat4 / glUniform*

    5. 绘制
       glBindVertexArray -> glDrawElements / glDrawArrays

    6. 交换缓冲区 + 轮询事件
       glfwSwapBuffers / glfwPollEvents
}```

### 分界原则

| 放在 main 中（循环外） | 放在渲染循环内 |
|---|---|
| 创建窗口、初始化 GLAD | 绑定纹理、绑定 VAO |
| 创建 VAO/VBO/EBO，上传顶点数据 | use() + 设置 uniform |
| 加载纹理图片到 GPU | glClear 清屏 |
| 创建 shader 程序对象 | glDraw* 绘制命令 |
| 只设置一次的值（如纹理单元映射） | 每帧变化的值（旋转角度、摄像机位置） |

### 为什么这样分

- glUniform* 必须在 glUseProgram(program) 之后调用才能生效。如果 uniform 放在循环外，下次循环里 use() 重新激活程序后，uniform 值虽然保留在程序对象中，但代码逻辑上割裂了——读代码的人要跳着看才能拼出完整渲染状态。

- 把 use() + uniform + bind + draw 写在一起，形成每帧一个完整的渲染块，既正确、又清晰。

---

## 当前进度

- [x] 创建 OpenGL 窗口 (GLFW + GLAD)
- [x] Hello Triangle（纹理四边形）
- [x] Shader 类封装
- [x] 纹理映射（stb_image）
- [x] 变换（GLM 已集成）
- [x] 坐标系（3D 投影）
- [x] 摄像机（Camera.h）
- [x] 基础光照（Phong 光照模型）
- [ ] 材质 + 光照贴图
- [ ] 多光源

---

## 常见问题

**Q: 运行后窗口一闪就关闭？**
A: 检查渲染循环是否正常运行，确认 `glfwWindowShouldClose` 能正常返回 `false`。

**Q: 屏幕为空，什么也没有？**
A: 检查着色器是否成功编译链接，以及 VAO/VBO 数据是否正确绑定。

**Q: 着色器报错 `ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ`**
A: 确保 `shaders/` 目录和可执行文件在同一目录。项目已配置 Post-build 事件自动复制。

**Q: 链接错误 `LNK2019: unresolved external symbol`**
A: 检查项目是否包含 `include/glad.c` 以及 `glfw3.lib` 路径是否正确。

---

## 参考资料

- [learnopengl.com](https://learnopengl.com) — 教程原文（英文）
- [learnopengl-cn.github.io](https://learnopengl-cn.github.io) — 中文翻译版
- [GLFW 文档](https://www.glfw.org/documentation.html)
- [GLM 手册](https://glm.g-truc.net/0.9.9/api/index.html)
