#include <iostream>     // std::cout
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <epoxy/gl.h>
#include <epoxy/glx.h>


#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <AGL3Drawable.hpp>

#include "shape_renderers/chunk.cpp"

#include "zadanie4.hpp"
#include "shader_pipeline_builder.cpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

void wait( int useconds )
{   // Pretty crossplatform, both ALL POSIX compliant systems AND Windows
    #ifdef _WIN32
        Sleep( 1000 * seconds );
    #else
        usleep( useconds );
    #endif
}

float horizontalAngle = 29.06f; // 4.0
float verticalAngle = 2.53f;

float blkscale = 25.0;
int objcorner = 0;

glm::vec3 CameraDirection;
glm::mat4 ViewMatrix;

void MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s \n type = 0x%x, \nseverity = 0x%x, \nmessage = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );

if (type == GL_DEBUG_TYPE_ERROR) {
	int *a = NULL; // halt gdb. or entire system if kernel is not fast enough;
	int payload = 2137;
	*a = payload;
}
}

void handle_controls(glm::vec3 &PerhapsCameraPosition, glm::vec3 &up, bool process) {
	double mousex, mousey;
	glfwGetCursorPos(window, &mousex, &mousey);
	double mouseSpeed = 0.01;
	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 1024/2);
	if (process) {
		horizontalAngle -= mouseSpeed * float(1024/2 - mousex );
		verticalAngle += mouseSpeed * float( 1024/2 - mousey );
	}

	if (verticalAngle < 0.5 * 3.14)
		verticalAngle = 0.5 * 3.14;
	if (verticalAngle > 1.5 * 3.14)
		verticalAngle = 1.5 * 3.14;


	printf("horizontal %lf vertical %lf\n", horizontalAngle, verticalAngle);
	CameraDirection = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f/2.0f), 0, cos(horizontalAngle - 3.14f/2.0f));	
	up = glm::cross(right, CameraDirection );

	float deltaTime = 1.0;
	float speed = 0.05;
	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		PerhapsCameraPosition += CameraDirection * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		PerhapsCameraPosition -= CameraDirection * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		PerhapsCameraPosition += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		PerhapsCameraPosition -= right * deltaTime * speed;
	}

	printf("%lf %lf %lf\n", PerhapsCameraPosition[0],  PerhapsCameraPosition[1],  PerhapsCameraPosition[2]);
}


int main( void )
{
	int labsize = 8;
	int labstage = 0;

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 1024, "przegrałeś w gre", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	glEnable (GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	AGLErrors("why the fuck would you fail now?");

	glfwSetCursorPos(window, 1024/2, 1024/2);
	
	std::pair<int, int> shader_result = BuildPipeline();
	GLuint shaderhandle = shader_result.first;
	GLuint bufferlocation = shader_result.second;
	
	float size_mod = 1.0;

	float last_commit = glfwGetTime();
	
	int gamebind = 2;

	Chunk Patch;
	Patch.init(shaderhandle, bufferlocation, 500, 500);
	glm::vec3 CameraPosition = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 PerhapsCameraPosition = glm::vec3(blkscale, blkscale, blkscale);
	glm::vec3 up;
	
	do{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		handle_controls(CameraPosition, up, true);

		/* game logic, handle moving target shape around, advance level stage.*/

		// recalculate ViewMatrix, and MVP.
		glm::vec3 eee = CameraPosition + CameraDirection;
		ViewMatrix = glm::lookAt(CameraPosition, eee, up);	
		glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
		glm::mat4 Model      = glm::mat4(1.0);
		glm::mat4 MVP = Projection * ViewMatrix * Model;

		Patch.draw(CameraPosition, Model, Projection, MVP);
		AGLErrors("main-loopbegin");

		glfwSwapBuffers(window);

		/* unbind cursor from window */
		if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				gamebind -= 1;
			}
		}

		/* rebind cursor to window */
		if (glfwGetKey( window, GLFW_KEY_TAB ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				gamebind = 2;
			}
		}
		
	} while( gamebind > 0 &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

