
#include <stdlib.h>
#include <stdio.h>

#include <tuple>
#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"
#include "zadanie4.hpp"

#define PI 3.14159265

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

        int triangle_count = 0;
        int instances = 100;

        void set_buffers() {
            std::vector<std::vector<std::tuple<GLfloat, GLfloat, GLfloat>>> Point_set;
            std::vector<std::tuple<GLfloat, GLfloat, GLfloat>> points;

            /* generate the points on a sphere */
            int resolution = 20;
            Point_set.resize(resolution);
            

            /* for obvious reason we don't use resolution < 2*/
            for(int i = 0; i < resolution; i++) {
                float hangle = (float)i * (180.0 / (float)resolution); /* god I hope that this is obvious to me when I have to debug this garbage */
                GLfloat z = cos(PI * hangle/180.0);
                GLfloat scale = sin(PI * hangle/180.0);

                for(int j = 0; j < resolution + 2; j++) {
                    float angle = j * (360.0/resolution);
                    GLfloat x = sin(PI * angle/180.0) * scale;
                    GLfloat y = cos(PI * angle/180.0) * scale;
                    
                    Point_set[i].push_back(std::make_tuple((GLfloat)x, (GLfloat)y, (GLfloat)z));
                }
            }

            /* once we have the poinset we will generate the points coorddump */
            for(int i = 0; i < resolution - 1; i++) {
                for(int j = 0; j < resolution + 1; j++) { /* oh lord please save us from segmentation faults*/
                    // A ......... B (< i+1)
                    //
                    //
                    //
                    // C ......... D (< i)

                    /* A C D triangle*/
                    std::tuple<GLfloat, GLfloat, GLfloat> p1; 
                    std::tuple<GLfloat, GLfloat, GLfloat> p2;
                    std::tuple<GLfloat, GLfloat, GLfloat> p3;
                    
                    p1 = Point_set[i][j];  // C
                    p2 = Point_set[i][j + 1]; // D
                    p3 = Point_set[i+1][j]; // A
                    
                    printf("");

                    points.push_back(p1);
                    points.push_back(p2);
                    points.push_back(p3);
                    
                    // do stuffs.
                    /* A B D */
                    p1 = Point_set[i][j+1]; // D
                    p2 = Point_set[i+1][j]; // A
                    p3 = Point_set[i+1][j+1]; // B
                    points.push_back(p1);
                    points.push_back(p2);
                    points.push_back(p3);
                    
                }
            }

            size_t datacount = 3*points.size();
            printf("%d points\n", datacount);

            /* dump these objects into a buffer */
            GLfloat g_vertex_buffer_data[datacount];
            GLfloat g_color_buffer_data[datacount];

            for(int j = 0; j < points.size(); j++) {
                g_color_buffer_data[3*j + 0] = (GLfloat)(rand()%1000)/1000.0;
                g_color_buffer_data[3*j + 1] = (GLfloat)(rand()%1000)/1000.0;
                g_color_buffer_data[3*j + 2] = (GLfloat)(rand()%1000)/1000.0;
                
                GLfloat a, b, c;
                std::tie(a, b, c) = points[j];
                g_vertex_buffer_data[3*j + 0] = a;
                g_vertex_buffer_data[3*j + 1] = b;
                g_vertex_buffer_data[3*j + 2] = c;
            }

        triangle_count = points.size();

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
    
    void commit_instance_buffer() {
            GLfloat instance_buffer_data[instances][3];

            for(int i = 0; i < instances; i++) {
                float a = (float)(rand()%500)/10.0;
                instance_buffer_data[i][0] = a;
                a = (float)(rand()%500)/10.0;
                instance_buffer_data[i][1] = a;
                a = (float)(rand()%500)/10.0;
                instance_buffer_data[i][2] = a;
            }

            glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(instance_buffer_data), instance_buffer_data, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(
                2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
            );

            glVertexAttribDivisor(2, 1); 
    }

    void load_shaders() {
        programID = LoadShaders( "shaders/TransformVertexShader.vertexshader", "shaders/ColorFragmentShader.fragmentshader" );
        
        MatrixID = glGetUniformLocation(programID, "MVP");
        PositionID = glGetUniformLocation(programID, "position");

        printf("program %d, %d %d\n", programID, MatrixID, PositionID);
    }

    void bindBuffers() {
        glBindVertexArray(VertexArrayID);       
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
    }

    public:
        void init(int cubesize, float cubescale) {
            __cubesize = cubesize;
            __cubescale = cubescale;

            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);
            printf("VAOid = %d\n", VertexArrayID);

            glGenBuffers(1, &vertexbuffer);
            glGenBuffers(1, &colorbuffer);
            glGenBuffers(1, &instancebuffer);

            load_shaders();
            set_buffers();
            commit_instance_buffer();
        }

        void draw(float x, float y, float z, glm::mat4 MVP) {
		    glUseProgram(programID);
            bindBuffers();

    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniform3f(PositionID, x, y, z);
            AGLErrors("uniform dumps failed in sphere.cpp");

            //glDrawArrays(GL_TRIANGLES, 0, triangle_count);
            glDrawArraysInstanced(GL_TRIANGLES, 0, triangle_count, instances);

        }
};