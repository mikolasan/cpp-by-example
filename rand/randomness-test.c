// based on JavaScript code from
// https://home.ubalt.edu/ntsbarsh/business-stat/otherapplets/Randomness.htm

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void compute(int8_t *values, int size)
{
  int N = 0;
  float SUM = 0.0;
  
  // calculate mean
  // Run through all the input, add those that have valid values
  for (int i = 0; i < size; i++)
  {
    SUM += values[i];
    N++;
  }

  // check for insufficient data
  if (N <= 10)
  {
    printf("Insufficient data. Must have at least 10\n");
    exit(1);
  }

  // Do the math
  float x = SUM / (float)N; // mean value
  // float y = roundf(100000 * x);
  // float z = y / 100000.0;
  printf("Mean: %f\n", x);

  int R = 1;
  int N1 = 0;
  int N2 = 0;
  // run through each value and compare it with mean
  for (int i = 0; i < size; i++)
  {
    float v = (float)values[i];
    // check if a value is present and discard the ties
    if (v != x)
    {
      // check if it is greater than mean then adds one
      if (v > x)
      {
        N1++;
        int a = i;
        while (a > 0)
        {
          a--;
          if ((float)values[a] != x)
          {
            break;
          }
        }
        if ((float)values[a] < x)
        {
          R++;
        }
      }
      // if it is less than mean
      else if (v < x)
      {
        N2++;
        int a = i;
        while (a > 0)
        {
          a--;
          if ((float)values[a] != x)
          {
            break;
          }
        }
        if ((float)values[a] > x)
        {
          R++;
        }
      } // closing else-if statement
    }
  }

  printf("Scores: %d [%d %d]\n", R, N1, N2); // value of x or "Scores"

  // compute the expected mean and variance of R
  float EM = 1.0 + (2.0 * N1 * N2) / (N1 + N2); // Mean "Mu"
  float SD1 = (2.0 * N1 * N2) * (2.0 * N1 * N2 - N1 - N2);
  float SD2 = powf((float)(N1 + N2), 2);
  float SD3 = N1 + N2 - 1.0;
  float SD4 = SD1 / (SD2 * SD3); // Standard deviation "Sigma"
  float SD = sqrtf(SD4);
  // calculating P value MStyle
  float z1 = (R - EM) / SD;
  float z2 = abs(z1);
  float z = z2;

  float t = (z > 0) ? z : (-z);
  float P1 = powf((1 + t * (0.049867347 + t * (0.0211410061 + t * (0.0032776263 + t * (0.0000380036 + t * (0.0000488906 + t * (0.000005383))))))), -16);
  float p = 1 - P1 / 2;
  t = 1 - ((z > 0) ? p : 1 - p); // this is P-value

  if (t < 0.000001)
  {
    printf("p value: 'Almost Zero'\n");
    exit(2);
  }
  else
  {
    printf("p value: %f\n", t);
  }
  // determine the conclusion
  if (t < 0.01)
  {
    printf("Very strong evidence against randomness (trend or seasonality)\n");
  }
  else if (t < 0.05 && t >= 0.01)
  {
    printf("Moderate evidence against randomness\n");
  }
  else if (t < 0.10 && t >= 0.05)
  {
    printf("Suggestive evidence against randomness\n");
  }
  else if (t >= 0.10)
  {
    printf("Little or no real evidences against randomness\n");
  }
  else
  {
    printf("Strong evidence against randomness (trend or seasonality exists)\n");
  }
}

int main(int argc, char const *argv[])
{
  FILE *f = fopen("Debug/randomness_test.exe", "rb");
  if (f == NULL)
  {
    printf("no file\n");
    return -1;
  }

  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  printf("found %d bytes in binary file\n", size);
  if (size <= 0)
    return -1;
  
  fseek(f, 0, SEEK_SET);

  int8_t *values = malloc(size);
  fread(values, sizeof(int8_t), size, f);
  fclose(f);

  compute(values, size);
  free(values);

  return 0;
}
