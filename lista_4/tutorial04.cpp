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

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

glm::vec3 CameraDirection;
glm::vec3 CameraPosition;
glm::mat4 ViewMatrix;

void handle_controls() {
	double mousex, mousey;
	glfwGetCursorPos(window, &mousex, &mousey);
	double mouseSpeed = 0.01;
	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 1024/2);
	
	horizontalAngle -= mouseSpeed * float(1024/2 - mousex );
	verticalAngle += mouseSpeed * float( 1024/2 - mousey );

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
}


int* labirynth;
int _labsize;

void initialize_labirynth(int labsize, int stages) {
    labirynth = (int *)malloc(sizeof(int) * labsize * labsize * labsize);
    _labsize = labsize;

    for(int i = 0 ; i < labsize; i++)
        for(int j = 0 ; j < labsize; j++)
            for(int k = 0 ; k < labsize; k++)
                labirynth[labsize*labsize*i + labsize*j + k] = rand()%stages;
}

int main( void )
{
	int labsize = 16;
	int labstage = 0;
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

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * ViewMatrix * Model; // Remember, matrix multiplication is the other way around

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	AGLErrors("uuuu");

	Cube Ech;
	Ech.init(labsize, 15.0f);
	float counter = 0;
	float size_mod = 1.0;
	float px = 0.0;
	float py = 15.0;

	float last_commit = glfwGetTime();

	do{
		handle_controls();

		if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				labstage = max(0, labstage - 1);
				printf("entering labstage %d\n", labstage);
				Ech.recommit_instance_buffer();
			}
		}
		if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				labstage = min(9, labstage + 1);
				printf("entering labstage %d\n", labstage);
				Ech.recommit_instance_buffer();
			}
		}


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	    AGLErrors("main-loopbegin");

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform

		// Draw the triangle !

		Ech.draw(px, py, 0.0f, ViewMatrix);
		//Ech.draw(0.0, 0.0, 0.0f, View);

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);

		// Swap buffers
		AGLErrors("main-afterdraw");

		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

