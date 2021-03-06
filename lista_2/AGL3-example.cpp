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

// ==========================================================================
// Drawable object: no-data only: vertex/fragment programs
// ==========================================================================
class MyTri : public AGLDrawable {
public:
   MyTri() : AGLDrawable(0) {
      setShaders();
   }
   void setShaders() {
      compileShaders(R"END(

         #version 330 
         out vec4 vcolor;
         void main(void) {
            const vec2 vertices[3] = vec2[3](vec2( 0.9, -0.9),
                                             vec2(-0.9, -0.9),
                                             vec2( 0.9,  0.9));
            const vec4 colors[]    = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),
                                             vec4(0.0, 1.0, 0.0, 1.0),
                                             vec4(0.0, 0.0, 1.0, 1.0));

            vcolor      = colors[gl_VertexID];
            gl_Position = vec4(vertices[gl_VertexID], 0.5, 1.0); 
         }

      )END", R"END(

         #version 330 
         in  vec4 vcolor;
         in  vec4 gl_FragCoord;
         out vec4 color;

         void main(void) {
            if(((gl_FragCoord[0] - 400) * (gl_FragCoord[0] - 400) + (gl_FragCoord[1] - 400) * (gl_FragCoord[1] - 400)) < 10000)
               color = vec4(1, 1, 1, 1.0);
            else
               color = vcolor;
         } 

      )END");
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

   MyCross cross;
   MyTri   trian;
   MyCircle circle(15);

   float cx, cy;
   srand(time(NULL));
   cx = float(random()%2000-1000)/1000.0;
   cy = float(random()%2000-1000)/1000.0;

   float   tx=0.0, ty=0.0;

   do {
      // collision detection.
      if ((SQ(cx - tx) + SQ(cy-ty)) < 0.09f) {
         cx = float(random()%2000-1000)/1000.0;
         cy = float(random()%2000-1000)/1000.0;
      }

      glClear( GL_COLOR_BUFFER_BIT );
   
      AGLErrors("main-loopbegin");
      // =====================================================        Drawing
      trian.draw();
      circle.draw(tx, ty);
      cross.draw(cx,cy);

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
