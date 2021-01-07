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

#include "maputils.cpp"

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

	if (mapview == TOPDOWN) {
		float speed = PerhapsCameraPosition[2] * 0.025f;
		if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
			PerhapsCameraPosition -= vec3(0.0, 1.0, 0.0) * deltaTime * speed;
		}
		// Move backward
		if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
			PerhapsCameraPosition += vec3(0.0, 1.0, 0.0) * deltaTime * speed;
		}
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
			PerhapsCameraPosition += vec3(1.0, 0.0, 0.0) * deltaTime * speed;
		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
			PerhapsCameraPosition -= vec3(1.0, 0.0, 0.0) * deltaTime * speed;
		}
	} else {
		float speed = 0.5f;
		// Move forward
		if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
			PerhapsCameraPosition += vec3(0.0, 1.0, 0.0) * deltaTime * speed;
		}
		// Move backward
		if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
			PerhapsCameraPosition -= vec3(0.0, 1.0, 0.0) * deltaTime * speed;
		}
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
			PerhapsCameraPosition += vec3(1.0, 0.0, 0.0) * deltaTime * speed;
		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
			PerhapsCameraPosition -= vec3(1.0, 0.0, 0.0) * deltaTime * speed;
		}
	}
}

//glm::vec3 CameraPosition = glm::vec3(-180.0, -50.0, -15.0);
glm::vec3 CameraPosition = glm::vec3(10.0, 10.0, 10.0);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	CameraPosition -= vec3(0.0, 0.0, 0.1) * (float)yoffset;
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
	//glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetCursorPos(window, 1024/2, 1024/2);
	

	
	float size_mod = 1.0;

	float last_commit = glfwGetTime();
	
	int gamebind = 2;

	
	std::pair<int, int> shader_result = BuildPipeline();
	GLuint shaderhandle = shader_result.first;
	GLuint bufferlocation = shader_result.second;

	std::vector<Chunk> mapdata;

	std::vector<std::string> maptargets = gettargets();

	for(int i = 0 ; i < maptargets.size(); i += 1) {
		unsigned short int *mapv = readfile_and_downsample(maptargets[i], 1);
		Chunk tmp_patch;
		tmp_patch.init(shaderhandle, bufferlocation, 1, 1, 18, mapv);

		int maptlen = maptargets[i].size();
		int moffx = 180 - ((maptargets[i][maptlen-7] - '0') * 100 + (maptargets[i][maptlen-6] - '0') * 10 + (maptargets[i][maptlen-5] - '0') * 1);
		if (maptargets[i][maptlen-8] == 'E')
			moffx *= -1;
		else
			moffx -= 360;

		int moffy = (maptargets[i][maptlen-10] - '0') * 10 + (maptargets[i][maptlen-9] - '0') * 1;
		if (maptargets[i][maptlen-11] == 'N')
			moffy *= -1;

		tmp_patch.setOoffset(glm::vec3((float)moffx, (float)moffy, 0.0));
		mapdata.push_back(tmp_patch);
	}

	for(int i = 0 ; i < mapdata.size(); i += 1) {
		mapdata[i].PrintMapSignature();
	}

	Chunk background;
	background.init(shaderhandle, bufferlocation, 10, 10, 20, NULL);
	background.setOoffset(glm::vec3(0.0, 0.0, 0.0));

	glm::vec3 PerhapsCameraPosition = glm::vec3(blkscale, blkscale, blkscale);
	glm::vec3 up;
	double last_time = glfwGetTime() - 1.0;
	
	int blkcap = mapdata.size();

	do{

		double time = glfwGetTime();
		double deltaTime = time-last_time;
		last_time = time;
		printf("%lf s per frame, %lf fps ", deltaTime, 1.0/deltaTime);

		/* histerisis control */
		if (1.0/deltaTime < 20.0) {
			printf("too slow, decreasing blkcount %d\n", max(32, (int)((float)blkcap * 0.9)));
			blkcap = max(32, (int)((float)blkcap * 0.9));
		} else if (1.0/deltaTime > 40.0) {
			printf("too fast, increasing blkcount %d\n", min((int)mapdata.size(), (int)((float)blkcap * 1.1)));
			blkcap = min((int)mapdata.size(), (int)((float)blkcap * 1.1));
		} else printf("keeping %d blocks\n", blkcap);
		

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (gamebind == 2)
			handle_controls(CameraPosition, up, true);

		// recalculate ViewMatrix, and MVP.
		glm::vec3 apparent_camera_position;
		if(mapview == SPHERE) {
			float earthradus = CameraPosition[2] + 5.0;
			float hradus = earthradus * cos(3.1415f/180.0 * CameraPosition[1]);
			apparent_camera_position = glm::vec3(
				hradus * cos(3.1415/180.0 * CameraPosition[0]),
				earthradus * sin(3.1415/180.0 * CameraPosition[1]),
				hradus * sin(3.1415/180.0 * CameraPosition[0])
			);
		} else apparent_camera_position = CameraPosition;
		printf("%lf %lf %lf\n", apparent_camera_position[0],  apparent_camera_position[1],  apparent_camera_position[2]);

		glm::vec3 eee = apparent_camera_position + CameraDirection;
		ViewMatrix = glm::lookAt(apparent_camera_position, eee, up);	
		glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.001f);
		glm::mat4 Model      = glm::mat4(1.0);
		glm::mat4 MVP = Projection * ViewMatrix * Model;

		// get distances.
		std::vector<std::pair<double, int>> distmap;
		for(int i = 0; i < mapdata.size(); i += 1) {
			distmap.push_back(std::make_pair(glm::distance(mapdata[i].GetRndrChunkCenter(), apparent_camera_position), i));
		}

		std::sort(distmap.begin(), distmap.end());

		background.draw(apparent_camera_position, Model, Projection, MVP);
		
		for(int i = 0; i < 16; i += 1) {
			mapdata[distmap[i].second].set_teselation(32);
			mapdata[distmap[i].second].draw(apparent_camera_position, Model, Projection, MVP);
		}

		for(int i = 16; i < blkcap; i += 1) {
			mapdata[distmap[i].second].set_teselation(1);
			mapdata[distmap[i].second].draw(apparent_camera_position, Model, Projection, MVP);
		}

		AGLErrors("main-loopbegin");

		glfwSwapBuffers(window);

		if (glfwGetKey( window, GLFW_KEY_V ) == GLFW_PRESS){
			if (glfwGetTime() > last_commit + 1.0f) {
				last_commit = glfwGetTime();
				if (mapview == TOPDOWN) {
					mapview = SPHERE;
				} else {
					mapview = TOPDOWN;
				}
			}
		}

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

