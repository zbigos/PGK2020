
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
            static const GLfloat g_vertex_buffer_data[] = { 
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

            // One color for each vertex. They were generated randomly.
            static GLfloat g_color_buffer_data[] = { 
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

            for(int e = 0; e < 12*3*3; e++)
                g_color_buffer_data[e] *= __cubescale/2.0f;

            GLfloat instance_buffer_data[__cubesize*__cubesize*__cubesize][3];
            for (int i = 0; i < __cubesize; i++)
                for (int j = 0; j < __cubesize; j++)
                    for (int k = 0; k < __cubesize; k++) {
                        instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][0] =  i;
                        instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][1] =  j;
                        instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][2] =  k;
                    }
        

        bindBuffers();

        // 1rst attribute buffer : vertices
        glBindVertexArray(vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        // 2nd attribute buffer : colors
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(instance_buffer_data), instance_buffer_data, GL_STATIC_DRAW);
        // 3rd attribute buffer : instances
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        glVertexAttribDivisor(2, 1); // each cube is 12*3 verts long, so let this number of dudes pass. 
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    void load_shaders() {
        programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
        
        MatrixID = glGetUniformLocation(programID, "MVP");
        PositionID = glGetUniformLocation(programID, "position");
        distance_scaleID = glGetUniformLocation(programID, "distance_scale");

        printf("%d %d %d\n", MatrixID, PositionID, distance_scaleID);
    }

    void bindBuffers() {
        glBindVertexArray(vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instancebuffer);
    }

    public:
        void recommit_instance_buffer() {
            printf("recommit invoked\n");
            GLfloat instance_buffer_data[__cubesize*__cubesize*__cubesize][3];
            for (int i = 0; i < __cubesize; i++)
                for (int j = 0; j < __cubesize; j++)
                    for (int k = 0; k < __cubesize; k++) {
                        if (labirynth[k + j*__cubesize + i*__cubesize*__cubesize]) {
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][0] =  i;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][1] =  j;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][2] =  k;
                        } else {
                            printf("eject\n");
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][0] = 10000.0;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][1] = 10000.0;
                            instance_buffer_data[k + j*__cubesize + i*__cubesize*__cubesize][2] = 10000.0;
                        }
                    }

            glBindBuffer(GL_ARRAY_BUFFER, 0);            
            glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(instance_buffer_data), instance_buffer_data, GL_STATIC_DRAW);
            // 3rd attribute buffer : instances
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(
                2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            glVertexAttribDivisor(2, 1); // each cube is 12*3 verts long, so let this number of dudes pass. 
            glBindBuffer(GL_ARRAY_BUFFER, 0);            
        }

        void init(int cubesize, float cubescale) {
            __cubesize = cubesize;
            __cubescale = cubescale;
            glGenVertexArrays(1, &vertexbuffer);
            glGenBuffers(1, &colorbuffer);
            glGenBuffers(1, &instancebuffer);

            load_shaders();
            set_buffers();
            
            recommit_instance_buffer();
        }

        //glm::mat4 Projection, glm::mat4 View, glm::mat4 Model
        void draw(float x, float y, float z, glm::mat4 &View) {
		    glUseProgram(programID);


            glm::mat4 Projection = glm::infinitePerspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f);
            glm::mat4 Model      = glm::mat4(1.0);
            
            glm::mat4 MVP = Projection * View * Model;
    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(PositionID, x, y, z);
            glUniform1f(distance_scaleID, __cubescale);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 12*3, __cubesize * __cubesize * __cubesize);
        }
};