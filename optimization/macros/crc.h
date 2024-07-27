//--Mongoose-C inline macros 
// Various macros by Tony Swain and aided by ChatGTP.
// 
#define MODULE_MACRO 1
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>


#define GET_ENDIAN ((*(unsigned short *)"\0\xff" < 0x100) ? "Little endian" : "Big endian")

#define ROUND_TO(x, dp) ({ \
   typeof(x) _x = (x); \
   typeof(dp) _dp = (dp); \
   _x *= POW(10, _dp); \
   if (_x >= 0) { \
      _x += 0.5; \
   } else { \
      _x -= 0.5; \
   } \
   _x /= POW(10, _dp); \
   _x; \
})

#define POW(x, y) ({ \
   __typeof__(x) _x = (x); \
   __typeof__(y) _y = (y); \
   __typeof__(x) _res = 1; \
   if (_y >= 0) { \
      for (__typeof__(y) i = 0; i < _y; i++) { \
         _res *= _x; \
      } \
   } else { \
      for (__typeof__(y) i = 0; i > _y; i--) { \
         _res /= _x; \
      } \
   } \
   _res; \
})

#define ROUND(x, dp) ({ \
    __typeof__(x) _x = (x); \
    __typeof__(dp) _dp = (dp); \
    __typeof__(x) _res = 0; \
    __typeof__(x) _p = 1; \
    for (__typeof__(dp) i = 0; i < _dp; i++) { \
        _p *= 10; \
    } \
    if (_x >= 0) { \
        _res = (int)(_x * _p + 0.5) / (double)_p; \
    } else { \
        _res = (int)(_x * _p - 0.5) / (double)_p; \
    } \
    _res; \
})

#define SQRT(x, dp) ({ \
    __typeof__(x) _x = (x); \
    __typeof__(dp) _dp = (dp); \
    __typeof__(x) _y = _x; \
    __typeof__(dp) _p; \
    if (_x < 0) { \
        fprintf(stderr, "SQRT: cannot compute square root of a negative number\n"); \
        NAN; \
    } else { \
        for (_p = 0; _p <= _dp; _p++) { \
            _y = (_y + _x / _y) / 2; \
        } \
        ROUND(_y, _dp); \
    } \
})

//--
#define TRY do { jmp_buf ex_buf__; switch( setjmp(ex_buf__) ) { case 0: while(1) {
#define CATCH(exception) break; case exception:
#define FINALLY break; } default: {
#define ETRY break; } } } while(0);
#define THROW(exception) longjmp(ex_buf__, exception)

// Define exception types Name em what you want.
#define EXCEPTION_TYPE_0 0x00
#define EXCEPTION_TYPE_1 0x01
#define EXCEPTION_TYPE_2 0x02

// ... define up to 256 exception types

static int crc_initialized = 0;
static uint64_t crc_table[256];

#include <stdint.h>

static uint64_t crc_table[256];

#define INIT_CRC_TABLE() \
    do { \
        uint64_t poly = 0xc96c5795d7870f42ULL; \
        for (uint32_t i = 0; i < 256; i++) { \
            uint64_t crc = i; \
            for (int j = 0; j < 8; j++) { \
                crc = (crc >> 1) ^ (-(int64_t)(crc & 1) & poly); \
            } \
            crc_table[i] = crc; \
        } \
    } while (0)

#define CRC_64(buffer, size) \
    ({ \
        uint64_t crc = 0; \
        if (!crc_initialized) { \
            INIT_CRC_TABLE(); \
            crc_initialized = 1; \
        } \
        for (int i = 0; i < size; i++) { \
            crc = (crc >> 8) ^ crc_table[(crc ^ buffer[i]) & 0xff]; \
        } \
        crc; \
    })


