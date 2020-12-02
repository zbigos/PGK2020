
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"
#include "zadanie4.hpp"

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
        GLuint cubesize_ID;
        GLuint ColorId;

        GLuint programID;
        GLuint MatrixID;
        GLuint PositionID;

        void set_buffers(glm::vec3 p, glm::vec3 l) {
            /*
            glm::vec3 p = vec3(2.0, 1.0, 1.0);
            glm::vec3 l = vec3(-1.0, -1.0, -1.0);
            */

            glm::vec3 lll = vec3(l[0], l[1], l[2]);
            glm::vec3 ppp = vec3(p[0], p[1], p[2]);

            glm::vec3 lpp = vec3(l[0], p[1], p[2]);
            glm::vec3 plp = vec3(p[0], l[1], p[2]);
            glm::vec3 ppl = vec3(p[0], p[1], l[2]);

            glm::vec3 llp = vec3(l[0], l[1], p[2]);
            glm::vec3 lpl = vec3(l[0], p[1], l[2]);
            glm::vec3 pll = vec3(p[0], l[1], l[2]);

            GLfloat g_vertex_buffer_data[] = { 
                lll[0], lll[1], lll[2],
                llp[0], llp[1], llp[2],
                lpp[0], lpp[1], lpp[2], 
                ppl[0], ppl[1], ppl[2],
                lll[0], lll[1], lll[2],
                lpl[0], lpl[1], lpl[2],
                plp[0], plp[1], plp[2],
                lll[0], lll[1], lll[2],
                pll[0], pll[1], pll[2],
                ppl[0], ppl[1], ppl[2],
                pll[0], pll[1], pll[2],
                lll[0], lll[1], lll[2],
                lll[0], lll[1], lll[2],
                lpp[0], lpp[1], lpp[2], 
                lpl[0], lpl[1], lpl[2],
                plp[0], plp[1], plp[2],
                llp[0], llp[1], llp[2],
                lll[0], lll[1], lll[2],
                lpp[0], lpp[1], lpp[2], 
                llp[0], llp[1], llp[2],
                plp[0], plp[1], plp[2],
                ppp[0], ppp[1], ppp[2],
                pll[0], pll[1], pll[2],
                ppl[0], ppl[1], ppl[2],
                pll[0], pll[1], pll[2],
                ppp[0], ppp[1], ppp[2],
                plp[0], plp[1], plp[2],
                ppp[0], ppp[1], ppp[2],
                ppl[0], ppl[1], ppl[2],
                lpl[0], lpl[1], lpl[2],
                ppp[0], ppp[1], ppp[2],
                lpl[0], lpl[1], lpl[2],
                lpp[0], lpp[1], lpp[2], 
                ppp[0], ppp[1], ppp[2],
                lpp[0], lpp[1], lpp[2], 
                plp[0], plp[1], plp[2],
            };

            for(int e = 0; e < 12*3*3; e++) {
                g_vertex_buffer_data[e] *= 2.0f;
            }
        
        AGLErrors("arrayop failed in cube.cpp");

        // 1rst attribute buffer : vertices
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

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
    }

    void load_shaders() {
        programID = LoadShaders( "shaders/borderek.vertexshader", "shaders/borderek.fragmentshader" );
        MatrixID = glGetUniformLocation(programID, "MVP");
        ColorId =  glGetUniformLocation(programID, "color");
    }

    void bindBuffers() {
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    }

    public:
        void init(glm::vec3 p, glm::vec3 l) {
            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);

            glGenBuffers(1, &vertexbuffer);

            AGLErrors("glGenBuffers failed in cube.cpp");

            load_shaders();
            AGLErrors("load_shaders failed in cube.cpp");

            set_buffers(p, l);
            AGLErrors("set_buffers failed in cube.cpp");
        }

        //glm::mat4 Projection, glm::mat4 View, glm::mat4 Model
        void draw(glm::mat4 MVP, glm::vec3 color) {
		    glUseProgram(programID);
            bindBuffers();
            
    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(ColorId, color[0], color[1], color[2]);

            glDrawArrays(GL_TRIANGLES, 0, 12*3);
        }
};