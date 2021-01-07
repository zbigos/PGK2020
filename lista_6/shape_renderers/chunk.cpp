
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
        GLuint Pretlsation_;
        GLuint UChunkPosition_;
        GLuint UTextureSampler_;
        GLuint UCartesian_;


        GLuint PatchCount;
        GLuint Preteselation_level;
        GLuint UBackground_;

        GLuint TextureId;

        bool background = false;

        glm::vec3 ooffset;

        unsigned short int *maphandle;

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
            Pretlsation_ = glGetUniformLocation(ShaderHandle_, "preteselation");
            UChunkPosition_ = glGetUniformLocation(ShaderHandle_, "ChunkPosition");
            UTextureSampler_ = glGetUniformLocation(ShaderHandle_, "myTextureSampler");
            UBackground_ = glGetUniformLocation(ShaderHandle_, "background");
            UCartesian_ = glGetUniformLocation(ShaderHandle_, "cartesian");
        }

        void set_buffers(GLuint preteselation_level, unsigned short int* mapdata) {
            maphandle = mapdata;

            if (mapdata != NULL) {
                glGenTextures(1, &TextureId);
                std::cout << "texture handl " << TextureId << " ";
                glBindTexture(GL_TEXTURE_2D, TextureId);

                glTexImage2D(
                    GL_TEXTURE_2D, 
                    0, 
                    GL_RED, // dziękuję bardzo stackoverflow. 
                    1200, 
                    1200, 
                    0, 
                    GL_RED, // naprawdę 0 pojęcia co tu się dzieje xD 
                    GL_SHORT,
                    mapdata
                ); 

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
            // pretesselate right now.
            GLuint tslation_faces = preteselation_level * preteselation_level;
            GLuint tslation_verts = (preteselation_level+1) * (preteselation_level);
            PatchCount = tslation_faces;
            Preteselation_level = preteselation_level;

            int *Faces = (int *)malloc(tslation_faces * 4 * sizeof(int));
            float *Verts = (float *)malloc(tslation_verts * 4 * sizeof(float));
            
            double msx = 1.0;
            double msy = 1.0;

            if (mapdata == NULL) {
                msx = 100.0;
                msy = 100.0;
            }

            for(int i = 0; i < (preteselation_level + 1); i += 1)
                for(int j = 0; j < (preteselation_level + 1); j += 1) {
                    Verts[(preteselation_level + 1) * i + j + 0] = (2.0 * msx)*((double)i/(double)(preteselation_level + 1)) - msx;
                    Verts[(preteselation_level + 1) * i + j + 1] = (2.0 * msy)*((double)j/(double)(preteselation_level + 1)) - msy;
                    Verts[(preteselation_level + 1) * i + j + 2] = 0.0;
                    if(!mapdata) {
                        printf("%lf %lf\n", Verts[(preteselation_level + 1) * i + j + 0], Verts[(preteselation_level + 1) * i + j + 1]);
                    }
                }

            for(int i = 0; i < preteselation_level; i += 1)
                for(int j = 0; j < preteselation_level; j += 1) {
                    Faces[preteselation_level * i + j + 0] = preteselation_level * (i + 0) + (j + 0);
                    Faces[preteselation_level * i + j + 1] = preteselation_level * (i + 1) + (j + 0);
                    Faces[preteselation_level * i + j + 2] = preteselation_level * (i + 1) + (j + 1);
                    Faces[preteselation_level * i + j + 3] = preteselation_level * (i + 0) + (j + 1);
                }

            // Create the VAO:
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Create the VBO for positions:
            glGenBuffers(1, &positions);
            glBindBuffer(GL_ARRAY_BUFFER, positions);
            glBufferData(GL_ARRAY_BUFFER, tslation_verts * 4 * sizeof(float), Verts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(BufferLocation_);
            glVertexAttribPointer(BufferLocation_, 3, GL_FLOAT, GL_FALSE, 0, 0);

            // Create the VBO for indices:
            glGenBuffers(1, &indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, tslation_faces * 4 * sizeof(int), Faces, GL_STATIC_DRAW);

            std::cout << "initialized with preteselation = " << preteselation_level << " teselation = " << tesselation_outer_ << std::endl;

            if (maphandle) free(maphandle);
        }


    public:
        void set_teselation(int tslevel) {
            tesselation_outer_ = tslevel;
            tesselation_inner_ = tslevel;
        }

        void PrintMapSignature() {
            return;

            for(int i = 0 ; i < 10; i+=1) {
                printf("%d ", maphandle[i]);
            }

            printf("\n");
        }

        glm::vec3 GetRndrChunkCenter() {
            if (mapview == SPHERE) {
                float earthradus = 5.0;
                float hradus = earthradus * cos(3.1415f/180.0 * ooffset[1]);
                glm::vec3 transformed = glm::vec3(
                    hradus * cos(3.1415/180.0 * ooffset[0]),
                    earthradus * sin(3.1415/180.0 * ooffset[1]),
                    hradus * sin(3.1415/180.0 * ooffset[0])
                );
                return transformed;
            } else return ooffset;
        }

        void init(GLuint ShaderHandle, GLuint BufferLocation, GLuint TesselationInner, GLuint TesselationOuter, GLuint preteselation_level, unsigned short int *mapdata) {
            BufferLocation_ = BufferLocation;
            ShaderHandle_ = ShaderHandle;
            tesselation_outer_ = TesselationOuter;
            tesselation_inner_ = TesselationInner;
            succ_uniform_ids();
            background = (mapdata == NULL);

            set_buffers(preteselation_level, mapdata);
        }

        void setOoffset(glm::vec3 offset) {
            ooffset = offset;
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
            glUniform1i(Pretlsation_, Preteselation_level);

            glUniform3f(ULightPosition_, 0.25, 0.25, 1.0);
            //printf("inserting to uniform %lf %lf %lf\n", C[0], C[1], C[2]);
            glUniform3f(UCameraPosition_, C[0], C[1], C[2]);
        
            glUniform3f(UChunkPosition_, ooffset[0], ooffset[1], ooffset[2]);

            glUniformMatrix4fv(UProjectionMatrix_, 1, 0, &M[0][0]);
            glUniformMatrix4fv(UModelViewMatrix_, 1, 0, &P[0][0]);
            
            if (mapview == SPHERE)
                glUniform1i(UCartesian_, 0);
            else if (mapview == TOPDOWN)
                glUniform1i(UCartesian_, 1);


            if (!background) {
                glBindTexture(GL_TEXTURE_2D, TextureId);
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(UTextureSampler_, 0);
                glUniform1i(UBackground_, 0);
            } else {
                glUniform1i(UBackground_, 1);
            }

            /* lights and colors, do we even need these? */
            glUniform3f(UAmbientMaterial_, 0.04f, 0.04f, 0.04f);
            glUniform3f(UDiffuseMaterial_, 0, 0.75, 0.75);
            
            /* wireframe */
            //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

            /* to mówi ile vertexów składa się na jeden patch */
            glPatchParameteri(GL_PATCH_VERTICES, 4); 

            glDrawElements(GL_PATCHES, PatchCount * 4, GL_UNSIGNED_INT, 0);
        }
};