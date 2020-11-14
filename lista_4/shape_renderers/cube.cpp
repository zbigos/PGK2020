#ifndef circle_cpp_defined
    #define circle_cpp_defined

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
class MyCircle : public AGLDrawable {
public:
   MyCircle(int vertex_count) : AGLDrawable(0) {
      setShaders();
      setBuffers(vertex_count);
   }
   void setShaders() {
      compileShaders(R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require
         layout(location = 0) in vec2 pos;
         layout(location = 0) uniform float scale;
         layout(location = 1) uniform vec2  center;
         out vec4 vtex;

         void main(void) {
            vec2 p = (pos * scale + center);
            gl_Position = vec4(p, 0.0, 1.0);
         }

      )END", R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         layout(location = 3) uniform vec3  cross_color;
         out vec4 color;

         void main(void) {
            color = vec4(cross_color,1.0);
         } 

      )END");
   }
    void setBuffers(int vertex_count) {
        bindBuffers();
        float size_mod = 3.0;
        __vertex_count = vertex_count;
        GLfloat vert[vertex_count * 3][3];
        for (int i = 0; i < vertex_count; i ++) {
            vert[3*i][0] = cos(PI * (360.0 * float(i) / float(vertex_count)) / 180.0) * size_mod;
            vert[3*i][1] = sin(PI * (360.0 * float(i) / float(vertex_count)) / 180.0) * size_mod;
            vert[3*i][2] = 0.0;

            vert[3*i + 1][0] = cos(PI * (360.0 * float(i + 1) / float(vertex_count)) / 180.0) * size_mod;
            vert[3*i + 1][1] = sin(PI * (360.0 * float(i + 1) / float(vertex_count)) / 180.0) * size_mod;
            vert[3*i + 1][2] = 0.0;

            vert[3*i + 2][0] = 0.0;
            vert[3*i + 2][1] = 0.0;
            vert[3*i + 2][2] = 0.0;
        }


      glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * 3 * sizeof(float), vert, GL_STATIC_DRAW );
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
         0,                 // attribute 0, must match the layout in the shader.
         3,       // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,//24,             // stride
         (void*)0            // array buffer offset
      );
   }
   void draw(float tx, float ty) {
      bindProgram();
      bindBuffers();
      glUniform1f(0, 1.0/16);  // scale  in vertex shader
      glUniform2f(1, tx, ty);  // center in vertex shader
      glUniform3f(3, cross_color[0],cross_color[1],cross_color[2]);

      glDrawArrays(GL_TRIANGLES, 0, __vertex_count * 3);
   }
   void setColor(float r, float g, float b){
      cross_color[0]=r;cross_color[1]=g;cross_color[2]=b;
   }
 private:
    int __vertex_count = 0;
   GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
};

#endif