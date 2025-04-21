#ifndef FORWARD_INVERSION
#define FORWARD_INVERSION

#include <math.h>

double calculateVolumeIntegral(const struct Prism *prism, double px, double py);
double calculateRMSE (struct ObservedMag *obsmag, int num_obs);
double golden_search_magnetization(struct ObservedMag *obsmag, struct Prism *prism, int num_obs, double tol);
double golden_search_z1_z2(struct ObservedMag *obsmag, struct Prism *prism, int num_obs, double tol);

#endif // FORWARD_INVERSION
