// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"  //create and compile shader program
#include "Camera.h" //Used to create camera, accept view/projection matrix changes based on input
#include "Model.h" //Load model, vertices and its texture.
#include "Background.h"//Load background pics
#include "Boids.h" //create boids and obstacles

//The main work for this project is in this header file
//Please define input (position, rotation etc...) in this header file
#include "MatrixTransformation.h"  



//SOIL library is used to import images.
#include <SOIL.h>

// Properties
GLuint screenWidth = 800, screenHeight = 600;


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
GLFWwindow* GetEverythingInitialized();

// Camera
Camera camera(glm::vec3(0.0f, 50.0f, 100.0f));
bool keys[1024] = { false };
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
//GLfloat deltaDetect = 0.01f;
//GLfloat lastDetect = 0.0f;
bool gameStart = false;
bool initialized = false;
bool animationDone = false;
const int NUM_BOIDS = 40;
const int NUM_OBSTACLES = 2;





// The MAIN function, from here we start our application and run our Game loop

int main()
{
	// Init GLFW
	GLFWwindow* window = GetEverythingInitialized();

	// Setup and compile all the shaders
	Shader backgroundShader("bgVertexShader.txt", "bgFragmentShader.txt");

	//Create boids;
	Boid boids[NUM_BOIDS];
	GLfloat distanceBetween = 15.0f;
	int size = glm::ceil(std::cbrt(NUM_BOIDS)),numberDone=0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				boids[numberDone].initialization(glm::vec3(i, j, k)*distanceBetween+ glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3((rand()%10+1)*0.1f, 0.0f, 0.0f));
				numberDone++;
				if (numberDone >= NUM_BOIDS) break;
			}if (numberDone >= NUM_BOIDS) break;
		}if (numberDone >= NUM_BOIDS) break;
		
	}


	//Create floor
	Floor floor = Floor();

	//Create destination object
	Destination destination = Destination();

	//Create obstacles
	Obstacle obstacles[] = {
		Obstacle(glm::vec3(100.0f,0.0f,0.0f)),
		Obstacle(glm::vec3(-100.0f,0.0f,0.0f)),
	};

	//Background pics
	Background background("background.jpg", 0);

	//press B and then animation will start
	std::cout << "Press B key to Begin!" << std::endl<<std::endl;
	std::cout << "Press LEFT, RIGHT, UP, DOWN, J and K key to navigate the destination ball" << std::endl;

	// Game loop
	do {
		// Set frame time for camera control
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check and call events for camera control
		glfwPollEvents();
		Do_Movement();

		// Clear the colorbuffer and build background
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Calculate camera projection and view matrix based on key and mouse input, combine them together get cameraMatrix
		Camera::cameraMatrix = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f)*camera.GetViewMatrix();
		//Build background as visible coordinates
		backgroundShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(backgroundShader.Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		background.Draw(backgroundShader);

		//Now we start draw objects.

		//Check if game is still going on.

		if (!animationDone&&gameStart) {
			if (!initialized) {
				initialized = true;
				for (int i = 0; i < NUM_BOIDS; i++) {
					boids[i].startUpdating();
				}
			}
			//calculate dynamics
			for (int i = 0; i < NUM_BOIDS; i++) {
				//detect ground code goes here
				//detect cylinder obstacle code goes here
				//detect dynamics with other boids goes here
				boids[i].updateDestination(&destination);
				boids[i].updateFloor();
				for (int j = 0; j < NUM_OBSTACLES; j++) {
					boids[i].updateObstacles(&obstacles[j]);
				}
				for (int j = i + 1; j < NUM_BOIDS; j++) {
					boids[i].updateDynamicBoid(&boids[j]);
				}
				boids[i].updateMotion();
			}

		}
		//draw main object
		destination.Draw();
		floor.Draw();
		for (int i = 0; i < NUM_BOIDS; i++) {
			boids[i].Draw();
		}
		for (int i = 0; i < NUM_OBSTACLES; i++) {
			obstacles[i].Draw();
		}
		// Swap the buffers
		glfwSwapBuffers(window);

		//Until user press escape or close the window, the program should loop.
	} while (!glfwWindowShouldClose(window));

	//remove the resources used to free up memory


	glfwTerminate();
	return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (gameStart) {
		if (keys[GLFW_KEY_LEFT])
			Destination::currentPosition -= glm::vec3(deltaTime*Destination::velocity, 0.0f, 0.0f);
		if (keys[GLFW_KEY_RIGHT])
			Destination::currentPosition += glm::vec3(deltaTime*Destination::velocity, 0.0f, 0.0f);
		if (keys[GLFW_KEY_UP])
			Destination::currentPosition -= glm::vec3(0.0f, 0.0f, deltaTime*Destination::velocity);
		if (keys[GLFW_KEY_DOWN])
			Destination::currentPosition += glm::vec3(0.0f, 0.0f, deltaTime*Destination::velocity);
		if (keys[GLFW_KEY_J])
			Destination::currentPosition += glm::vec3(0.0f, deltaTime*Destination::velocity,0.0f);
		if (keys[GLFW_KEY_K])
			Destination::currentPosition -= glm::vec3(0.0f, deltaTime*Destination::velocity, 0.0f);
	}
	
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	//checking before game start
	if (gameStart == false) {
		//If B pressed, game started.
		if (key == GLFW_KEY_B) {
			gameStart = true;
		}
	}
	//update key pressing
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}
//update mouse input info to move camera (changing view and projection matrix)
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
//receive scroll information to accept zoom in and zoom out.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

GLFWwindow* GetEverythingInitialized() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// Options, disable mouse cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();
	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);
	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_PROGRAM_POINT_SIZE);
	return window;
}

#pragma endregion