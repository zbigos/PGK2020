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
/* 
         layout(location = 0) uniform float scale;
         layout(location = 1) uniform vec2  center;
         layout(location = 0) in vec2 pos;
         out vec4 vtex;
         
         void main(void) {
            vec2 p = (pos * scale + center);
            gl_Position = vec4(p, 0.0, 1.0);
         }

layout(location = 3) uniform vec3  cross_color;

*/

class MyLine : public AGLDrawable {
public:
   MyLine(int chunkcount, int winsize) : AGLDrawable(0) {
      setShaders();
      setBuffers(chunkcount, winsize);
   }
   void setShaders() {
      compileShaders(R"END(
         #version 330 core
         layout (location = 0) in vec2 aPos;
         layout (location = 1) in vec3 aColor;
         layout (location = 2) in vec4 aOffset;
         out vec3 fColor;

         void main()
         {
            fColor = aColor;
            if (gl_VertexID % 2 == 0 )
               gl_Position = vec4(aPos[0] + aOffset[0], aPos[1] + aOffset[1], 0.0, 1.0);
            else
               gl_Position = vec4(aPos[0] + aOffset[2], aPos[1] + aOffset[3], 0.0, 1.0);
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
   void setBuffers(int chunkcount, int winsize) {
      __chunkcount = chunkcount;
      int index = 0;
      int instances = chunkcount * chunkcount;
      float translations[chunkcount * chunkcount][4];
      for (int y = 0; y < chunkcount; y += 1)
      {
         for (int x = 0; x < chunkcount; x += 1)
         {
               float size_mod = 0.08;
               int seed = random()%360;
               float cy = cos(PI * (360.0 * float(seed) / float(360)) / 180.0) * size_mod;
               float cx = sin(PI * (360.0 * float(seed) / float(360)) / 180.0) * size_mod;
               printf("%lf %lf\n", cx, cy);

               float Ox = -1.0 + (((float)x + 0.5) / (float)chunkcount *2.0);
               float Oy = -1.0 + (((float)y + 0.5) / (float)chunkcount *2.0);

               translations[index][0] = Ox + cx;
               translations[index][1] = Oy + cy;
               
               translations[index][2] = Ox - cx;
               translations[index][3] = Oy - cy;

               printf("%lf %lf\n", translations[index][0], translations[index][1]);
               index++; 
         }
      }

    glGenBuffers(1, &eboId);
    glBindBuffer(GL_ARRAY_BUFFER, eboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * chunkcount * chunkcount, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    float linelength = 0.07;
    float lineVertices[] = {
        // positions     // colors
         0.0,  0.0f,  1.0f, 0.0f, 0.0f,
         0.0,  0.0f,  0.0f, 1.0f, 0.0f,
    };

    for(int i = 0 ; i < 2; i++) {
       for(int j = 0 ; j < 5; j++)
         printf("%lf ", lineVertices[i*5+j]);
         printf("\n");
    }

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
      glLineWidth(4.0);
      glDrawArraysInstanced(GL_LINES, 0, 2, __chunkcount * __chunkcount); // 100 triangles of 6 vertices each
   }
   void setColor(float r, float g, float b){
   }
 private:
   GLfloat cross_color[3] = { 1.0, 0.0, 0.0 };
   float __chunkcount = 1;
};

#endif