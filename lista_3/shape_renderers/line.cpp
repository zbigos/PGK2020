#ifndef line_cpp_defined
    #define line_cpp_defined

    #include <stdlib.h>
    #include <stdio.h>

    #include "../AGL3Window.hpp"
    #include "../AGL3Drawable.hpp"

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
   MyLine() : AGLDrawable(0) {
      setShaders();
      setBuffers();
   }
   void setShaders() {
      compileShaders(R"END(
         #version 330 core
         layout (location = 0) in vec2 aPos;
         layout (location = 1) in vec3 aColor;
         layout (location = 2) in vec2 aOffset;

         out vec3 fColor;

         void main()
         {
            fColor = aColor;
            gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
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
   void setBuffers() {

      int index = 0;
      int instances = 100;
      float translations[100][2];
      for (int y = -10; y < 10; y += 2)
      {
         for (int x = -10; x < 10; x += 2)
         {
               translations[index][0] = (float)x / 10.0f;
               translations[index][1] = (float)y / 10.0f;
               index++; 
         }
      }

    glGenBuffers(1, &eboId);
    glBindBuffer(GL_ARRAY_BUFFER, eboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    // also set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, eboId); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.!
   }

   void draw(float tx, float ty) {
      printf("called\n");

      bindProgram();
      bindBuffers();
      //glDrawArrays(GL_LINES, 0, 2);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
   }
   void setColor(float r, float g, float b){
   }
 private:
   GLfloat cross_color[3] = { 1.0, 0.0, 0.0 };
};

#endif