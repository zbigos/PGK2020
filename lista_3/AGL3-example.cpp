// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// ==========================================================================
// AGL3 example usage 
//===========================================================================
#include <stdlib.h>
#include <stdio.h>

#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>

#include "shape_renderers/cross.cpp"
#include "shape_renderers/circle.cpp"
#include "shape_renderers/line.cpp"


#define chunkcount 10
#define winsize 800
#define chunksize ((winsize) / (chunkcount))
// ==========================================================================
// Drawable object: no-data only: vertex/fragment programs
// ==========================================================================
class MyTri : public AGLDrawable {
public:
   MyTri(
      char* board_back, char* board_front,    
      float a, float b, 
      float c, float d,
      float e, float f
   ) : AGLDrawable(0) {
      setShaders(board_back, board_front, a, b, c, d, e, f);
   }
   void setShaders(char* board_back, char* board_front,
   float a, float b, 
   float c, float d,
   float e, float f) {
      char shadera[1024], shaderb[1024];

      sprintf(shadera, R"END(
         #version 330 
         void main(void) {
            const vec2 vertices[3] = vec2[3](vec2( %lf, %lf),
                                             vec2( %lf, %lf),
                                             vec2( %lf,  %lf));

            gl_Position = vec4(vertices[gl_VertexID], 0.5, 1.0); 
         }

      )END", a, b, c, d, e, f);

      sprintf(shaderb, R"END(
         #version 330 
         in  vec4 gl_FragCoord;
         out vec4 color;

         void main(void) {
            int ry = int(gl_FragCoord[0]/(800/%d));
            int rx = int(gl_FragCoord[1]/(800/%d));
            float oy = ry * (800/%d) + ((800/%d)/2);
            float ox = rx * (800/%d) + ((800/%d)/2);
            float py = gl_FragCoord[0];
            float px = gl_FragCoord[1];

            if(((px-ox)*(px-ox) + (py-oy)*(py-oy)) < 0.2*(800/%d)*(800/%d))
               if((rx+ry) % 2 == 0)
                  color = vec4(%s);
               else
                  color = vec4(%s);
            else
               if((rx+ry) % 2 == 1)
                  color = vec4(%s);
               else
                  color = vec4(%s);
         } 

      )END", 
      chunkcount, chunkcount, chunkcount, chunkcount, chunkcount, chunkcount, chunkcount, chunkcount,
      board_back, board_front, board_back, board_front
      );
      printf("%s", shaderb);
      compileShaders(shadera, shaderb);
   }
   void draw() {
      bindProgram();
      glDrawArrays(GL_TRIANGLES, 0, 3);
   }
};
// skoro okno ma 800x800, to środkiem jest 400x400


// color = vcolor;
// ==========================================================================
// Window Main Loop Inits ...................................................
// ==========================================================================
class MyWin : public AGLWindow {
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
        : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    virtual void KeyCB(int key, int scancode, int action, int mods);
    void MainLoop();
};


// ==========================================================================
void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
       ; // do something
    }
    if (key == GLFW_KEY_HOME  && (action == GLFW_PRESS)) {
       ; // do something
    }
}

#define SQ(x) ((x)*(x))
// ==========================================================================
void MyWin::MainLoop() {
   ViewportOne(0,0,wd,ht);

   MyLine line(chunkcount, winsize);

   MyTri   trian1(
      "1, 1, 1, 1.0", "0, 0, 0, 1.0",
      -1.0f, 1.0f, 
      -1.0f, -1.0f, 
      1.0f, 1.0f
   );

   MyTri   trian2(
      "1, 1, 1, 1.0", "0, 0, 0, 1.0",
      1.0f, -1.0f, 
      -1.0f, -1.0f, 
      1.0f, 1.0f
   );

   MyCircle circle(15);


   float   tx=0.0, ty=0.0;

   do {
      // collision detection.

      glClear( GL_COLOR_BUFFER_BIT );
   
      AGLErrors("main-loopbegin");
      // =====================================================        Drawing
      trian1.draw();
      trian2.draw();

      line.draw();

      circle.draw(tx, ty);


      AGLErrors("main-afterdraw");

      glfwSwapBuffers(win()); // =============================   Swap buffers
      glfwPollEvents();
      //glfwWaitEvents();   

      if (glfwGetKey(win(), GLFW_KEY_DOWN ) == GLFW_PRESS) {
         ty -= 0.01;
      }
      
      if (glfwGetKey(win(), GLFW_KEY_UP ) == GLFW_PRESS) {
         ty += 0.01;
      }
      
      if (glfwGetKey(win(), GLFW_KEY_RIGHT ) == GLFW_PRESS) {
         tx += 0.01;
      }
      
      if (glfwGetKey(win(), GLFW_KEY_LEFT ) == GLFW_PRESS) {
         tx -= 0.01;
      }
   } while( glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0 );
}

int main(int argc, char *argv[]) {
   MyWin win;
   win.Init(800,800,"AGL3 example",0,33);
   win.MainLoop();
   return 0;
}
