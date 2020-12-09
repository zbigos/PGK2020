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

#include <shader.hpp>
#include <AGL3Drawable.hpp>

#include "shape_renderers/cube.cpp"
#include "shape_renderers/sphere.cpp"

#include "zadanie4.hpp"


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

float blkscale = 15.0;
int objcorner = 0;

Sphere Endpoint;

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
	float speed = 0.5;
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

void Resize( int _wd, int _ht ) {
   glViewport(0, 0, (GLsizei) _wd, (GLsizei) _ht);
}

void CallbackResize(GLFWwindow* window, int cx, int cy) {
   printf("pls don't touch the window\n");
   void *ptr = glfwGetWindowUserPointer( window );
   if (AGLWindow *winPtr = static_cast<AGLWindow*>( ptr ) )
      winPtr->Resize( cx, cy );
}


float initial_q_dist = -200.0f;

Cube Frame1, Frame2, Frame3;
Cube Frame1a, Frame2a, Frame3a, Frame4a, Frame5a, Frame6a;
Cube Frame1b, Frame2b, Frame3b;

Cube Floor;


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
	glfwSetWindowSizeCallback(window, CallbackResize);


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	glEnable (GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	AGLErrors("why the fuck would you fail now?");
	float thickness = 0.5f;
	float wallsize = 100.0f;
	glm::vec3 origin = vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 To = vec3(thickness, thickness, thickness);
	glm::vec3 Too = vec3(thickness*2, thickness*2, thickness*2);

	glm::vec3 T1 = vec3(wallsize, thickness, thickness);
	glm::vec3 T2 = vec3(thickness, wallsize, thickness);
	glm::vec3 T3 = vec3(thickness, thickness, wallsize);
	
	Frame1.init(origin, T1);
	Frame2.init(origin, T2);
	Frame3.init(origin, T3);

	Frame1a.init(T1 - To, T1 + T3 - To);
	Frame2a.init(T1 - To, T1 + T2 - To);

	Frame3a.init(T2 - To, T2 + T3 - To);
	Frame4a.init(T2 - To, T2 + T1 - To);

	Frame5a.init(T3 - To, T3 + T1 - To);
	Frame6a.init(T3 - To, T3 + T2 - To);

	Frame1b.init(T1 + T2 - Too, T1 + T2 + T3 - Too);
	Frame2b.init(T1 + T3 - Too, T1 + T2 + T3 - Too);
	Frame3b.init(T2 + T3 - Too, T1 + T2 + T3 - Too);

	Floor.init(T2, T1 + T2 + T3 - Too - To);

	Endpoint.init(10, blkscale);
	
	AGLErrors("yeeted while initializing endpoint");

	AGLErrors("yeeted while initializing board");
	glfwSetCursorPos(window, 1024/2, 1024/2);

	float size_mod = 1.0;

	float last_commit = glfwGetTime();
	
	int gamebind = 1;

	int gamestate = IN_MENU;
	glm::vec3 CameraPosition = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 up;

	bool render_player = false;

	glm::mat4 MVP;
	glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
	glm::mat4 Model      = glm::mat4(1.0);
	
	do{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			
		/* game logic, handle moving target shape around, advance level stage.*/
		if (gamebind == 0) {
			printf("gamebind == 1, first person view");
			handle_controls(CameraPosition, up, true);
			render_player = false;

			glm::vec3 eee = CameraPosition + CameraDirection;
			ViewMatrix = glm::lookAt(CameraPosition, eee, up);	
			MVP = Projection * ViewMatrix * Model;
		}

		if (gamebind == 1) {
			printf("gamebind == 2, third person view\n");
			handle_controls(CameraPosition, up, true);
			render_player = true;

			float s = 10.0;
			glm::vec3 scaled_camdir = vec3(CameraDirection[0] * s, CameraDirection[1] * s, CameraDirection[2] * s);
			ViewMatrix = glm::lookAt(CameraPosition - scaled_camdir, CameraPosition, up);	
			MVP = Projection * ViewMatrix * Model;
		}

		if (gamebind == 2) {
			printf("gamebind == 3, full aquarium view\n");
			render_player = true;

			float s = 10.0;
			glm::vec3 observer_position = vec3(-3*wallsize, wallsize, wallsize);
			glm::vec3 camera_target = vec3(1.0, 0.0, 0.0);

			ViewMatrix = glm::lookAt(observer_position, observer_position + camera_target, up);	
			MVP = Projection * ViewMatrix * Model;
		}


		// recalculate ViewMatrix, and MVP.

		AGLErrors("main-loopbegin");
		//
		Endpoint.draw(CameraPosition[0], CameraPosition[1], CameraPosition[2], render_player, MVP, ViewMatrix, Model, CameraPosition);

		Frame1.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame2.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame3.draw(MVP, vec3(0.0, 0.0, 0.0));

		Frame1a.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame2a.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame3a.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame4a.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame5a.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame6a.draw(MVP, vec3(0.0, 0.0, 0.0));

		Frame1b.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame2b.draw(MVP, vec3(0.0, 0.0, 0.0));
		Frame3b.draw(MVP, vec3(0.0, 0.0, 0.0));

		Floor.draw(MVP, vec3(0.298, 0.274, 0.196));

		AGLErrors("yeeted while processing endpoint draw");

		AGLErrors("main-afterdraw");

		glfwSwapBuffers(window);

		/* unbind cursor from window */
		if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				gamebind = -1;
			}
		}

		/* rebind cursor to window */
		if (glfwGetKey( window, GLFW_KEY_TAB ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				gamebind = (gamebind + 1) % 3;
			}
		}
		
	} while( gamebind >= 0 &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

