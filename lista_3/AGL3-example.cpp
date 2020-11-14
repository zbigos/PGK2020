// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// ==========================================================================
// AGL3 example usage 
//===========================================================================
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>

#include <crossing.hpp>

#include "shape_renderers/cross.cpp"
#include "shape_renderers/circle.cpp"
#include "shape_renderers/line.cpp"
#include "shape_renderers/line_player.cpp"


#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

void wait( int useconds )
{   // Pretty crossplatform, both ALL POSIX compliant systems AND Windows
    #ifdef _WIN32
        Sleep( 1000 * seconds );
    #else
        usleep( useconds );
    #endif
}
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
      char shadera[2048], shaderb[2048];

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
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require

         in  vec4 gl_FragCoord;
         layout(location = 0) uniform int cc;
         layout(location = 1) uniform float lv;

         out vec4 color;

         void main(void) {
            int ry = int(gl_FragCoord[0]/(800/cc));
            int rx = int(gl_FragCoord[1]/(800/cc));
            float oy = ry * (800/cc) + ((800/cc)/2);
            float ox = rx * (800/cc) + ((800/cc)/2);
            float py = gl_FragCoord[0];
            float px = gl_FragCoord[1];

            if(((px-ox)*(px-ox) + (py-oy)*(py-oy)) < lv*((800/cc)*(800/cc)))
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
      board_back, board_front, board_back, board_front
      );
      printf("%s", shaderb);
      compileShaders(shadera, shaderb);
   }
   void draw(int chunkcount, float level) {
      bindProgram();
      bindBuffers();
      glUniform1i(0, chunkcount);
      glUniform1f(1, level);
      

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
    void MainLoop(int chunkcount, bool noclip, float level);
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

bool check_collisions(float mlpx, float mlpy, float mlkx, float mlky, MyPlayer *obstacle_optional, float *collision_data, int chunkcount) {
   bool colliding = false;
   for(int lineit = 0; lineit < chunkcount; lineit++) {
      Point pg1 = {mlpx, mlpy};
      Point pg2 = {mlkx, mlky};
      Point pq1 = {collision_data[lineit * 4 + 0], collision_data[lineit * 4 + 1]};
      Point pq2 = {collision_data[lineit * 4 + 2], collision_data[lineit * 4 + 3]};
      if(doIntersect(pg1, pq1, pg2, pq2)) {
         if (obstacle_optional != NULL)
            obstacle_optional[lineit].forcedraw(
               collision_data[lineit * 4 + 0], collision_data[lineit * 4 + 1], 
               collision_data[lineit * 4 + 2], collision_data[lineit * 4 + 3]
            );

         colliding = true;
      } else {
         if (obstacle_optional != NULL)
            obstacle_optional[lineit].forcedraw(100.0, 100.0, 100.0, 100.0);
      }
   }

   return colliding;
} 

void MyWin::MainLoop(int chunkcount, bool noclip, float level) {
   bool intro = true;
   bool outro = false;
   int intro_chunkcount = 1;
   int winsize = 800;
   ViewportOne(0,0,wd,ht);

   float collision_data [chunkcount * chunkcount + 4][4];
   MyLine line(chunkcount, winsize, *collision_data, level);
   MyPlayer obstacle_optional[chunkcount * chunkcount + 8];

   MyTri   trian1("1, 1, 1, 1.0", "0, 0, 0, 1.0", -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
   MyTri   trian2("1, 1, 1, 1.0", "0, 0, 0, 1.0", 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
   MyPlayer gracz;

   gracz.setColor(1.0, 1.0, 1.0);
   MyPlayer endpoint;

   int endrot = 0;
   float tx=1.0/float(chunkcount)-1.0, ty=1.0/float(chunkcount)-1.0;
   float mlpx, mlpy, mlkx, mlky;
   float elpx, elpy, elkx, elky;

   int rot = 0;
   bool last_colliding = false;
   float rollback_tx, rollback_ty;
   int rollback_rot;
   int delay = 100000;//00000;
   float mi = 0.0, ma = 0.5, dx = 0.007, state = level;
   double last_time = 0.0;
   do {
      double time = glfwGetTime();
      double time_delta = time-last_time;
      double desired = 1.0/60.0;
      double rest = desired - time_delta;
      printf("%lf\n", 1.0/time_delta);
      //printf("current %lf desired %lf sleep for %lf\n", time_delta, desired, rest);
      if (rest < 0) {
      //   printf("the thread is running behind, 60fps was expected, currently running at %lf\n", 1.0/time_delta);
      }
      last_time = time;
      glClear( GL_COLOR_BUFFER_BIT );
   
      AGLErrors("main-loopbegin");
      if (outro) {
         state += dx;
         if (state > ma) dx = -abs(dx);
         if (state < mi) dx = abs(dx);
         
         trian1.draw(intro_chunkcount, state);
         trian2.draw(intro_chunkcount, state);

      } else if (intro) {
         trian1.draw(intro_chunkcount, level);
         trian2.draw(intro_chunkcount, level);

         wait(delay);
         delay = (int)((float)delay*0.95);
         intro_chunkcount ++;
         if(chunkcount == intro_chunkcount)
            intro = false;

      } else {
         endrot += 3;
         if (endrot > 360)
            endrot = 0;


         // =====================================================        Drawing
         trian1.draw(chunkcount, level);
         trian2.draw(chunkcount, level);

         // assume that we won't hit a situation when one op does not equal full rot
         if (rot > 360)
            rot -= 360;
         if (rot < -1)
            rot += 360;

         gracz.draw(rot, tx, ty, level, chunkcount, mlpx, mlpy, mlkx, mlky);

         line.draw();
         bool colliding = check_collisions(mlpx, mlpy, mlkx, mlky, obstacle_optional, *collision_data, chunkcount * chunkcount + 4);
         
         if (!noclip) {
            if (colliding and not last_colliding) { // jeśli dopiero teraz zacząłem kolidować to zapamiętuję pozycje do rollbackowania
               rollback_tx = tx;
               rollback_ty = ty;
               rollback_rot = rot;
            } else if (colliding and last_colliding) { //jeśli kolidowałem wtedy, i teraz nadal koliduję, to wykonuję rollback
               tx = rollback_tx;
               ty = rollback_ty;
               rot = rollback_rot;
            }
         }

         int64_t c = HSVtoRGB(float(endrot), 100, 100);
         endpoint.setColor((float)((c >> 16) & 0xff)/255.0, (float)((c >> 8) & 0xff)/255.0, (float)(c & 0xff)/255.0);
         endpoint.draw(endrot, 1.0-1.0/float(chunkcount), 1.0-1.0/float(chunkcount), level, chunkcount, elpx, elpy, elkx, elky);

         
         float e_data[1][4] = {elpx, elpy, elkx, elky};
         if (check_collisions(mlpx, mlpy, mlkx, mlky, NULL, *e_data, 1)) {
            printf("brawo, wygrales w gre %ds\n", (int)glfwGetTime());
            outro = true;
         }


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
            rot += 2;
         }
         
         if (glfwGetKey(win(), GLFW_KEY_LEFT ) == GLFW_PRESS) {
            rot -= 2;
         }

         last_colliding = colliding;
         }

      AGLErrors("main-afterdraw");

      glfwSwapBuffers(win()); // =============================   Swap buffers
      glfwPollEvents();
      
   } while( glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0 );
}

int main(int argc, char *argv[]) {
   int boardsize = 0;
   int aflag = 0;
   int bflag = 0;
   char *cvalue = NULL;
   bool noclip = false;
   int index;
   char *level = NULL;
   int c, opterr = 0;
   char *seedopt = NULL;

   while ((c = getopt (argc, argv, "l:s:n:r:")) != -1)
    switch (c)
      {
      case 's':
        cvalue = optarg;
        break;
      case 'l':
        level = optarg;
        break;
      case 'r':
         seedopt = optarg;
         break;
      case 'n':
        noclip = true;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
         exit(1);
      }

   int chunkcount;
   if (cvalue)
      chunkcount = atoi(cvalue);
   else
      chunkcount = 10;

   if (seedopt) {
      printf("setting seed to %s\n", seedopt);
      srand(atoi(seedopt));
   }

   if (chunkcount > 16) {
      printf("could you like, not activelly try to break the game?");
      chunkcount = 16;
   }

   if (chunkcount < 2) {
      printf("could you like, not activelly try to break the game?\n");
      printf("this actually breaks shit, don't think I'll let you do that");
      chunkcount = 3;
   }

   float hardness = 0.25;
   if (level) {
      if (!strcmp(level, "easy")) hardness = 0.2;
      else if (!strcmp(level, "normal")) hardness = 0.25;
      else if (!strcmp(level, "hard")) hardness = 0.3;
      else if (!strcmp(level, "hurtmeplenty")) hardness = 0.4;
      else printf("idk what level is %s. Defaulting to normal\n", level);
   }    else printf("idk what level is %s. Defaulting to normal\n", level);

   MyWin win;
   win.Init(800,800,"AGL3 example",0,33);
   win.MainLoop(chunkcount, noclip, hardness);
   return 0;
}
