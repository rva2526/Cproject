#include "read_parse.h"
#include "forward_inversion.h"
#include <math.h>

double calculateVolumeIntegral(const struct Prism *prism, double px, double py) {
    double v1 = 0.0, v2 = 0.0, v3 = 0.0, v4 = 0.0, v5 = 0.0, v6 = 0.0;  //volume integrals
    double prop = 400.0 * PI; //conversion to nT (given magnetic permeability of free space))
    // printf("%lf\n",prop);
    double bx, by, bz, b_total; //components of the magnetic field and total magnetic field
    
    for (int i = 0; i < prism->num_vertices; i++) {
        double x1, y1, x2, y2;
        if (i == prism->num_vertices - 1) {
            x1 = prism->vertex[i].north - px;
            y1 = prism->vertex[i].east - py;
            x2 = prism->vertex[0].north - px;
            y2 = prism->vertex[0].east - py;
        } else {
            x1 = prism->vertex[i].north - px;
            y1 = prism->vertex[i].east - py;
            x2 = prism->vertex[i+1].north - px;
            y2 = prism->vertex[i+1].east - py;
        }

        double delta_x = x2 - x1;
        double delta_y = y2 - y1;
        double delta_s = sqrt(delta_x * delta_x + delta_y * delta_y);
        double c = delta_y / delta_s;
        double s = delta_x / delta_s;
        double p = (x1 * y2 - x2 * y1) / delta_s;

        double d1 = x1 * s + y1 * c;
        double d2 = x2 * s + y2 * c;

        double r1sq = x1 * x1 + y1 * y1;
        double r2sq = x2 * x2 + y2 * y2;
        double r11 = sqrt(r1sq + prism->z1 * prism->z1);
        double r12 = sqrt(r1sq + prism->z2 * prism->z2);
        double r21 = sqrt(r2sq + prism->z1 * prism->z1);
        double r22 = sqrt(r2sq + prism->z2 * prism->z2);

        double f = log((r22 + prism->z2) / (r12 + prism->z2) * (r11 + prism->z1) / (r21 + prism->z1));
        double q = log((r22 + d2) / (r12 + d1) * (r11 + d1) / (r21 + d2));
        double w = atan2((prism->z2 * d2), (p * r22)) 
                     - atan2((prism->z2 * d1), (p * r12)) 
                     - atan2((prism->z1 * d2), (p * r21)) 
                     + atan2((prism->z1 * d1), (p * r11));

        v1 += s * c * f - c * c * w;
        v2 += s * c * w + c * c * f;
        v3 += c * q;
        v4 += -s * c * f - s * s * w;
        v5 += -s * q;
        v6 += w;
        // printf("%lf %lf %lf %lf %lf %lf\n",v1,v2,v3,v4,v5,v6);
    }

    double ml = cos(prism->minc) * cos(prism->mdec);
    double mm = cos(prism->minc) * sin(prism->mdec);
    double mn = sin(prism->minc);
    // printf("%lf %lf %lf\n",ml,mm,mn);

    double mx = prism->mi * ml;
    double my = prism->mi * mm;
    double mz = prism->mi * mn;
    // printf("%lf %lf %lf\n",mx,my,mz);
   
    bx = prop * (mx * v1 + my * v2 + mz * v3);
    by = prop * (mx * v2 + my * v4 + mz * v5);
    bz = prop * (mx * v3 + my * v5 + mz * v6);

    b_total = ml*bx + mm*by + mn*bz;
    
    // printf("total field: %lf\n",b_total);

    return b_total;
}

// Invert magnetization intensity using Golden Section Search over multiple prisms
double golden_search_magnetization_multi(struct ObservedMag *obsmag, struct Prism *prisms, int num_prisms, int num_obs, double tol) {
    const double phi = (1.0 + sqrt(5.0)) / 2.0;
    double mi_low = -50.0;
    double mi_high = 50.0;

    double mi1 = mi_high - (mi_high - mi_low) / phi;
    double mi2 = mi_low + (mi_high - mi_low) / phi;

    while (fabs(mi_high - mi_low) > tol) {
        // Evaluate RMSE at mi1
        for (int p = 0; p < num_prisms; p++) {
            prisms[p].mi = mi1;
        }
        for (int i = 0; i < num_obs; i++) {
            double total_calc_mag = 0.0;
            for (int p = 0; p < num_prisms; p++) {
                total_calc_mag += calculateVolumeIntegral(&prisms[p], obsmag[i].north, obsmag[i].east);
            }
            obsmag[i].calc_mag = total_calc_mag;
            obsmag[i].residuals = obsmag[i].obs_mag - total_calc_mag;
        }
        double rmse1 = calculateRMSE(obsmag, num_obs);

        // Evaluate RMSE at mi2
        for (int p = 0; p < num_prisms; p++) {
            prisms[p].mi = mi2;
        }
        for (int i = 0; i < num_obs; i++) {
            double total_calc_mag = 0.0;
            for (int p = 0; p < num_prisms; p++) {
                total_calc_mag += calculateVolumeIntegral(&prisms[p], obsmag[i].north, obsmag[i].east);
            }
            obsmag[i].calc_mag = total_calc_mag;
            obsmag[i].residuals = obsmag[i].obs_mag - total_calc_mag;
        }
        double rmse2 = calculateRMSE(obsmag, num_obs);

        // Shrink search interval
        if (rmse1 < rmse2) {
            mi_high = mi2;
            mi2 = mi1;
            mi1 = mi_high - (mi_high - mi_low) / phi;
        } else {
            mi_low = mi1;
            mi1 = mi2;
            mi2 = mi_low + (mi_high - mi_low) / phi;
        }
    }

    double best_mi = (mi_low + mi_high) / 2.0;

    // Finalize: set best mi into all prisms
    for (int p = 0; p < num_prisms; p++) {
        prisms[p].mi = best_mi;
    }

    // Update final modeled field and residuals
    for (int i = 0; i < num_obs; i++) {
        double total_calc_mag = 0.0;
        for (int p = 0; p < num_prisms; p++) {
            total_calc_mag += calculateVolumeIntegral(&prisms[p], obsmag[i].north, obsmag[i].east);
        }
        obsmag[i].calc_mag = total_calc_mag;
        obsmag[i].residuals = obsmag[i].obs_mag - total_calc_mag;
    }

    return best_mi;
}

double calculateRMSE (struct ObservedMag *obsmag, int num_obs){
    double sum_residuals = 0.0;

    for (int i = 0; i < num_obs; i++) {
        sum_residuals += obsmag[i].residuals * obsmag[i].residuals;
    }
    
    return sqrt(sum_residuals / num_obs);
}
