
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"
#include "tutorial4.hpp"

#define PI 3.14159265

/* compiler take the wheel */



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

            // One color for each vertex. They were generated randomly.
            GLfloat g_color_buffer_data[] = { 
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
                g_color_buffer_data[e] *= __cubescale/2.0f;
                g_vertex_buffer_data[e] = 1.0f;
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
    }

    void load_shaders() {
        programID = LoadShaders( "shaders/TransformVertexShaderSphere.vertexshader", "shaders/ColorFragmentShaderSphere.fragmentshader" );
        
        MatrixID = glGetUniformLocation(programID, "MVP");
        PositionID = glGetUniformLocation(programID, "position");
        distance_scaleID = glGetUniformLocation(programID, "distance_scale");

        printf("program %d, %d %d %d\n", programID, MatrixID, PositionID, distance_scaleID);
    }

    void bindBuffers() {
        glBindVertexArray(vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instancebuffer);
    }

    public:
        void init(int cubesize, float cubescale) {
            __cubesize = cubesize;
            __cubescale = cubescale;
            glGenVertexArrays(1, &vertexbuffer);
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
    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(PositionID, x, y, z);
            glUniform1f(distance_scaleID, __cubescale);
            glDrawArrays(GL_TRIANGLES, 0, 12*3);

        }
};