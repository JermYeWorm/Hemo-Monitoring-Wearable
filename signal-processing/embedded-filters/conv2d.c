/*
 * 2D convolution template
 */

#include <stddef.h>
#include <stdio.h>

void convolve(const double Signal[/* SignalLen */], size_t SignalLen,
              const double Kernel[/* KernelLen */], size_t KernelLen,
              double Result[/* SignalLen + KernelLen - 1 */])
{
  size_t n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    size_t kmin, kmax, k;

    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
}

void printSignal(const char* Name,
                 double Signal[/* SignalLen */], size_t SignalLen)
{
  size_t i;

  for (i = 0; i < SignalLen; i++)
  {
    printf("%s[%zu] = %f\n", Name, i, Signal[i]);
  }
  printf("\n");
}

#define ELEMENT_COUNT(X) (sizeof(X) / sizeof((X)[0]))

int main(void)
{
  double signal[] = { 1, 1, 1, 1, 1 };
  double kernel[] = { 1, 1, 1, 1, 1 };
  double result[ELEMENT_COUNT(signal) + ELEMENT_COUNT(kernel) - 1];

  convolve(signal, ELEMENT_COUNT(signal),
           kernel, ELEMENT_COUNT(kernel),
           result);

  printSignal("signal", signal, ELEMENT_COUNT(signal));
  printSignal("kernel", kernel, ELEMENT_COUNT(kernel));
  printSignal("result", result, ELEMENT_COUNT(result));

  return 0;
}