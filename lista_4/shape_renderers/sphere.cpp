
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"
#include "tutorial4.hpp"

#define PI 3.14159265

/* compiler take the wheel */
#define V0 -0.26286500f, 0.0000000f, 0.42532500f
#define V1 0.26286500f, 0.0000000f, 0.42532500f
#define V2 -0.26286500f, 0.0000000f, -0.42532500f
#define V3 0.26286500f, 0.0000000f, -0.42532500f
#define V4 0.0000000f, 0.42532500f, 0.26286500f
#define V5 0.0000000f, 0.42532500f, -0.26286500f
#define V6 0.0000000f, -0.42532500f, 0.26286500f
#define V7 0.0000000f, -0.42532500f, -0.26286500f
#define V8 0.42532500f, 0.26286500f, 0.0000000f
#define V9 -0.42532500f, 0.26286500f, 0.0000000f
#define V10 0.42532500f, -0.26286500f, 0.0000000f
#define V11 -0.42532500f, -0.26286500f, 0.0000000f

#define tridef(V1, V2, V3) V1, V2, V3,

class Sphere {
    private:
        float __cubescale;
        int __cubesize;

        GLuint vertexbuffer;
        GLuint colorbuffer;

        GLuint instancebuffer;

        GLuint VertexArrayID;
        GLuint ColorArrayID;
        GLuint InstanceArrayID;

        GLuint distance_scaleID;

        GLuint programID;
        GLuint MatrixID;
        GLuint PositionID;

        void set_buffers() {
            GLfloat g_color_buffer_data[20*3*3];

            // One color for each vertex. They were generated randomly.
            static GLfloat g_vertex_buffer_data[] = { 
                tridef(V0, V6, V1)
                tridef(V0, V11, V6)
                tridef(V1, V4, V0)
                tridef(V1, V8, V4)
                tridef(V1, V10, V8)
                tridef(V2, V5, V3)
                tridef(V2, V9, V5)
                tridef(V2, V11, V9)
                tridef(V3, V7, V2)
                tridef(V3, V10, V7)
                tridef(V4, V8, V5)
                tridef(V4, V9, V0)
                tridef(V5, V8, V3)
                tridef(V5, V9, V4)
                tridef(V6, V10, V1)
                tridef(V6, V11, V7)
                tridef(V7, V10, V6)
                tridef(V7, V11, V2)
                tridef(V8, V10, V3)
                tridef(V9, V11, V0)
            };

            for(int e = 0; e < 20*3*3; e++) {
                g_vertex_buffer_data[e] *= __cubescale/2.0f;
                g_color_buffer_data[e] = (float)(rand()%100)/100.0;
            }

 
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);


        AGLErrors("stuff failed in sphere.cpp");

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);        
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );


        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);        
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
        // 1rst attribute buffer : vertices

    	AGLErrors("tu sie nie siedzi, tu sie inicjalizuje bufory");
    }

    void load_shaders() {
        programID = LoadShaders( "shaders/TransformVertexShaderSphere.vertexshader", "shaders/ColorFragmentShaderSphere.fragmentshader" );
        
        MatrixID = glGetUniformLocation(programID, "MVP");
        PositionID = glGetUniformLocation(programID, "position");

        printf("program %d, %d %d\n", programID, MatrixID, PositionID);
    }

    void bindBuffers() {
        glBindVertexArray(VertexArrayID);       
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    }

    public:
        void init(int cubesize, float cubescale) {
            __cubesize = cubesize;
            __cubescale = cubescale;

            /* co do kurwy przepraszam bardzo */
            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);
            printf("VAOid = %d\n", VertexArrayID);


            glGenBuffers(1, &vertexbuffer);
            glGenBuffers(1, &colorbuffer);
            

            load_shaders();
            set_buffers();
        }

        //glm::mat4 Projection, glm::mat4 View, glm::mat4 Model
        void draw(float x, float y, float z, glm::mat4 &View) {
		    glUseProgram(programID);
            bindBuffers();

            glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
            glm::mat4 Model      = glm::mat4(1.0);
            
            glm::mat4 MVP = Projection * View * Model;
            AGLErrors("MVP calc failed in sphere.cpp");

    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(PositionID, x, y, z);
            AGLErrors("uniform dumps failed in sphere.cpp");

            glDrawArrays(GL_TRIANGLES, 0, 20*3);

        }
};