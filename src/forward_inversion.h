#ifndef FORWARD_INVERSION
#define FORWARD_INVERSION

#include <math.h>



double calculateVolumeIntegral(const struct Prism *prism, double px, double py);
double calculateRMSE (struct ObservedMag *obsmag, int num_obs);

#endif // FORWARD_INVERSION
