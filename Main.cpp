#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shaderUtils.h>
#include "TessShader.h"
#include "texture.h"

#include <camera.h>
#include <stb_image.h>
#include <model.h>

#include "perlin/HeightMap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <stdlib.h>
#include <iostream>
#include <stdlib.h> 


// HEIGHT MAP & TERRAIN
int h = 2048, ww = 1024; // Resolution
float HeightRange = 60;
bool generateTerrain = false;

const int MAX_FPS = 60,
HEIGHT_SCENE = 200,
WIDTH_SCENE = 200;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// screen settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(5.0f, 2.5f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float t1 = 0.0, t2 = 0.0, frameTime = 0.0; // time variables
bool wireframe = false;
glm::mat4 identityMatrix;

int main()
{

	if (generateTerrain) {
		// HEIGHT MAP & TERRAIN
		int h = 4096, ww = 1024; // Resolution
		float HeightRange = 60;

		srand(time(NULL));
		HeightMap* hm = new HeightMap(h, HeightRange, 10, rand() % 10, 0.5);
		hm->saveMap("resources/heightMap.bmp");
	}
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														
	// glfw window creation										 
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tessellation Shader on terrain", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader skyboxShader("shaders/skyboxVert.vert", "shaders/skyboxFrag.frag");
	//Shader program2("vertexShader1.glsl", "fragmentShader1.glsl");
	//Shader modelLoading("model_loading.vs", "model_loading.fs");
	TessellationShader tshader("shaders/tessVertexShader.vert", "shaders/tessControlShader.tcs", "shaders/tessEvaluationShader.tes", "shaders/tessFragmentShader.frag");
	TessellationShader tshader2("shaders/tessSmoothingVertexShader.vert", "shaders/tessSmoothingControlShader.tcs", "shaders/tessSmoothingEvaluationShader.tes", "shaders/tessSmoothingFragmentShader2.frag");
	TessellationShader lightShader2("shaders/tessSmoothingVertexShader.vert", "shaders/tessSmoothingControlShader.tcs", "shaders/tessSmoothingEvaluationShader.tes", "shaders/lightFragmentShader.frag");

	// cube vertices
	float vertices1[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	//cube texture coordinates
	float textureCoordinate[] = {
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
	};
	
	// initializing buffer objects
	unsigned int VAO1, VBO1;// , EBO1;
	glGenVertexArrays(2, &VAO1);
	glGenBuffers(2, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	//send vertices information
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//send normal information assigning a variable in shader. 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	/* in case of indices drawing
	glGenBuffers(1, &EBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/
	glBindVertexArray(0);

	// lampVAO;
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// skyboxVAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// set light matrices
	glm::mat4 lightModel, idMat;
	glm::vec3 lightPosition = glm::vec3(-5.0f, 5.0f, -5.0f);
	lightModel = glm::translate(lightModel, lightPosition);
	lightModel = glm::scale(lightModel, glm::vec3(0.5, 0.5, 0.5));
	
	//loading models
	Model plane_("resources/plane.obj", GL_PATCHES);
	Model monkey("resources/head.obj", GL_PATCHES);
	Model sphere("resources/sphere.obj", GL_PATCHES);

	//init textures
	unsigned int texture = TextureFromFile("heightMap.bmp", "resources", false);
	unsigned int texture1 = TextureFromFile("terrainTexture.jpg", "resources", false);

	vector<std::string> faces =
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	glm::vec3 fogColor(204, 224, 255);
	fogColor /= 255.0;
	glm::vec3 lightColor(255, 255, 153);
	lightColor /= 255.0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		t1 = glfwGetTime(); 
		float degreePerSecond = 10.0f;
		float dist = 50.0;
		float x = cos(glm::radians(t1*degreePerSecond))*dist;
		float z = sin(glm::radians(t1*degreePerSecond))*dist;
		lightPosition = glm::vec3(x, 7.5, z); //ruotate light

		// input
		processInput(window);

		// render
		glEnable(GL_DEPTH_TEST);
		glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// toggle/untoggle wireframe mode
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Camera (View Matrix) setting
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


		// draw terrain
		// set matrices
		glm::mat4 gWorld, gVP;
		gWorld = glm::scale(gWorld, glm::vec3(5.0, 0.0, 5.0));
		gVP = proj * view;


		if (true) {
			// Draw the terrain
			// set uniform variables
			tshader.use();
			tshader.setVec3("gEyeWorldPos", camera.Position); // terrain shader
			tshader.setMat4("gWorld", gWorld);
			tshader.setMat4("gVP", gVP);
			tshader.setFloat("gDispFactor", 4.5f);
			tshader.setVec3("u_LightColor", lightColor);
			tshader.setVec3("u_LightPosition", lightPosition);
			tshader.setVec3("u_ViewPosition", camera.Position);
			tshader.setVec3("fogColor", fogColor);

			// set textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			tshader.setInt("gDisplacementMap", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			tshader.setInt("tex", 1);

			// finally, draw
			plane_.Draw(tshader);

		}

		if (false) {
			tshader2.use();
			tshader2.setVec3("gEyeWorldPos", camera.Position); // terrain shader
			tshader2.setFloat("gTessellationLevel", 4.0f);
			tshader2.setMat4("gWorld", glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0)));
			tshader2.setMat4("gVP", gVP);
			tshader2.setVec3("u_LightColor", lightColor);
			tshader2.setVec3("u_LightPosition", lightPosition);
			tshader2.setVec3("u_ViewPosition", camera.Position);
			tshader2.setVec3("fogColor", fogColor);

			monkey.Draw(tshader2);
		}

		// draw light source
		lightShader2.use();
		glm::mat4 rot = glm::rotate(identityMatrix, glm::radians(-t1 * degreePerSecond), glm::vec3(0.0, 1.0, 0.0));
		lightModel = glm::translate(idMat, lightPosition) * rot; //update light model
		lightShader2.setMat4("gWorld", lightModel);
		lightShader2.setMat4("gVP", gVP);
		lightShader2.setFloat("gTessellationLevel", 5.0f);
		lightShader2.setVec3("u_LightColor", lightColor);
		sphere.Draw(lightShader2);

		//draw skyBox
		glDepthFunc(GL_LEQUAL);
		//glDepthMask(GL_FALSE);
		skyboxShader.use();
		glm::mat4 view2 = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0, -0.1, 0.0));
		skyboxShader.setMat4("model", model);
		skyboxShader.setMat4("view", view2);
		skyboxShader.setMat4("projection", proj);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

		t2 = glfwGetTime();
		frameTime = t2 - t1;
		float targetFrameRate = 60.0f;
		float timeToSleep = 1000.0f / targetFrameRate - (t2 - t1)*1000.0f;
		if (timeToSleep > 0.0f) {
			Sleep(timeToSleep);
		}
		t2 = glfwGetTime();
		frameTime = t2 - t1;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, frameTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, frameTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, frameTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, frameTime);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) 
		wireframe = !wireframe;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	std::cout << "processing mouse scroll" << std::endl;
	camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}