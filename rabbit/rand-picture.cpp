
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <vector>

/*
------------------------------------------------------------------------------
readable.c: My random number generator, ISAAC.
(c) Bob Jenkins, March 1996, Public Domain
You may use this code in any way you wish, and it is free.  No warrantee.
* May 2008 -- made it not depend on standard.h
------------------------------------------------------------------------------
*/
// #include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/* a ub4 is an unsigned 4-byte quantity */
typedef uint32_t ub4;

/* external results */
ub4 randrsl[256], randcnt;

/* internal state */
static    ub4 mm[256];
static    ub4 aa = 0, bb = 0, cc = 0;


void isaac()
{
  ub4 i, x, y;

  cc = cc + 1;    /* cc just gets incremented once per 256 results */
  bb = bb + cc;   /* then combined with bb */

  for (i = 0; i < 256; ++i)
  {
    x = mm[i];
    switch (i % 4)
    {
    case 0: aa = aa ^ (aa << 13); break;
    case 1: aa = aa ^ (aa >> 6); break;
    case 2: aa = aa ^ (aa << 2); break;
    case 3: aa = aa ^ (aa >> 16); break;
    }
    aa = mm[(i + 128) % 256] + aa;
    mm[i] = y = mm[(x >> 2) % 256] + aa + bb;
    randrsl[i] = bb = mm[(y >> 10) % 256] + x;

    /* Note that bits 2..9 are chosen from x but 10..17 are chosen
       from y.  The only important thing here is that 2..9 and 10..17
       don't overlap.  2..9 and 10..17 were then chosen for speed in
       the optimized version (rand.c) */
       /* See http://burtleburtle.net/bob/rand/isaac.html
          for further explanations and analysis. */
  }
}


/* if (flag!=0), then use the contents of randrsl[] to initialize mm[]. */
#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

void randinit(int flag)
{
  int i;
  ub4 a, b, c, d, e, f, g, h;
  aa = bb = cc = 0;
  a = b = c = d = e = f = g = h = 0x9e3779b9;  /* the golden ratio */

  for (i = 0; i < 4; ++i)          /* scramble it */
  {
    mix(a, b, c, d, e, f, g, h);
  }

  for (i = 0; i < 256; i += 8)   /* fill in mm[] with messy stuff */
  {
    if (flag)                  /* use all the information in the seed */
    {
      a += randrsl[i]; b += randrsl[i + 1]; c += randrsl[i + 2]; d += randrsl[i + 3];
      e += randrsl[i + 4]; f += randrsl[i + 5]; g += randrsl[i + 6]; h += randrsl[i + 7];
    }
    mix(a, b, c, d, e, f, g, h);
    mm[i] = a; mm[i + 1] = b; mm[i + 2] = c; mm[i + 3] = d;
    mm[i + 4] = e; mm[i + 5] = f; mm[i + 6] = g; mm[i + 7] = h;
  }

  if (flag)
  {        /* do a second pass to make all of the seed affect all of mm */
    for (i = 0; i < 256; i += 8)
    {
      a += mm[i]; b += mm[i + 1]; c += mm[i + 2]; d += mm[i + 3];
      e += mm[i + 4]; f += mm[i + 5]; g += mm[i + 6]; h += mm[i + 7];
      mix(a, b, c, d, e, f, g, h);
      mm[i] = a; mm[i + 1] = b; mm[i + 2] = c; mm[i + 3] = d;
      mm[i + 4] = e; mm[i + 5] = f; mm[i + 6] = g; mm[i + 7] = h;
    }
  }

  isaac();            /* fill in the first set of results */
  randcnt = 256;        /* prepare to use the first set of results */
}

constexpr auto max_size = std::numeric_limits<uint32_t>::max();

int main(int argc, char const *argv[])
{
    std::vector<int> freq_map(max_size, 0);
    std::vector<uint64_t> hist(34, 0);

    // std::map<uint32_t, uint64_t> m;
    
    uint64_t n_processed = 0;
    // uint64_t range_upper_limit = max_size; // 100000; //std::atoll(argv[1]);
    uint64_t sample_size = max_size;
    
    srand(0);

    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()}; 
    std::mt19937 rng(seed);
    std::uniform_int_distribution<uint32_t> dist(0, max_size);

    // isaac
    aa = bb = cc = (ub4)0;
    for (int i = 0; i < 256; ++i)
        mm[i] = randrsl[i] = dist(rng);
    randinit(1);
    int j = 256;


    std::cout << "Random value on [0 " << sample_size << "]: \n";

    

    do {

        if (j >= 256) {
            isaac();
            j = 0;
        }
        ++j;

        // uint32_t input_number = rand();
        // uint32_t input_number = dist(rng);
        uint32_t input_number = randrsl[j];

        // Classic Modulo (Biased)
        // uint32_t random_number = static_cast<uint32_t>(input_number % range_upper_limit);

        ++n_processed;
        // m[input_number]++;
        ++freq_map[input_number];

    } while (n_processed < sample_size * 4);
    
    // std::cout << m.size() << std::endl;
    std::cout << "Finished with rand, counting now..." << std::endl;

    int max = 0;
    uint32_t max_number = 0;
    uint64_t n_unique_numbers = 0;
    for (size_t i = 0; i < freq_map.size(); ++i) {

        ++hist[freq_map[i]];

        if (freq_map[i] > 0) {
            ++n_unique_numbers;
        }

        // if (freq_map[i] > 9) {
        //     std::cerr << freq_map[i] << ": " << i << std::endl;
        // }

        if (freq_map[i] > max) {
            max = freq_map[i];
            max_number = i;
        }
        
    }

    std::cout << "unique numbers after full cycle: " << n_unique_numbers << std::endl;
    
    std::cout << "max freq: " << max << " for value " << max_number << std::endl;
    
    
    for (int i = 0; i < hist.size(); ++i) {
        std::cout << i << ": " << hist[i] << std::endl;
    }

    // for (const auto& [number, hits] : m) {
    //     std::cout << number << ": " << hits / static_cast<double>(n_processed) << " [" << hits << "]" << std::endl;
    // }
    
    return 0;
}