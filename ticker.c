

float ticker_zero = -1;
float ticker_follow;
float ticker_last;

void ticker_poll(){
//#define GET_SECS ((*(long far *)MK_FP( 0x40 , 0x6c )) / 18.5)
#define GET_SECS (SDL_GetTicks()/1000.0)
  if( ticker_zero < 0 ){
    ticker_zero   = GET_SECS;
    ticker_follow =(GET_SECS - ticker_zero);
    ticker_last   = ticker_follow;
  }
  ticker_last = ticker_follow;
  ticker_follow += 0.1 *((GET_SECS - ticker_zero) - ticker_follow);
}

float ticker_get_seconds(){
  return ticker_follow;
}

float ticker_get_elapsed(){
  return ticker_follow - ticker_last;
}



