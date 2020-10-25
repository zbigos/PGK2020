#ifndef linepl_cpp_defined
    #define linepl_cpp_defined

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
class MyPlayer : public AGLDrawable {
public:
   MyPlayer() : AGLDrawable(0) {
      setShaders();
      __attribute__((unused)) float a,b,d,c;
      setBuffers(0.0, a, b, c, d);
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
   void setBuffers(float seed, float &mlpx, float &mlpy, float &mlkx, float &mlky) {
      bindBuffers();

      float size_mod = 1.0/16.0;
      float cy = cos(PI * (360.0 * float(seed) / float(360)) / 180.0) * size_mod;
      float cx = sin(PI * (360.0 * float(seed) / float(360)) / 180.0) * size_mod;
      
      mlpx = cx;
      mlpy = -cy;

      mlkx = -cx;
      mlky = cy;

      GLfloat vert[2][2] = {  // Cross lines
         { cx, -cy  },
         {-cx,  cy  },
      };

      glBufferData(GL_ARRAY_BUFFER, 2*2*sizeof(float), vert, GL_STATIC_DRAW );
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
         0,                 // attribute 0, must match the layout in the shader.
         2,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,//24,             // stride
         (void*)0            // array buffer offset
      );
   }

   void setBuffers(float mlpx, float mlpy, float mlkx, float mlky) {
      bindBuffers();

      GLfloat vert[2][2] = {  // Cross lines
         { mlpx, mlpy  },
         { mlkx, mlky  },
      };

      glBufferData(GL_ARRAY_BUFFER, 2*2*sizeof(float), vert, GL_STATIC_DRAW );
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
         0,                 // attribute 0, must match the layout in the shader.
         2,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,//24,             // stride
         (void*)0            // array buffer offset
      );
   }


   void forcedraw(float mlpx, float mlpy, float mlkx, float mlky) {
      setBuffers(mlpx, mlpy, mlkx, mlky);
      bindProgram();
      bindBuffers();
      glUniform1f(0, 1.0);  // scale  in vertex shader
      glUniform2f(1, 0.0, 0.0);  // center in vertex shader
      glUniform3f(3, 1.0, 0.0, 0.0);
      glDrawArrays(GL_LINES, 0, 4);
   }

   void draw(int rot, float px, float py, float &mlpx, float &mlpy, float &mlkx, float &mlky) {
      setBuffers(rot, mlpx, mlpy, mlkx, mlky);

      bindProgram();
      bindBuffers();
      glUniform1f(0, 1.0);  // scale  in vertex shader
      glUniform2f(1, px, py);  // center in vertex shader
      glUniform3f(3, cross_color[0], cross_color[1], cross_color[2]);

      mlpx += px;
      mlkx += px;

      mlpy += py;
      mlky += py;
      
      glDrawArrays(GL_LINES, 0, 4);
   }

   void setColor(float r, float g, float b){
      cross_color[0]=r;cross_color[1]=g;cross_color[2]=b;
   }
 private:
   GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
};

#endif