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

Cube Ech;
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

bool check_presence(int tblkx, int tblky, int tblkz, float blksize, glm::vec3 position) {
	float x = position[0];
	float y = position[1];
	float z = position[2];
	
	// bloki mają środki na punktach kratowych blksize, zaczynając od 0, 0
	int blkx = round(x/blksize);
	int blky = round(y/blksize);
	int blkz = round(z/blksize);

	return ((tblkx == blkx) & (tblky == blky) & (tblkz == tblkz));
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


int maxlabstage = 10;
void initialize_labirynth(int labsize, int stages, int renderer_type) {
	labsize += 2;

    labirynth = (int *)malloc(sizeof(int) * labsize * labsize * labsize);
    _labsize = labsize;

	std::vector <int> ops;
	for(int i = 0 ;i < labsize-3; i++) {
		ops.push_back(1);
		ops.push_back(2);
		ops.push_back(3);
	}

	random_shuffle(ops.begin(), ops.end());
	glm::ivec3 current = glm::ivec3(1, 1, 1);
	std::vector <glm::ivec3> cpath;
	for(int i = 0 ; i < ops.size(); i++) {
		cpath.push_back(current);
		current[ops[i]-1] ++;
	}
	cpath.push_back(current);

    for(int i = 0 ; i < labsize; i++)
        for(int j = 0 ; j < labsize; j++)
            for(int k = 0 ; k < labsize; k++)
				if (renderer_type == 0) { //game
					if((i == 0 || j == 0 || k == 0) || (i == labsize -1|| j == labsize-1 || k == labsize-1))
						labirynth[labsize*labsize*i + labsize*j + k] = -1;
					else
						labirynth[labsize*labsize*i + labsize*j + k] = rand()%maxlabstage;
				} else if (renderer_type == 1) {
					if((i == 0 || j == 0 || k == 0) || (i == labsize -1|| j == labsize-1 || k == labsize-1))
						labirynth[labsize*labsize*i + labsize*j + k] = labsize*3+6-(i+j+k);
					else
						labirynth[labsize*labsize*i + labsize*j + k] = -1;
				} else if (renderer_type == 2) {
					if((i == 0 || j == 0 || k == 0) || (i == labsize -1|| j == labsize-1 || k == labsize-1))
						labirynth[labsize*labsize*i + labsize*j + k] = labsize*3+6-(i+j+k);
					else
						labirynth[labsize*labsize*i + labsize*j + k] = -1;
				}


	if (renderer_type == 0) {
		for(int i = 0 ; i < cpath.size(); i++) {
			glm::ivec3 v = cpath[i];
			int x = v[0];
			int y = v[1];
			int z = v[2];
			labirynth[labsize*labsize*x + labsize*y + z] = 1000;
		}
	}
	labirynth[0] = maxlabstage*10;
	labirynth[labsize*labsize*labsize-1] = maxlabstage*10;

    //for(int i = 0 ; i < labsize; i++) {
	//	labirynth[labsize*labsize*i + labsize*1 + 1] = -1;
	//}

}

int G(int a, int b, int c, int labsize) {
	return labirynth[labsize*labsize*a + labsize*b + c];
}
// x, y, z nie

float check_collisions(glm::vec3 &position, float blksize, int labstage, int labsize) {
	float x = position[0];
	float y = position[1];
	float z = position[2];
	
	float hitboxrange = 0.25;

	// bloki mają środki na punktach kratowych blksize, zaczynając od 0, 0
	int blkx = round(x/blksize);
	int blky = round(y/blksize);
	int blkz = round(z/blksize);
	printf("px = %lf py = %lf pz = %lf\n", x, y, z);
	printf("blkx = %d blky = %d blkz = %d\n", blkx, blky, blkz);
	
	if(G(blkx, blky, blkz, labsize) < labstage) {
		printf("in block!\n");
	}

	// first degree neighbors, should be 6 (sides directly touching current square.)
	if(G(blkx, blky, blkz+1, labsize) < labstage) { printf("block on +z!\n"); if(z > (blksize + hitboxrange)*blkz) {printf("HARD_STOPa!"); position[2] = (blksize + hitboxrange)*blkz;}}
	if(G(blkx, blky, blkz-1, labsize) < labstage) { printf("block on -z!\n"); if(z < (blksize - hitboxrange)*blkz) {printf("HARD_STOPb!"); position[2] = (blksize - hitboxrange)*blkz;}}	

	if(G(blkx, blky+1, blkz, labsize) < labstage) { printf("block on +y!\n"); if(y > (blksize + hitboxrange)*blky) {printf("HARD_STOPc!"); position[1] = (blksize + hitboxrange)*blky;}}
	if(G(blkx, blky-1, blkz, labsize) < labstage) { printf("block on -y!\n"); if(y < (blksize - hitboxrange)*blky) {printf("HARD_STOPd!"); position[1] = (blksize - hitboxrange)*blky;}}

	if(G(blkx+1, blky, blkz, labsize) < labstage) { printf("block on +x!\n"); if(x > (blksize + hitboxrange)*blkx) {printf("HARD_STOPe!"); position[0] = (blksize + hitboxrange)*blkx;}}
	if(G(blkx-1, blky, blkz, labsize) < labstage) { printf("block on -x!\n"); if(x < (blksize - hitboxrange)*blkx) {printf("HARD_STOPf!"); position[0] = (blksize - hitboxrange)*blkx;}}
	// second degree neighbors, should be 12 (sides touching edge of current square.)


	// third degree neighbors, should be 8 (sides touching corner of current square.)
	return 0.0;
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
int main( void )
{
	int labsize = 8;
	int labstage = 0;
	initialize_labirynth(labsize, 10, 1);

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

	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	glEnable (GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 

	AGLErrors("why the fuck would you fail now?");

	Endpoint.init(10, blkscale);
	AGLErrors("yeeted while initializing endpoint");

	Ech.init(labsize + 2, blkscale);
	Ech.recommit_instance_buffer(0);
	AGLErrors("yeeted while initializing board");
	glfwSetCursorPos(window, 1024/2, 1024/2);

	float size_mod = 1.0;

	float last_commit = glfwGetTime();
	
	int gamebind = 2;

	int gamestate = IN_MENU;
	glm::vec3 CameraPosition = glm::vec3(blkscale, blkscale, blkscale);
	glm::vec3 PerhapsCameraPosition = glm::vec3(blkscale, blkscale, blkscale);
	glm::vec3 up;
	
	
	Ech.recommit_instance_buffer(1);

	int animation_cube_buildstate = 20;

	do{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (gamestate == IN_MENU) {
			if (gamebind == 2)
				handle_controls(CameraPosition, up, false);
			CameraPosition = glm::vec3(initial_q_dist, initial_q_dist, initial_q_dist);
			glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
			glm::vec3 eee = CameraPosition + CameraDirection;
			ViewMatrix = glm::lookAt(CameraPosition, eee, up);	
			glm::mat4 Model      = glm::mat4(1.0f);
			glm::mat4 MVP        = Projection * ViewMatrix * Model; // Remember, matrix multiplication is the other way around

			Ech.draw(0.0, 0.0, 0.0f, MVP);
			AGLErrors("yeeted while processing board draw");

		if (glfwGetKey( window, GLFW_KEY_ENTER ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				gamestate = PLAYING_ENTRY_ANIMATION;
			}
		}
					
		} else if (gamestate == PLAYING_ENTRY_ANIMATION) {
			handle_controls(CameraPosition, up, false);
			initialize_labirynth(labsize, 10, 2);
			CameraPosition = glm::vec3(initial_q_dist, initial_q_dist, initial_q_dist);
			glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
			glm::vec3 eee = CameraPosition + CameraDirection;
			ViewMatrix = glm::lookAt(CameraPosition, eee, up);	
			glm::mat4 Model      = glm::mat4(1.0f);
			glm::mat4 MVP        = Projection * ViewMatrix * Model; // Remember, matrix multiplication is the other way around
			animation_cube_buildstate += 1;
			Ech.recommit_instance_buffer(animation_cube_buildstate);
			Ech.draw(0.0, 0.0, 0.0f, MVP);
			wait(200000);

			if (animation_cube_buildstate == labsize*3+10) {
			// tutaj dopiero jak włączam grę
				gamestate = IN_GAME;
				CameraPosition = glm::vec3(labsize, labsize, labsize);
				free(labirynth);
				initialize_labirynth(labsize, 10, 0);
				Ech.recommit_instance_buffer(0);
			}
		} else if (gamestate == IN_GAME) {

			if (gamebind == 2)
				handle_controls(CameraPosition, up, true);

			check_collisions(CameraPosition, blkscale, labstage, labsize + 2);

			/* game logic, handle moving target shape around, advance level stage.*/

			float coord = 0.0f;
			if(objcorner == 0) {
				coord = blkscale * labsize;
				if(check_presence(labsize, labsize, labsize, blkscale, CameraPosition)) {
					labstage = min(maxlabstage, labstage + 1);
					printf("entering labstage %d\n", labstage);
					Ech.recommit_instance_buffer(labstage);
					objcorner = 1;
				}
			} else {
				coord = blkscale;
				if(check_presence(1, 1, 1, blkscale, CameraPosition)) {
					labstage = min(maxlabstage, labstage + 1);
					printf("entering labstage %d\n", labstage);
					Ech.recommit_instance_buffer(labstage);
					objcorner = 0;
				}
			}


			// recalculate ViewMatrix, and MVP.
			glm::vec3 eee = CameraPosition + CameraDirection;
			ViewMatrix = glm::lookAt(CameraPosition, eee, up);	

			glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
			glm::mat4 Model      = glm::mat4(1.0);
			glm::mat4 MVP = Projection * ViewMatrix * Model;


			AGLErrors("main-loopbegin");

			Ech.draw(0.0, 0.0, 0.0f, MVP);
			AGLErrors("yeeted while processing board draw");

			Endpoint.draw(coord, coord, coord, MVP);
			AGLErrors("yeeted while processing endpoint draw");

			AGLErrors("main-afterdraw");


			/* DEBUG, advance labirynth level */
			if (glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS){
				if (glfwGetTime() > last_commit + 1.0f) {
					last_commit = glfwGetTime();
					labstage = max(0, labstage - 1);
					printf("entering labstage %d\n", labstage);
					Ech.recommit_instance_buffer(labstage);
				}
			}

			/* DEBUG, advance labirynth level */
			if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS){
				if (glfwGetTime() > last_commit + 1.0f) {
					last_commit = glfwGetTime();
					labstage = min(maxlabstage, labstage + 1);
					printf("entering labstage %d\n", labstage);
					Ech.recommit_instance_buffer(labstage);
				}
			}

			if(maxlabstage-1 == labstage) {
				gamebind = 0;
				printf("wygrałes gre");
			}
		}

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

