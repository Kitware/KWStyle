// Second, redirect to the optimized version if necessary
#ifdef ITK_USE_OPTIMIZED_REGISTRATION_METHODS
#include "itkOptBSplineInterpolateImageFunction.txx"
#else

void test()
{
  test;
#ifdef ITK_USE_ORIENTED_IMAGE_DIRECTION
#endif

  return;
}

#endif
