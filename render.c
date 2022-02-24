

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "tri.h"


#define M_2PI 6.28318531




typedef     long            s32;
typedef     unsigned char   u8;


#include "ticker.c"

void xform( int w, int h, float *X , float *Y , float *Z , float x , float y , float a, float b ){
/*
  euler rot di un punto nel piano xy(z=0) 

  | X |   | 1   0   0 |   | cb -sb  0 |   | x |
  | Y | = | 0  ca -sa | * | sb  cb  0 | * | y |
  | Z |   | 0  sa  ca |   | 0    0  1 |   | 0 |

  | X |   |    cb   -sb   0 |   | x |
  | Y | = | ca*sb ca*cb -sa | * | y |
  | Z |   | sa*sb sa*cb  ca |   | 0 |

  la proiezione 3D→2D ha ø=centro schermo, gli assi sono x→ y↑ z× (mano sinistra)

  alla tmap serve l'inverso di z, quindi
  Z = 1/Z
*/

  float ca = cos(a);
  float sa = sin(a);
  float cb = cos(b);
  float sb = sin(b);

  *X = cb*x - sb*y;
  *Y = ca*sb*x + ca*cb*y;
  *Z = sa*sb*x + sa*cb*y + 150;

  if(*Z<=0){
    fprintf(stderr,"*Z<=0");
    exit(1);
  }

  *Z = 1 / *Z;

  *X = w/2 + (*X) * h * (*Z);
  *Y = h/2 - (*Y) * h * (*Z);
}



#define COUNT(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))


struct VVV {
  float x,y,u,v;
  float X,Y,Z;
} vvv[]={
  { -100,-100 ,  0,0  },
  { +100,-100 , 64,0  },
  { +100,+100 , 64,64 },
  { -100,+100 ,  0,64 }
};




int render( SDL_Surface *screen ){

  int i;
  float r;
  
  ticker_poll();
  r = ticker_get_seconds();
  
  memset( screen->pixels , 32 , screen->pitch*screen->h );

  for( i=0 ; i<COUNT(vvv) ; i++ ){
    xform( 
      screen->w,
      screen->h,

      &vvv[i].X ,
      &vvv[i].Y ,
      &vvv[i].Z , 

      vvv[i].x,
      vvv[i].y,

      M_2PI*60/360 , r
//      0,0
    );
//      DBG("%d %f %f %f",i,vvv[i].X,vvv[i].Y,vvv[i].Z);
  }



#define CP(D,S){\
  tri.vert[D].x = vvv[S].X;\
  tri.vert[D].y = vvv[S].Y;\
  tri.vert[D].z = vvv[S].Z;\
  tri.vert[D].u = vvv[S].u * vvv[S].Z;\
  tri.vert[D].v = vvv[S].v * vvv[S].Z;\
}

#define TRI(A,B,C){\
  CP(0,A);\
  CP(1,B);\
  CP(2,C);\
  tri_draw();\
}

  tri.buffer = screen->pixels;
  tri.bpl = screen->pitch;
  tri.bpp = 4;
  tri.clip.l = 10;
  tri.clip.r = screen->w-tri.clip.l;
  tri.clip.t = 10;
  tri.clip.b = screen->h-tri.clip.t;

  TRI(0,1,2);
  TRI(2,3,0);
}

