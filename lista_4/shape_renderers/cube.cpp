
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"
#include "tutorial4.hpp"

#define PI 3.14159265

/* compiler take the wheel */



class Cube {
    private:
        float __cubescale;
        int __cubesize;

        GLuint vertexbuffer;

        GLuint instancebuffer;

        GLuint VertexArrayID;
        GLuint ColorArrayID;
        GLuint InstanceArrayID;

        GLuint distance_scaleID;

        GLuint programID;
        GLuint MatrixID;
        GLuint PositionID;

        void set_buffers() {
            GLfloat g_vertex_buffer_data[] = { 
                -1.0f,-1.0f,-1.0f,
                -1.0f,-1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f,-1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f, 1.0f,-1.0f,
                1.0f,-1.0f, 1.0f,
                -1.0f,-1.0f,-1.0f,
                1.0f,-1.0f,-1.0f,
                1.0f, 1.0f,-1.0f,
                1.0f,-1.0f,-1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f,-1.0f,
                1.0f,-1.0f, 1.0f,
                -1.0f,-1.0f, 1.0f,
                -1.0f,-1.0f,-1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f,-1.0f, 1.0f,
                1.0f,-1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f,-1.0f,-1.0f,
                1.0f, 1.0f,-1.0f,
                1.0f,-1.0f,-1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f,-1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f,-1.0f,
                -1.0f, 1.0f,-1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f,-1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f,-1.0f, 1.0f
            };

            for(int e = 0; e < 12*3*3; e++) {
                g_vertex_buffer_data[e] *= __cubescale/2.0f;
            }

            GLfloat instance_buffer_data[__cubesize*__cubesize*__cubesize][3];
            for (int i = 0; i < __cubesize; i++)
                for (int j = 0; j < __cubesize; j++)
                    for (int k = 0; k < __cubesize; k++) {
                        instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][0] =  i;
                        instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][1] =  j;
                        instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][2] =  k;
                    }
        
        
        AGLErrors("arrayop failed in cube.cpp");

        // 1rst attribute buffer : vertices
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(instance_buffer_data), instance_buffer_data, GL_STATIC_DRAW);


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


        // 3rd attribute buffer : instances
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glVertexAttribDivisor(1, 1); // each cube is 12*3 verts long, so let this number of dudes pass. 
    }

    void load_shaders() {
        programID = LoadShaders( "shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader" );
        
        MatrixID = glGetUniformLocation(programID, "MVP");
        PositionID = glGetUniformLocation(programID, "position");
        distance_scaleID = glGetUniformLocation(programID, "distance_scale");

        printf("program %d, %d %d %d\n", programID, MatrixID, PositionID, distance_scaleID);
    }

    void bindBuffers() {
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
    }

    public:
        void recommit_instance_buffer(int labstage) {
            printf("recommit invoked\n");
            GLfloat instance_buffer_data[__cubesize*__cubesize*__cubesize][3];
            for (int i = 0; i < __cubesize; i++)
                for (int j = 0; j < __cubesize; j++)
                    for (int k = 0; k < __cubesize; k++) {
                        if (labirynth[k + j*__cubesize + i*__cubesize*__cubesize] < labstage) {
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][0] =  i;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][1] =  j;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][2] =  k;
                        } else {
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][0] = 10000.0;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][1] = 10000.0;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][2] = 10000.0;
                        }
                    }


            glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(instance_buffer_data), instance_buffer_data, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            glVertexAttribDivisor(1, 1); // each cube is 12*3 verts long, so let this number of dudes pass. 
        }

        void init(int cubesize, float cubescale) {
            __cubesize = cubesize;
            __cubescale = cubescale;

            /* co do kurwy przepraszam bardzo */
            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);
            printf("VAOid = %d\n", VertexArrayID);

            glGenBuffers(1, &vertexbuffer);
            glGenBuffers(1, &instancebuffer);

            AGLErrors("glGenBuffers failed in cube.cpp");

            load_shaders();
            AGLErrors("load_shaders failed in cube.cpp");

            set_buffers();
            AGLErrors("set_buffers failed in cube.cpp");

            recommit_instance_buffer(1);
            AGLErrors("recommit instance buffer failed in cube.cpp");

        }

        //glm::mat4 Projection, glm::mat4 View, glm::mat4 Model
        void draw(float x, float y, float z, glm::mat4 &View) {
		    glUseProgram(programID);
            bindBuffers();

            glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
            glm::mat4 Model      = glm::mat4(1.0);
            
            glm::mat4 MVP = Projection * View * Model;
    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(PositionID, x, y, z);
            glUniform1f(distance_scaleID, __cubescale);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 12*3, __cubesize * __cubesize * __cubesize);
        }
};