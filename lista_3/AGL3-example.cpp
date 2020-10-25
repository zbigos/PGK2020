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

#include <crossing.hpp>

#include "shape_renderers/cross.cpp"
#include "shape_renderers/circle.cpp"
#include "shape_renderers/line.cpp"
#include "shape_renderers/line_player.cpp"

#define chunkcount 10
#define winsize 800
#define chunksize ((winsize) / (chunkcount))
// ==========================================================================
// Drawable object: no-data only: vertex/fragment programs
// ==========================================================================
int64_t HSVtoRGB(float H, float S,float V){
    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    int R = (r+m)*255;
    int G = (g+m)*255;
    int B = (b+m)*255;
    return B << 16 | G << 8 | R;
}

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

bool check_collisions(float mlpx, float mlpy, float mlkx, float mlky, MyPlayer *obstacle_optional, float *collision_data) {
   bool colliding = false;
   for(int lineit = 0; lineit < chunkcount * chunkcount + 4; lineit++) {
      Point pg1 = {mlpx, mlpy};
      Point pg2 = {mlkx, mlky};
      Point pq1 = {collision_data[lineit * 4 + 0], collision_data[lineit * 4 + 1]};
      Point pq2 = {collision_data[lineit * 4 + 2], collision_data[lineit * 4 + 3]};
      if(doIntersect(pg1, pq1, pg2, pq2)) {
         obstacle_optional[lineit].forcedraw(
            collision_data[lineit * 4 + 0], collision_data[lineit * 4 + 1], 
            collision_data[lineit * 4 + 2], collision_data[lineit * 4 + 3]
         );

         colliding = true;
      } else {
         obstacle_optional[lineit].forcedraw(100.0, 100.0, 100.0, 100.0);
      }
   }

   return colliding;
} 

void MyWin::MainLoop() {
   ViewportOne(0,0,wd,ht);

   float collision_data [chunkcount * chunkcount + 4][4];
   MyLine line(chunkcount, winsize, *collision_data);
   MyPlayer obstacle_optional[chunkcount * chunkcount + 4];

   MyTri   trian1("1, 1, 1, 1.0", "0, 0, 0, 1.0", -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
   MyTri   trian2("1, 1, 1, 1.0", "0, 0, 0, 1.0", 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
   MyPlayer gracz;
   MyPlayer endpoint;

   int endrot = 0;
   float tx=1.0/float(chunkcount)-1.0, ty=1.0/float(chunkcount)-1.0;
   float mlpx, mlpy, mlkx, mlky;
   float elpx, elpy, elkx, elky;

   int rot = 0;
   bool last_colliding = false;
   float rollback_tx, rollback_ty;
   int rollback_rot;

   do {

      endrot += 3;
      if (endrot > 360)
         endrot = 0;

      glClear( GL_COLOR_BUFFER_BIT );
   
      AGLErrors("main-loopbegin");
      // =====================================================        Drawing
      trian1.draw();
      trian2.draw();

      // assume that we won't hit a situation when one op does not equal full rot
      if (rot > 360)
         rot -= 360;
      if (rot < -1)
         rot += 360;

      gracz.draw(rot, tx, ty, mlpx, mlpy, mlkx, mlky);

      line.draw();

      bool colliding = check_collisions(mlpx, mlpy, mlkx, mlky, obstacle_optional, *collision_data);

      if (colliding and not last_colliding) { // jeśli dopiero teraz zacząłem kolidować to zapamiętuję pozycje do rollbackowania
         rollback_tx = tx;
         rollback_ty = ty;
         rollback_rot = rot;
      } else if (colliding and last_colliding) { //jeśli kolidowałem wtedy, i teraz nadal koliduję, to wykonuję rollback
         tx = rollback_tx;
         ty = rollback_ty;
         rot = rollback_rot;
      }

      int64_t c = HSVtoRGB(float(endrot), 100, 100);
      endpoint.setColor((float)((c >> 16) & 0xff)/255.0, (float)((c >> 8) & 0xff)/255.0, (float)(c & 0xff)/255.0);
      endpoint.draw(endrot, 1.0-1.0/float(chunkcount), 1.0-1.0/float(chunkcount), elpx, elpy, elkx, elky);
      

      AGLErrors("main-afterdraw");

      glfwSwapBuffers(win()); // =============================   Swap buffers
      glfwPollEvents();
      //glfwWaitEvents();   

      float movement_speed = 0.01f;

      if (glfwGetKey(win(), GLFW_KEY_DOWN ) == GLFW_PRESS) {
         ty -= cos(PI * (360.0 * float(rot) / float(360)) / 180.0) * movement_speed;
         tx += sin(PI * (360.0 * float(rot) / float(360)) / 180.0) * movement_speed;
      }
      
      if (glfwGetKey(win(), GLFW_KEY_UP ) == GLFW_PRESS) {
         ty += cos(PI * (360.0 * float(rot) / float(360)) / 180.0) * movement_speed;
         tx -= sin(PI * (360.0 * float(rot) / float(360)) / 180.0) * movement_speed;
      }
      
      if (glfwGetKey(win(), GLFW_KEY_RIGHT ) == GLFW_PRESS) {
         rot += 3;
      }
      
      if (glfwGetKey(win(), GLFW_KEY_LEFT ) == GLFW_PRESS) {
         rot -= 3;
      }

      last_colliding = colliding;
   } while( glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0 );
}

int main(int argc, char *argv[]) {
   MyWin win;
   win.Init(800,800,"AGL3 example",0,33);
   win.MainLoop();
   return 0;
}
