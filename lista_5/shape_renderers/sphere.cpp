
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
    	GLuint normalbuffer;
        GLuint instancebuffer;

        GLuint VertexArrayID;
        GLuint ColorArrayID;
        GLuint InstanceArrayID;

        GLuint distance_scaleID;

        GLuint programID;
        GLuint MatrixID;
        GLuint PositionID;

        GLuint LECID;
        GLuint MID;
        GLuint VID;
        GLuint LPUID;
        GLuint CPID;
        GLuint EMISRCID;
        int triangle_count = 0;
        float aquarium_size = 200.0;
        
        float orbit_distance = 300.0;
        float orbit_agle = 0.0;

        const int instances = 100;
        bool initialized = false;
        GLfloat *instance_buffer_data;
        GLfloat *baked_buffer_data;

        glm::vec3 triangleNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
            glm::vec3 U = p2 - p1;
            glm::vec3 V = p3 - p1;

            glm::vec3 normal;
            normal.x = (U.y*V.z) - (U.z*V.y);
            normal.y = (U.z*V.x) - (U.x*V.z);
            normal.z = (U.x*V.y) - (U.y*V.x);

            return glm::normalize(normal);
        }

        void set_buffers() {
            std::vector<std::vector<std::tuple<GLfloat, GLfloat, GLfloat>>> Point_set;
            std::vector<std::tuple<GLfloat, GLfloat, GLfloat>> points;
            std::vector<glm::vec3> normals;

            /* generate the points on a sphere */
            int resolution = 100;
            Point_set.resize(resolution);
            

            /* for obvious reason we don't use resolution < 2*/
            for(int i = 0; i < resolution; i++) {
                float hangle = (float)i * (180.0 / (float)(resolution - 1)); /* god I hope that this is obvious to me when I have to debug this garbage */
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
                    
                    GLfloat a, b, c;
                    std::tie(a, b, c) = p1;
                    glm::vec3 gp1 = glm::vec3(a, b, c);
                    std::tie(a, b, c) = p2;
                    glm::vec3 gp2 = glm::vec3(a, b, c);
                    std::tie(a, b, c) = p3;
                    glm::vec3 gp3 = glm::vec3(a, b, c);
                    
                    glm::vec3 norm1 = triangleNormal(gp1, gp2, gp3);
                    normals.push_back(norm1);
                    normals.push_back(norm1);
                    normals.push_back(norm1);

                    points.push_back(p1);
                    points.push_back(p2);
                    points.push_back(p3);
                    
                    // do stuffs.
                    /* A B D */
                    p1 = Point_set[i][j+1]; // D
                    p2 = Point_set[i+1][j]; // A
                    p3 = Point_set[i+1][j+1]; // B

                    points.push_back(p3);
                    points.push_back(p2);                    
                    points.push_back(p1);

                    std::tie(a, b, c) = p1;
                    gp1 = glm::vec3(a, b, c);
                    std::tie(a, b, c) = p2;
                    gp2 = glm::vec3(a, b, c);
                    std::tie(a, b, c) = p3;
                    gp3 = glm::vec3(a, b, c);
                    
                    norm1 = triangleNormal(gp3, gp2, gp1);
                    normals.push_back(norm1);
                    normals.push_back(norm1);
                    normals.push_back(norm1);

                }
            }

            size_t datacount = points.size();
            printf("%d points\n", datacount);

            /* dump these objects into a buffer */
            GLfloat g_vertex_buffer_data[datacount * 3];
            GLfloat g_color_buffer_data[datacount * 3];

            for(int j = 0; j < points.size(); j++) {
                g_color_buffer_data[3*j + 0] = (float)(rand()%(int)(10.0*aquarium_size))/10.0;
                g_color_buffer_data[3*j + 1] = (float)(rand()%(int)(10.0*aquarium_size))/10.0;
                g_color_buffer_data[3*j + 2] = (float)(rand()%(int)(10.0*aquarium_size))/10.0;
                
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

        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

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

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			3,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

    }
    
    int counter = 0;
    void commit_instance_buffer(float px, float py, float pz, bool render_player, bool commit_emishn_source) {

        if (!initialized) {
            instance_buffer_data = new GLfloat[instances * 4];
            baked_buffer_data = new GLfloat[instances * 4];

            for(int i = 0; i < instances; i++) {
                instance_buffer_data[i * 4 + 0] = (float)(rand()%(int)(10.0*aquarium_size))/10.0;
                instance_buffer_data[i * 4 + 1] = (float)(rand()%(int)(10.0*aquarium_size))/10.0;
                instance_buffer_data[i * 4 + 2] = (float)(rand()%(int)(10.0*aquarium_size))/10.0;
                instance_buffer_data[i * 4 + 3] = (float)(rand()%100)/100.0;
            }

            initialized = true;
        } else {
            /* przelicz gracza */
            if (render_player) {
                instance_buffer_data[0] = px * 1.0;
                instance_buffer_data[1] = py * 1.0;
                instance_buffer_data[2] = pz * 1.0;
            } else {
                instance_buffer_data[0] = 10000.0f;
                instance_buffer_data[1] = 10000.0f;
                instance_buffer_data[2] = 10000.0f;
            }
            instance_buffer_data[3] += 0.01;
            if(instance_buffer_data[3] > 1.0)
                instance_buffer_data[3] = 0.0;
            
            /* przelicz słońce */
            orbit_agle += 1.0f;
            if (orbit_agle > 360.0f)
                orbit_agle = 0.0f;
            
            float aaax = cos(PI * orbit_agle/180.0) * orbit_distance;
            float aaay = sin(PI * orbit_agle/180.0) * orbit_distance;
            float aaaz = aquarium_size/2.0;
            
            instance_buffer_data[4] = aaax + aaaz;
            instance_buffer_data[5] = aaay + aaaz;
            instance_buffer_data[6] = aaaz;
            instance_buffer_data[7] = 60.0/360.0;


            for(int i = 2; i < instances; i++) {
                //instance_buffer_data[i * 4 + 1] -= 0.0001 * (2*(10.0*aquarium_size)-instance_buffer_data[i * 3 + 1]);

                if (instance_buffer_data[i * 4 + 1] < 0.0) {
                    instance_buffer_data[i * 4 + 1] = (float)aquarium_size;
                    instance_buffer_data[i * 4 + 0] = (float)(rand()%(int)(aquarium_size*10.0))/10.0;
                    instance_buffer_data[i * 4 + 2] = (float)(rand()%(int)(aquarium_size*10.0))/10.0;
                    instance_buffer_data[i * 4 + 3] = (float)(rand()%100)/100.0;
                }
            }
        }

        // Sortowanie timeeeee.
        std::pair<double, int> sorting_lookaside_buffer[instances];
        for(int i = 0; i < instances; i++) {
            sorting_lookaside_buffer[i].second = i;
            double a = instance_buffer_data[0] - instance_buffer_data[4*i + 0];
            double b = instance_buffer_data[1] - instance_buffer_data[4*i + 1];
            double c = instance_buffer_data[2] - instance_buffer_data[4*i + 2];
            double distance = a*a + b*b + c*c;
            sorting_lookaside_buffer[i].first = -distance;
        }

        std::sort(sorting_lookaside_buffer, sorting_lookaside_buffer+instances);

        int lightemitters[10];
        for(int i = 0; i < instances; i++) {
            if (sorting_lookaside_buffer[i].second < 10) {
                lightemitters[sorting_lookaside_buffer[i].second] = i;
            }
        }

        printf("emission of lightemitters... -> ");
        for(int i = 0; i < 10; i++) {
            printf("%d, ", lightemitters[i]);
        }
        printf("\n");

        if (commit_emishn_source)
            glUniform1iv(EMISRCID, 10, &lightemitters[0]);

        for(int i = 0; i < instances; i++) {
            int translation_cell = sorting_lookaside_buffer[i].second;
            for(int j = 0; j <= 3; j+= 1)
                baked_buffer_data[i * 4 + j] = instance_buffer_data[translation_cell * 4 + j];
        }

        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * instances, baked_buffer_data, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size
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
        MID = glGetUniformLocation(programID, "M");
        VID = glGetUniformLocation(programID, "V");
        LPUID = glGetUniformLocation(programID, "LightPosition_worldspace");
        CPID = glGetUniformLocation(programID, "CameraPosition_worldspace");
        LECID = glGetUniformLocation(programID, "LightEmitterColor");
        EMISRCID = glGetUniformLocation(programID, "emission_sources");
        PositionID = glGetUniformLocation(programID, "position");

        printf("program %d, %d %d\n", programID, MatrixID, PositionID);
    }

    void bindBuffers() {
        glBindVertexArray(VertexArrayID);       
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, instancebuffer);
    	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
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
        	glGenBuffers(1, &normalbuffer);

            load_shaders();
            set_buffers();
            commit_instance_buffer(0.0, 0.0, 0.0, true, false);
        }

        void draw(float x, float y, float z, bool render_player, glm::mat4 MVP, glm::mat4 V, glm::mat4 M, glm::vec3 CP) {

		    glUseProgram(programID);
            bindBuffers();

            commit_instance_buffer(x, y, z, render_player, true);
    		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    		glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);
    		glUniformMatrix4fv(VID, 1, GL_FALSE, &V[0][0]);

    		glUniform3f(CPID, CP[0], CP[1], CP[2]);

            GLfloat light_positions[30];
            GLfloat light_hueues[10];
            for(int i = 0 ; i < 10; i++) {
                light_positions[3 * i + 0] = instance_buffer_data[4 * i + 0];
                light_positions[3 * i + 1] = instance_buffer_data[4 * i + 1];
                light_positions[3 * i + 2] = instance_buffer_data[4 * i + 2];
                light_hueues[i] = instance_buffer_data[4 * i + 3];
            }

            // ustalamy sobie, że pierwsze 10 kulek świeci
            glUniform3fv(LPUID, 10, &light_positions[0]);
            glUniform3fv(LPUID, 10, &light_positions[0]);
            glUniform1fv(LECID, 10, &light_hueues[0]);

            AGLErrors("uniform dumps failed in sphere.cpp");

            //glDrawArrays(GL_TRIANGLES, 0, triangle_count);
            glDrawArraysInstanced(GL_TRIANGLES, 0, triangle_count, instances);
        }
};