
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>

#define FB_WIDTH  640
#define FB_HEIGHT 480

void pix( SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B ){
    Uint32 *bufp =(Uint32 *)screen->pixels + y*screen->pitch/4 + x;
    *bufp = SDL_MapRGB( screen->format , R , G , B );
}

int render( SDL_Surface *screen );



void main(){ 
    
  SDL_Surface *screen;
  SDL_Event event;
  int     i;

  if( SDL_Init(SDL_INIT_VIDEO) < 0 ){
    fprintf(stderr, "!SDL_Init: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);
  
  screen = SDL_SetVideoMode( FB_WIDTH , FB_HEIGHT , 32 , SDL_SWSURFACE );
//    screen = SDL_SetVideoMode( FB_WIDTH , FB_HEIGHT , 32 , SDL_HWSURFACE|SDL_DOUBLEBUF );
  if( screen == NULL ){
    fprintf(stderr, "!SDL_SetVideoMode: %s\n", SDL_GetError());
    exit(1);
  }

  while(1){
    while( SDL_PollEvent(&event)){
      switch( event.type ){
        case SDL_KEYDOWN:
        //                    printf( "Il tasto %s e' stato premuto!\n",
        //                           SDL_GetKeyName(event.key.keysym.sym));
          break;
        case SDL_MOUSEMOTION:
        //                    printf("Il Mouse e' stato mosso da %d,%d "
        //                               "a(%d,%d)\n", 
        //                           event.motion.xrel, event.motion.yrel,
        //                           event.motion.x , event.motion.y );
          break;
        case SDL_MOUSEBUTTONDOWN:
        //                    printf(
        //                        "Il pulsante %d del Mouse e' stato "
        //                        "premuto a(%d,%d)\n",
        //                        event.button.button, 
        //                        event.button.x, 
        //                        event.button.y );
          break;
        case SDL_QUIT:
        //                    printf( "SDL_QUIT\n" );
          exit(0);
      }
    }

    if( SDL_MUSTLOCK(screen)){
      if( SDL_LockSurface(screen) < 0 ){
        return;
      }
    }

    render(screen);

    if( SDL_MUSTLOCK(screen)){
      SDL_UnlockSurface(screen);
    }
    
    SDL_Flip( screen );
  }
}

