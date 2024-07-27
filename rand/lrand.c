// By Tony Swain. all rights preservered. 
//Nevermind you can have it, but you can't sue me.
// Actually just whipped it up for @Miklosan & @Mathicino
// **** Yet another random # generator. This one is designed for speed.
// compile gcc -o lrand lrand.c
// advantage; all done in inline and in register; zippidy do dah!
// No implied warranty, Based on code by Leo Schwab written a bazillion years ago.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint32_t tRandomSeed=0;

#define SRAND(a) (tRandomSeed=(uint32_t)a)

static inline uint32_t lrand()
{
   tRandomSeed <<= 1;                // Shift everything left 1 bit
   //asm("here:");
   if(tRandomSeed & 0x80000000) 
   {
      tRandomSeed ^= 0x80000000;
   }     // See if we overflow 32 Bits
   else
   {
      tRandomSeed ^= 0x1D872B41; // Exclusive OR with an arbitrary value
   }
   return tRandomSeed;
}

// static inline __int32 lrand(long tRange)
// {
//    tRandomSeed <<= 1;                // Shift everything left 1 bit
//    //asm("here:");
//    if(tRandomSeed & 0x80000000) 
//    {
//       tRandomSeed ^= 0x80000000;
//    }     // See if we overflow 32 Bits
//    else
//    {
//       tRandomSeed ^= 0x1D872B41; // Exclusive OR with an arbitrary value
//    }
//    return (__int32)(tRandomSeed % tRange); // return range modulo
// }

/* initialize state to random bits */
static uint32_t state[16];
/* init should also reset this to 0 */
static uint8_t index = 0;
/* return 32 bit random number */
uint32_t WELLRNG512(void)
{
   uint32_t a, b, c, d;
   a = state[index];
   c = state[(index+13)&15];
   b = a^c^(a<<16)^(c<<15);
   c = state[(index+9)&15];
   c ^= (c>>11);
   a = state[index] = b^c;
   d = a^((a<<5)&0xDA442D24UL);
   index = (index + 15)&15;
   a = state[index];
   state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
   return state[index];
}

int main(int argc,char *argv[])
{
   uint32_t z, loops, range;
   uint32_t loopend;
   time_t lt = time(&lt);
   SRAND(lt);

   // if(argc == 1)
   // {
   //    loops = 42; //atoi(argv[1]);
   //    range = 0xffffffff;
   // }
   // else
   // {
   //    if(argc > 1)
   //    {
   //       loops = atoi(argv[1]);
   //       range = atoi(argv[1]);
   //    }
   // }
   
   // if(loops < 1)
   // {
   //    loops = 1;
   // }

   for (int i = 0; i < 16; ++i) {
      state[i] = lrand();
   }

   range = 0xffffffff;
   loops = 10;
   loopend = loops - 1;
   
   // fprintf(stdout, "loops %d range%d\r\n", loops, range);
   // fflush(stdout);

   union {
      uint32_t value;
      uint8_t stream[4];
   } v;

   // for (z = 0; z < loops; z++)
   while(1)
   {
      v.value = WELLRNG512();
      // fprintf(stdout, "%lu\n", v.value);
      for (int i = 0; i < 4; ++i)
      {
         fprintf(stdout, "%c", v.stream[4-i-1]);
      }
   }
}
