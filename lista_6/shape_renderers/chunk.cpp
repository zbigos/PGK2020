
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "../AGL3Window.hpp"
#include "../AGL3Drawable.hpp"
#include "zadanie4.hpp"

#define PI 3.14159265

/* compiler take the wheel */



class Chunk {
    private:    
        GLuint ShaderHandle_;
        
        /* buffers */
        GLuint BufferLocation_;
        
        GLuint vao;
        GLuint positions;
        GLuint indices;

        /* tesselation data */
        GLuint tesselation_outer_;
        GLuint tesselation_inner_;
        
        /* uniform locations */
        GLuint UProjectionMatrix_;
        GLuint UModelViewMatrix_;
        GLuint UNormalMatrix_;
        GLuint ULightPosition_;
        GLuint UAmbientMaterial_;
        GLuint UDiffuseMaterial_;
        GLuint UTesselationInnerLevel_;
        GLuint UTesselationOuterLevel_;
        GLuint UCameraPosition_;
        GLuint UMVP_;
        GLuint IndexCount;

        void succ_uniform_ids() {
            UProjectionMatrix_ = glGetUniformLocation(ShaderHandle_, "Projection");
            UModelViewMatrix_ = glGetUniformLocation(ShaderHandle_, "Modelview");
            UNormalMatrix_ = glGetUniformLocation(ShaderHandle_, "NormalMatrix");
            ULightPosition_ = glGetUniformLocation(ShaderHandle_, "LightPosition");
            UAmbientMaterial_ = glGetUniformLocation(ShaderHandle_, "AmbientMaterial");
            UDiffuseMaterial_ = glGetUniformLocation(ShaderHandle_, "DiffuseMaterial");
            UTesselationInnerLevel_ = glGetUniformLocation(ShaderHandle_, "TessLevelInner");
            UTesselationOuterLevel_ = glGetUniformLocation(ShaderHandle_, "TessLevelOuter");
            UCameraPosition_ = glGetUniformLocation(ShaderHandle_, "CameraPosition");
            UMVP_ = glGetUniformLocation(ShaderHandle_, "MVP");
        }

        void set_buffers() {
            const int Faces[] = {
                0, 1, 2, 3
            };

            const float Verts[] = {
                -1.0, -1.0, 0.0,
                1.0, -1.0, 0.0,
                1.0, 1.0, 0.0,
                -1.0, 1.0, 0.0
                };

            IndexCount = sizeof(Faces) / sizeof(Faces[0]);

            // Create the VAO:
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Create the VBO for positions:
            GLsizei stride = 4 * sizeof(float);
            glGenBuffers(1, &positions);
            glBindBuffer(GL_ARRAY_BUFFER, positions);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(BufferLocation_);
            glVertexAttribPointer(BufferLocation_, 4, GL_FLOAT, GL_FALSE, stride, 0);

            // Create the VBO for indices:
            glGenBuffers(1, &indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
        }

        void bindBuffers() {

        }

    public:
        void init(GLuint ShaderHandle, GLuint BufferLocation, GLuint TesselationInner, GLuint TesselationOuter) {
            BufferLocation_ = BufferLocation;
            ShaderHandle_ = ShaderHandle;
            tesselation_outer_ = TesselationOuter;
            tesselation_inner_ = TesselationInner;
            succ_uniform_ids();
            set_buffers();
        }

        void draw(glm::vec3 C, glm::mat4 M, glm::mat4 P, glm::mat4 MVP) {
            glUseProgram(ShaderHandle_);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, positions);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

            glUniformMatrix4fv(UMVP_, 1, GL_FALSE, &MVP[0][0]);
            /* tesselation uniforms */
            glUniform1f(UTesselationInnerLevel_, tesselation_inner_);
            glUniform1f(UTesselationOuterLevel_, tesselation_outer_);

            glUniform3f(ULightPosition_, 0.25, 0.25, 1.0);
            printf("inserting to uniform %lf %lf %lf\n", C[0], C[1], C[2]);
            glUniform3f(UCameraPosition_, C[0], C[1], C[2]);

            glUniformMatrix4fv(UProjectionMatrix_, 1, 0, &M[0][0]);
            glUniformMatrix4fv(UModelViewMatrix_, 1, 0, &P[0][0]);

            /* lights and colors, do we even need these? */
            glUniform3f(UAmbientMaterial_, 0.04f, 0.04f, 0.04f);
            glUniform3f(UDiffuseMaterial_, 0, 0.75, 0.75);
                
            /* to mówi ile vertexów składa się na jeden patch */
            glPatchParameteri(GL_PATCH_VERTICES, 4); 

            glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);
        }
};