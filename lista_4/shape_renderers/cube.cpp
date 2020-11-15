
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"

#define PI 3.14159265

/* compiler take the wheel */



class Cube {
    private:
        GLuint vertexbuffer;
        GLuint colorbuffer;

        GLuint instancebuffer;

        GLuint VertexArrayID;
        GLuint ColorArrayID;
        GLuint InstanceArrayID;


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
                g_color_buffer_data[e] *= 0.5f;

            GLfloat instance_buffer_data[] = {
                0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0,
                0.0, 1.0, 1.0,
                1.0, 0.0, 1.0,
                1.0, 1.0, 0.0,
                1.0, 1.0, 1.0,
            };
        

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
        printf("%d %d\n", MatrixID, PositionID);
    }

    void bindBuffers() {
        glBindVertexArray(vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instancebuffer);
    }

    public:
        void init() {
            glGenVertexArrays(1, &vertexbuffer);
            glGenBuffers(1, &colorbuffer);
            glGenBuffers(1, &instancebuffer);

            load_shaders();
            set_buffers();
        }

        //glm::mat4 Projection, glm::mat4 View, glm::mat4 Model
        void draw(float x, float y, float z, glm::mat4 &View) {
            AGLErrors("arrayinit2");

		    glUseProgram(programID);
            //bindBuffers();
            AGLErrors("binding failed");

            glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
            // Camera matrix

            glm::mat4 Model      = glm::mat4(1.0);
            
            glm::mat4 MVP = Projection * View * Model;
    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(PositionID, x, y, z);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 12*3, 8);
            //glDrawArrays(GL_TRIANGLES, 0, 12*3);
        }
};