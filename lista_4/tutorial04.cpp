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

#include "tutorial4.hpp"

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

glm::vec3 CameraDirection;
glm::vec3 CameraPosition = vec3(0.0, 0.0, 0.0);
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

void handle_controls() {
	double mousex, mousey;
	glfwGetCursorPos(window, &mousex, &mousey);
	double mouseSpeed = 0.01;
	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 1024/2);
	
	horizontalAngle -= mouseSpeed * float(1024/2 - mousex );
	verticalAngle += mouseSpeed * float( 1024/2 - mousey );
	
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

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);	

	float deltaTime = 1.0;
	float speed = 0.5;
	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		CameraPosition += CameraDirection * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		CameraPosition -= CameraDirection * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		CameraPosition += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		CameraPosition -= right * deltaTime * speed;
	}

	glm::vec3 up = glm::cross(right, CameraDirection );
	glm::vec3 eee = CameraPosition+CameraDirection;
	ViewMatrix = glm::lookAt(CameraPosition, eee, up);	

	printf("%lf %lf %lf\n", CameraPosition[0],  CameraPosition[1],  CameraPosition[2]);
}


int maxlabstage = 5;
void initialize_labirynth(int labsize, int stages) {
    labirynth = (int *)malloc(sizeof(int) * labsize * labsize * labsize);
    _labsize = labsize;

    for(int i = 0 ; i < labsize; i++)
        for(int j = 0 ; j < labsize; j++)
            for(int k = 0 ; k < labsize; k++)
                labirynth[labsize*labsize*i + labsize*j + k] = rand()%maxlabstage;
	
	labirynth[0] = maxlabstage;
	labirynth[labsize*labsize*labsize-1] = maxlabstage;

}

int main( void )
{
	int labsize = 16;
	int labstage = 1;
	initialize_labirynth(16, 10);

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 1024, "chuj", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	glEnable (GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f);
	// Camera matrix
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * ViewMatrix * Model; // Remember, matrix multiplication is the other way around

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	AGLErrors("why the fuck would you fail now?");

	Cube Ech;
	Sphere Endpoint;
	AGLErrors("for some reason failed while initializing classes...");


	Endpoint.init(10, 15.0f);
	AGLErrors("yeeted while initializing endpoint");

	Ech.init(labsize, 2.0f);
	AGLErrors("yeeted while initializing board");

	float counter = 0;
	float size_mod = 1.0;
	float px = 0.0;
	float py = 15.0;

	float last_commit = glfwGetTime();
	
	int gamebind = 2;
	do{
		if (gamebind == 2)
			handle_controls();

		if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				labstage = max(0, labstage - 1);
				printf("entering labstage %d\n", labstage);
				Ech.recommit_instance_buffer(labstage);
			}
		}
		if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				labstage = min(maxlabstage, labstage + 1);
				printf("entering labstage %d\n", labstage);
				Ech.recommit_instance_buffer(labstage);
			}
		}


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	    AGLErrors("main-loopbegin");

		Ech.draw(0.0, 0.0, 0.0f, ViewMatrix);
	    AGLErrors("yeeted while processing board draw");

		Endpoint.draw(0.0, 0.0, 0.0f, ViewMatrix);
	    AGLErrors("yeeted while processing endpoint draw");

		AGLErrors("main-afterdraw");

		glfwSwapBuffers(window);
		glfwPollEvents();
	
		if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				gamebind -= 1;
			}
		}

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

