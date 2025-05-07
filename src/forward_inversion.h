#ifndef FORWARD_INVERSION
#define FORWARD_INVERSION

#include <math.h>

double calculateVolumeIntegral(const struct Prism *prism, double px, double py);
double calculateRMSE (struct ObservedMag *obsmag, int num_obs);
double golden_search_magnetization_multi(struct ObservedMag *obsmag, struct Prism *prisms, int num_prisms, int num_obs, double tol);

#endif
