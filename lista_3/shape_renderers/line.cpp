#ifndef line_cpp_defined
    #define line_cpp_defined

    #include <stdlib.h>
    #include <stdio.h>
    #include <math.h>

    #include "../AGL3Window.hpp"
    #include "../AGL3Drawable.hpp"

    #define PI 3.14159265

/* compiler take the wheel */

// ==========================================================================
// Drawable object with some data in buffer and vertex/fragment programs
// ==========================================================================
//         

class MyLine : public AGLDrawable {
public:
   MyLine(int chunkcount, int winsize, float *collision_data, float scale) : AGLDrawable(0) {
      setShaders();
      setBuffers(chunkcount, winsize, collision_data, scale);
   }
   void setShaders() {// aOffset is composed of vec2(middle position), rotation, scale
      compileShaders(R"END(
         #version 330 core
         layout (location = 0) in vec2 aPos;
         layout (location = 1) in vec3 aColor;
         layout (location = 2) in vec4 aOffset; 
         out vec3 fColor;

         void main()
         {
            fColor = aColor;

            vec2 dv = vec2(sin(aOffset[2]), cos(aOffset[2])) * aOffset[3];
            vec2 bo = vec2(aOffset[0], aOffset[1]);
            if (gl_VertexID % 2 == 0)
               gl_Position = vec4(bo + dv, 0.0, 1.0);
            else
               gl_Position = vec4(bo - dv, 0.0, 1.0);
         }

      )END", R"END(
         #version 330 core
         out vec4 FragColor;
         in vec3 fColor;

         void main()
         {
            FragColor = vec4(fColor, 1.0);
         }
      )END");
   }
   void setBuffers(int chunkcount, int winsize, float *collision_data, float scale) {
      __chunkcount = chunkcount;
      int index = 4;
      int instances = chunkcount * chunkcount + 4;
      float translations[chunkcount * chunkcount + 4][4];

      // prawy bok
      translations[0][0] = 0.999;
      translations[0][1] = 0.0;
      translations[0][2] = 0;
      translations[0][3] = 1.0;
      collision_data[0] = 1.0;
      collision_data[1] = 1.0;
      collision_data[2] = 1.0;
      collision_data[3] = -1.0;
      // lewy bok
      translations[1][0] = -0.999;
      translations[1][1] = 0.0;
      translations[1][2] = 0;
      translations[1][3] = 1.0;
      collision_data[4] = -1.0;
      collision_data[5] = 1.0;
      collision_data[6] = -1.0;
      collision_data[7] = -1.0;

      // gorny bok
      translations[2][0] = 0.0;
      translations[2][1] = 0.999;
      translations[2][2] = PI/2;
      translations[2][3] = 1.0;

      collision_data[8] = -1.0;
      collision_data[9] = 1.0;
      collision_data[10] = 1.0;
      collision_data[11] = 1.0;

      // dolny bok
      translations[3][0] = 0.0;
      translations[3][1] = -0.999;
      translations[3][2] = PI/2;
      translations[3][3] = 1.0;

      collision_data[12] = -1.0;
      collision_data[13] = -1.0;
      collision_data[14] = 1.0;
      collision_data[15] = -1.0;


      
      for (int y = 0; y < chunkcount; y += 1)
      {
         for (int x = 0; x < chunkcount; x += 1)
         {
               float blksize = winsize / chunkcount;
               // developed on 100 blksize, scale by that factor
               float size_mod = 0.08 * scale/0.2 * blksize/80.0;
               int seed = random()%360;
               float cy = cos(PI * (360.0 * float(seed) / float(360)) / 180.0);
               float cx = sin(PI * (360.0 * float(seed) / float(360)) / 180.0);
               
               float Ox = -1.0 + (((float)x + 0.5) / (float)chunkcount * 2.0);
               float Oy = -1.0 + (((float)y + 0.5) / (float)chunkcount * 2.0);
               translations[index][0] = Oy;
               translations[index][1] = Ox;
               translations[index][2] = (PI * (360.0 * float(seed) / float(360)) / 180.0);
               translations[index][3] = size_mod;

               collision_data[4*index + 0] = Oy + cx * size_mod;
               collision_data[4*index + 1] = Ox + cy * size_mod;

               collision_data[4*index + 2] = Oy - cx * size_mod;
               collision_data[4*index + 3] = Ox - cy * size_mod;
            
               index++; 
         }
      }
   
   /* no need to have colliders for the first cell*/
   translations[4][0] = 100.0;
   translations[4][1] = 100.0;
   translations[4][2] = 0.0;
   translations[4][3] = 0.0;
   collision_data[16] = 100.0;
   collision_data[17] = 100.0;
   collision_data[18] = 100.0;
   collision_data[19] = 100.0;

   /* last line is drawn as a separate call, since it rotates and does rainbow shit 
    * no reason to have colliders for it as well. */

   translations[chunkcount * chunkcount - 1 + 4][0] = 100.0;
   translations[chunkcount * chunkcount - 1 + 4][1] = 100.0;
   translations[chunkcount * chunkcount - 1 + 4][2] = 0.0;
   translations[chunkcount * chunkcount - 1 + 4][3] = 0.0;
   collision_data[(chunkcount * chunkcount - 1 + 4) * 4 + 0] = 100.0;
   collision_data[(chunkcount * chunkcount - 1 + 4) * 4 + 1] = 100.0;
   collision_data[(chunkcount * chunkcount - 1 + 4) * 4 + 2] = 100.0;
   collision_data[(chunkcount * chunkcount - 1 + 4) * 4 + 3] = 100.0;


    glGenBuffers(1, &eboId);
    glBindBuffer(GL_ARRAY_BUFFER, eboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (4 * chunkcount * chunkcount + 12), &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    float linelength = 0.07;
    float lineVertices[] = {
        // positions     // colors
         0.0,  0.0f,  0.7f, 0.7f, 0.7f,
         0.0,  0.0f,  0.7f, 0.7f, 0.7f,
    };

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    // also set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, eboId); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.!
   }

   void draw() {
      bindProgram();
      bindBuffers();
      //glDrawArrays(GL_LINES, 0, 2);
      glLineWidth(3.0);

      glDrawArraysInstanced(GL_LINES, 0, 2, (__chunkcount * __chunkcount + 4)); // 100 triangles of 6 vertices each
   }
   void setColor(float r, float g, float b){
   }
 private:
   GLfloat cross_color[3] = { 1.0, 0.0, 0.0 };
   int __chunkcount = 1;
};

#endif