// OpenGLeXperiments.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Shader.h"
#include "Camera.h"
#include "Icosahedron.h"
#include <cstdlib>

const GLuint WIDTH = 800, HEIGHT = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

float lightX = 1, lightY = 0, lightZ = 1;

//Camera variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool firstMouse = true;
bool keys[1024];

void Do_Movement();
void Switch_Tess();
void Move_Light();

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

static const GLuint PositionSlot = 0;

//OpenGL Global section
typedef struct {
	GLuint Projection;
	GLuint Model;
	GLuint View;
	GLuint NormalMatrix;
	GLuint LightPosition;
	GLuint AmbientMaterial;
	GLuint DiffuseMaterial;
	GLuint TessLevelInner;
	GLuint TessLevelOuter;
	//GLuint inPoint0;
	//GLuint inPoint1;
	//GLuint inPoint2;
	GLuint areaA;
	GLuint areaB;
	GLuint areaC;
	GLuint areaD;
} ShaderUniforms;

static float TessLevelInner;
static float TessLevelOuter;
static glm::mat4 ProjectionMatrix;
static glm::mat4 ModelMatrix;
static glm::mat4 ViewMatrix;
static glm::mat3 NormalMatrix;
static ShaderUniforms Uniforms;

// OpenGL Global section END

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);


int main()
{
#pragma region SETUP
	//glfw init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Debug section
	/*glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}*/

	//window
	auto window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//optional
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glew init
	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
#pragma endregion SETUP

	//opengl options
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	//shader debug
	//Shader myShader("Shaders/shader.vs", "Shaders/shader.frag");
	//Uniforms.Model = glGetUniformLocation(myShader.Program, "model");
	//Uniforms.View = glGetUniformLocation(myShader.Program, "view");
	//Uniforms.Projection2 = glGetUniformLocation(myShader.Program, "projection");

	//rand init
	srand(27);
	float x = rand() % 60 + 20;
	float y = rand() % 60 + 20;
	float z = rand() % 60 + 20;
	x /= 100;
	y /= 100;
	z /= 100;
	glm::vec3 size = glm::vec3(x, y, z);
	std::cout << size.x << "," << size.y << "," << size.z << std::endl;
	bool arr[4] = { false, false, false, false };
	auto max = rand() % 5;
	std::cout << max << std::endl;
	for (auto i = 0; i < max; ++i)
	{
		if (rand() % 2 == 1)
			arr[i] = true;
	}

	for (auto i = 0; i < 4; ++i)
		std::cout << "arr[" << i << "] : " << arr[i] << std::endl;

	//shader final
	Shader myShader2("Shaders/transform.vs", "Shaders/transform.frag", "Shaders/transform.geom", "Shaders/transform.tcs", "Shaders/transform.tes");
	Uniforms.Projection = glGetUniformLocation(myShader2.Program, "Projection");
	Uniforms.Model = glGetUniformLocation(myShader2.Program, "Model");
	Uniforms.View = glGetUniformLocation(myShader2.Program, "View");
	Uniforms.NormalMatrix = glGetUniformLocation(myShader2.Program, "NormalMatrix");
	Uniforms.LightPosition = glGetUniformLocation(myShader2.Program, "LightPosition");
	Uniforms.AmbientMaterial = glGetUniformLocation(myShader2.Program, "AmbientMaterial");
	Uniforms.DiffuseMaterial = glGetUniformLocation(myShader2.Program, "DiffuseMaterial");
	Uniforms.TessLevelInner = glGetUniformLocation(myShader2.Program, "TessLevelInner");
	Uniforms.TessLevelOuter = glGetUniformLocation(myShader2.Program, "TessLevelOuter");
	//Uniforms.inPoint0 = glGetUniformLocation(myShader2.Program, "inPoint0");
	//Uniforms.inPoint1 = glGetUniformLocation(myShader2.Program, "inPoint1");
	//Uniforms.inPoint2 = glGetUniformLocation(myShader2.Program, "inPoint2");
	Uniforms.areaA = glGetUniformLocation(myShader2.Program, "areaA");
	Uniforms.areaB = glGetUniformLocation(myShader2.Program, "areaB");
	Uniforms.areaC = glGetUniformLocation(myShader2.Program, "areaC");
	Uniforms.areaD = glGetUniformLocation(myShader2.Program, "areaD");

	//sphere
	auto sphere = Icosahedron();

	//Init tess values
	TessLevelInner = 10;
	TessLevelOuter = 10;

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//GameLoop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();
		Switch_Tess();
		Move_Light();

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPatchParameteri(GL_PATCH_VERTICES, 3);

		// shader
		myShader2.Use();

		// Create camera transformation
		ViewMatrix = camera.GetViewMatrix();
		ProjectionMatrix = glm::perspective(camera.Zoom, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);
		NormalMatrix = glm::mat3(ViewMatrix);

		//tess param
		glUniform1f(Uniforms.TessLevelInner, TessLevelInner);
		glUniform1f(Uniforms.TessLevelOuter, TessLevelOuter);
		//light param
		glUniform3f(Uniforms.AmbientMaterial, 0.0001f, 0.0001f, 0.0001f);
		glUniform3f(Uniforms.DiffuseMaterial, 0.329412, 0.329412, 0.329412);
		glm::vec4 lightPosition(-lightX, -lightY, -lightZ, 0);
		glUniform3fv(Uniforms.LightPosition, 1, glm::value_ptr(lightPosition));

		//send matrix
		glUniformMatrix4fv(Uniforms.View, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
		glUniformMatrix4fv(Uniforms.Projection, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
		ModelMatrix = glm::mat4();
		ModelMatrix = glm::scale(glm::mat4(), size);
		glUniformMatrix4fv(Uniforms.Model, 1, GL_FALSE, glm::value_ptr(ModelMatrix));

		//Pass parameters to shader
		glm::mat3 nm = glm::transpose(NormalMatrix);
		glUniformMatrix3fv(Uniforms.NormalMatrix, 1, GL_FALSE, glm::value_ptr(nm));

		//param points
		//glUniform3f(Uniforms.inPoint0, 0.9, 0.1, 0);
		//glUniform3f(Uniforms.inPoint1, 0, 0.1, 0.75);
		//glUniform3f(Uniforms.inPoint2, 0.1, 0.9, 0);

		//rand
		glUniform1i(Uniforms.areaA, arr[0]);
		glUniform1i(Uniforms.areaB, arr[1]);
		glUniform1i(Uniforms.areaC, arr[2]);
		glUniform1i(Uniforms.areaD, arr[3]);

		sphere.draw();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_Z])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_Q])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Switch_Tess()
{
	if (keys[GLFW_KEY_P])
		TessLevelOuter++;
	if (keys[GLFW_KEY_O])
		TessLevelOuter--;
	if (keys[GLFW_KEY_M])
		TessLevelInner++;
	if (keys[GLFW_KEY_L])
		TessLevelInner++;
	if (keys[GLFW_KEY_R])
	{
		TessLevelOuter = 5;
		TessLevelInner = 5;
		lightX = 0;
		lightY = 0;
		lightZ = 0;
	}
}

void Move_Light()
{
	if (keys[GLFW_KEY_H])
		lightX += 0.1;
	if (keys[GLFW_KEY_J])
		lightY += 0.1;
	if (keys[GLFW_KEY_K])
		lightZ += 0.1;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}