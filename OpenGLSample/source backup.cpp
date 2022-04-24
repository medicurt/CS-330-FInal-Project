#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sphere.h"

#include "shader.h"

#include <iostream>

#include "cylinder.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(-12.0f, 1.0f, -1.0f);
glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCREEN_WIDTH / 2.0;
float lastY = SCREEN_HEIGHT / 2.0;
//set to 60 so the scene is easier to find once lost
float zoom = 60.0f;
float speed = 4.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//boolean controlling orthogonality
bool Ortho = false;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CTHOMAS Milestone One", NULL, NULL);
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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		//plane triangle one; indeces 0, 1, 2 go from top-left to top-right to bottom-right
		-10.0f, 0.0f,  10.0f, 0.0f, 0.0f,
		 10.0f, 0.0f,  10.0f,  12.0f, 0.0f,
		 10.0f, 0.0f, -10.0f,  12.0f, 12.0f,
		 //plane triangle two; indeces 3, 4, 5 go from bottom-right to bottom-left to top-left
		 10.0f,  0.0f, -10.0f,  12.0f, 12.0f,
		-10.0f,  0.0f, -10.0f,  0.0f, 12.0f,
		-10.0f,  0.0f,  10.0f,  0.0f, 0.0f,
		
	};

	float bowl[] = {
		//bottom of the bowl
		/*
		1.06f, 0.2f, 1.06f, //A
		0.0f, 0.2f, 1.50f, //B
		-1.06f, 0.2f, 1.06f, //C
		-1.50f, 0.2f, 0.00f, //D
		-1.06f, 0.2f, -1.06f, //E
		0.0f, 0.2f, -1.50f, //F
		1.06f, 0.2f, -1.06f, //G
		1.50f, 0.2f, 0.00f, //H
		*/

		//top of the bowl
		/*
		1.41f, 1.5f, 1.41f, //a
		0.00f, 1.5f, 2.00f, //b
		-1.41f, 1.5f, 1.41f, //c
		-2.00f, 1.5f, 0.00f, //d
		-1.41f, 1.5f, -1.41f, //e
		-0.00f, 1.5f, -2.00f, //f
		1.41f, 1.5f, -1.41f, //g
		2.00f, 1.5f, 0.00f, //h
		*/

		//middle bottom of the bowl
		//0.0f, 0.0f, 0.0f, //o

		//bottom of the bowl in triangles
		0.0f, 0.0f, 0.0f, 0.2, 0.0, //o
		1.06f, 0.2f, 1.06f, 0.5, 0.5,//A
		0.0f, 0.2f, 1.50f, 0.7, 0.0,//B

		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		0.0f, 0.2f, 1.50f, 0.5, 0.5,//B
		-1.06f, 0.2f, 1.06f, 0.7, 0.0,//C

		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		-1.06f, 0.2f, 1.06f, 0.5, 0.5,//C
		-1.50f, 0.2f, 0.00f, 0.7, 0.0,//D

		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		-1.50f, 0.2f, 0.00f, 0.5, 0.5,//D
		-1.06f, 0.2f, -1.06f, 0.7, 0.0,//E

		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		-1.06f, 0.2f, -1.06f, 0.5, 0.5,//E
		0.0f, 0.2f, -1.50f, 0.7, 0.0,//F

		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		0.0f, 0.2f, -1.50f, 0.5, 0.5,//F
		1.06f, 0.2f, -1.06f, 0.7, 0.0,//G
		
		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		1.06f, 0.2f, -1.06f, 0.5, 0.5,//G
		1.50f, 0.2f, 0.00f, 0.7, 0.0,//H
		
		0.0f, 0.0f, 0.0f, 0.2, 0.0,//o
		1.50f, 0.2f, 0.00f, 0.5, 0.5,//H
		1.06f, 0.2f, 1.06f, 0.7, 0.0,//A

		//side of the bowl in triangles
		//follows an anti-clockwise pattern of top-top-BOTTOM triangle followed by BOTTOM-BOTTOM-top triangle
		1.41f, 1.5f, 1.41f, 1.0, 1.0,//a
		0.00f, 1.5f, 2.00f, 0.0, 1.0,//b
		1.06f, 0.2f, 1.06f, 1.0, 0.0,//A

		1.06f, 0.2f, 1.06f, 1.0, 0.0,//A
		0.0f, 0.2f, 1.50f, 0.0, 0.0,//B
		0.00f, 1.5f, 2.00f, 0.0, 1.0,//b

		0.00f, 1.5f, 2.00f, 1.0, 1.0,//b
		-1.41f, 1.5f, 1.41f, 0.0, 1.0,//c
		0.0f, 0.2f, 1.50f, 1.0, 0.0,//B

		0.0f, 0.2f, 1.50f, 1.0, 0.0,//B
		-1.06f, 0.2f, 1.06f, 0.0, 0.0,//C
		-1.41f, 1.5f, 1.41f, 0.0, 1.0,//c

		-1.41f, 1.5f, 1.41f, 1.0, 1.0,//c
		-2.00f, 1.5f, 0.00f, 0.0, 1.0,//d
		-1.06f, 0.2f, 1.06f, 1.0, 0.0,//C

		-1.06f, 0.2f, 1.06f, 1.0, 0.0,//C
		-1.50f, 0.2f, 0.00f, 0.0, 0.0,//D
		-2.00f, 1.5f, 0.00f, 0.0, 1.0,//d

		-2.00f, 1.5f, 0.00f, 1.0, 1.0,//d
		-1.41f, 1.5f, -1.41f, 0.0, 1.0,//e
		-1.50f, 0.2f, 0.00f, 1.0, 0.0,//D

		-1.50f, 0.2f, 0.00f, 1.0, 0.0,//D
		-1.06f, 0.2f, -1.06f, 0.0, 0.0,//E
		-1.41f, 1.5f, -1.41f, 0.0, 1.0,//e

		-1.41f, 1.5f, -1.41f, 1.0, 1.0,//e
		-0.00f, 1.5f, -2.00f, 0.0, 1.0,//f
		-1.06f, 0.2f, -1.06f, 1.0, 0.0,//E
			
		-1.06f, 0.2f, -1.06f, 1.0, 0.0,//E
		0.0f, 0.2f, -1.50f, 0.0, 0.0,//F
		-0.00f, 1.5f, -2.00f, 0.0, 1.0,//f
		
		-0.00f, 1.5f, -2.00f, 1.0, 1.0,//f
		1.41f, 1.5f, -1.41f, 0.0, 1.0,//g
		0.0f, 0.2f, -1.50f, 1.0, 0.0,//F
			
		0.0f, 0.2f, -1.50f, 1.0, 0.0,//F
		1.06f, 0.2f, -1.06f, 0.0, 0.0,//G
		1.41f, 1.5f, -1.41f, 0.0, 1.0,//g
		
		1.41f, 1.5f, -1.41f, 1.0, 1.0,//g
		2.00f, 1.5f, 0.00f, 0.0, 1.0,//h
		1.06f, 0.2f, -1.06f, 1.0, 0.0,//G
			
		1.06f, 0.2f, -1.06f, 1.0, 0.0,//G
		1.50f, 0.2f, 0.00f, 0.0, 0.0,//H
		2.00f, 1.5f, 0.00f, 0.0, 1.0,//h
		
		2.00f, 1.5f, 0.00f, 1.0, 1.0,//h
		1.41f, 1.5f, 1.41f, 0.0, 1.0,//a
		1.50f, 0.2f, 0.00f, 1.0, 0.0,//H
			
		1.50f, 0.2f, 0.00f, 1.0, 0.0,//H
		1.06f, 0.2f, 1.06f, 0.0, 0.0,//A
		1.41f, 1.5f, 1.41f, 0.0, 1.0,//a
	};

	unsigned int VBO, VAO;
	unsigned int VBO2, VAO2;
	unsigned int VBO3, VAO3;
	unsigned int VBO4, VAO4;
	unsigned int VBO5, VAO5;
	


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	
	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glBindVertexArray(VAO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bowl), bowl, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glBindVertexArray(VAO5);
	glBindBuffer(GL_ARRAY_BUFFER, VBO5);


	// load and create a texture 
	// -------------------------
	unsigned int texHandle, texture2, texTable, texMalletHead, texBuddhaDetail, textureBowl;
	// texture 1
	// ---------
	glGenTextures(1, &texHandle);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("images/natural_wood.jpg", &width, &height, &nrChannels, 0);
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
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("images/gold_wood.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// texture 3
// ---------
	glGenTextures(1, &texTable);
	glBindTexture(GL_TEXTURE_2D, texTable);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data2 = stbi_load("images/table.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	// texture for the mallet's head
// ---------
	glGenTextures(1, &texMalletHead);
	glBindTexture(GL_TEXTURE_2D, texMalletHead);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data3 = stbi_load("images/felt.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data3);

	// texture for the mallet's head
// ---------
	glGenTextures(1, &texBuddhaDetail);
	glBindTexture(GL_TEXTURE_2D, texBuddhaDetail);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data4 = stbi_load("images/buddha.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data4);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data4);
	//// texture of the singing bowl
	//// ---------
	//glGenTextures(1, &textureBowl);
	//glBindTexture(GL_TEXTURE_2D, textureBowl);
	//// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// load image, create texture and generate mipmaps
	//width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	//unsigned char* data5 = stbi_load("images/silver.jpg", &width, &height, &nrChannels, 0);
	//if (data5)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//stbi_image_free(data5);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texHandle", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texTable", 2);
	ourShader.setInt("texMalletHead", 3);
	ourShader.setInt("texBuddhaDetail", 4);
	ourShader.setInt("textureBowl", 5);

	glm::mat4 model;
	float angle;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);
		mouse_callback(window, lastX, lastY);

		// render
		// ------
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units

		//glActiveTexture(GL_texHandle);
		//glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		//Logic here depends on processinput GLFW_KEY_P being pressed and switching the global bool Ortho.
		//If Ortho is false, then 3D perspective will be used. If Ortho is true, then orthographic projection
		//will be used. The four parameters are the left and right bounds and the bottom and top bounds
		//respectively. They could be any number but I've tied them to the screen width and height variables
		//as the instructor has done in order to make them more abstracted and therefor easier to change.
		//It's necessary to declare projection outside of the if statement in order to avoid errors when passing
		//to the shader.

		//Orthographic view works by only rendering that which falls within the viewspace. It's helpful to 
		//think of it as an unhelpful wizard casting invisibility on everything and you can only see things
		//when they fall within range of an anti-magic cube. The orthographic projection is the antimagic cube
		//which renders the invisible visible and foils the wizard's plans. The parameters passed to the
		//orthographic projection essentially determine the dimensions of the cube. The user inputs then
		//determine where the cube moves to render things visible, essentially scanning through the scene 
		//not unlike how radiology programs can scan through the 'slices' of a CT or MRI scan. 
		
		glm::mat4 projection;
		if (!Ortho) {
			projection = glm::perspective(glm::radians(zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		}
		else {
			float scale = 100.0f;
			projection = glm::ortho(-((float)SCREEN_WIDTH/scale), ((float)SCREEN_WIDTH / scale, -5.0f, 5.0f), 
				-((float)SCREEN_HEIGHT / scale), ((float)SCREEN_HEIGHT / scale));
		}
		ourShader.setMat4("projection", projection);

		// second parametere was originally cameraPos + cameraFront
		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);


		//Renders the plane with a wood texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texTable);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, NULL);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		glActiveTexture(GL_TEXTURE0);
		//Renders the singing bowl
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO4);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -0.5f, 7.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		ourShader.setMat4("model", model);
		int bowlsize = sizeof(bowl);
		glDrawArrays(GL_TRIANGLES, 0, bowlsize);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texHandle);
		// renders the handle
		glBindVertexArray(VAO2);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		angle = 90.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C(0.4, 10, 6, true, true, true);
		C.render();

		//renders the head of the mallet
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texMalletHead);

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C2(2, 16, 3, true, true, true);
		C2.render();

		//Renders the mala beads (prayer beads)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texHandle);
		glBindVertexArray(VAO5);
		float offset = 0.4; //creates the offset of the beads from each other
		float malaX = -4.0;
		float malaY = 1.0;
		float malaZ = 3.5;
		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY, malaZ));
		Sphere bead0 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead0.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY + offset, malaZ));
		Sphere bead1 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead1.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY + offset * 2, malaZ));
		Sphere bead2 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead2.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY + offset * 3, malaZ));
		Sphere bead3 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead3.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY + offset * 4, malaZ));
		Sphere bead4 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead4.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY + offset * 4, malaZ + offset + 0.1f));
		Sphere bead5 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead5.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX, malaY + offset * 3, malaZ + offset * 2));
		Sphere bead6 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead6.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 0.7, malaY + offset * 2.5, malaZ + offset * 1.7));
		Sphere bead7 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead7.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 1.5, malaY + offset * 2.9, malaZ + offset * 1.5));
		Sphere bead8 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead8.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset *2, malaY + offset * 3.6, malaZ + offset * 1.0));
		Sphere bead9 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead9.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.6, malaY + offset * 4.0, malaZ + offset * 0.4));
		Sphere bead10 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead10.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.7, malaY + offset * 4.0, malaZ + offset * -0.6));
		Sphere bead11 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead11.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.7, malaY + offset * 3.0, malaZ + offset * -0.6));
		Sphere bead12 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead12.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.7, malaY + offset * 2.0, malaZ + offset * -0.6));
		Sphere bead13 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead13.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.7, malaY + offset, malaZ + offset * -0.6));
		Sphere bead14 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead14.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.7, malaY, malaZ + offset * -0.6));
		Sphere bead15 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead15.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 2.0, malaY - offset, malaZ + offset * -0.6));
		Sphere bead16 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead16.Draw();

		model = glm::mat4(1.0f); //identity matrix
		model = glm::translate(model, glm::vec3(malaX + offset * 0.5 , malaY - offset, malaZ + offset * -0.6));
		Sphere bead17 = Sphere(0.2, 6, 6);
		ourShader.setMat4("model", model);
		bead17.Draw();

		//Renders the Buddha statue
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texBuddhaDetail);
		glBindVertexArray(VAO3);

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
		model = glm::translate(model, glm::vec3(5.0f, 1.0f, -6.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(2.0f, 90.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder BuddhaBody(2.5, 32, 12, true, true, true);
		BuddhaBody.render();

		//model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
		//model = glm::translate(model, glm::vec3(5.0f, 9.0f, -6.0f));
		//ourShader.setMat4("model", model);
		//Sphere BuddhaHead = Sphere(2.0, 12, 12);
		//BuddhaHead.Draw();

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first	
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, -6.0f));
		ourShader.setMat4("model", model);
		static_meshes_3D::Cylinder BuddhaBase(4, 48, 1, true, true, true);
		BuddhaBase.render();



		



		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	float camSpeed = speed * deltaTime;


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += camSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= camSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += camSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= camSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		Ortho = not Ortho;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos = glm::vec3(-12.0f, 1.0f, -1.0f);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//initializes lastx/y if none already exists, as detected by the global bool firstMouse which is set to true
	//Then sets firstMouse to false to keep the if clause from interfering with functionality
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//calculates offsets based on comparing last x and y of cursor to current x and y of cursor
	//then sets the last values to the current values
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	//uses the sensitivity to determine how aggressively the camera should respond to mouse position input
	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	//passes offsets to the values of yaw and pitch, which are global
	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.9f)
		pitch = 89.9f;
	if (pitch < -89.9f)
		pitch = -89.9f;

	//creates a matrix called front and fills it via trigonometry that I honest to goodness cannot remember
	//but I do know that the point is that it returns a value between -1 and 1 to determine which combination
	//x, y, and z directions the camera should point on, which is more sensible than using the values
	//likely to be found in yaw and pitch. 
	//Normalize relates these directions to the rest of the scene.
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// scroll_callback controls speed, not zoom here. += instead of -= is used because to do otherwise would
// violate my sensibilities. 
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	speed += (float)yoffset;
	if (speed < 0.09f)
		speed = 0.09f;
	if (speed > 45.0f)
		speed = 45.0f;
}