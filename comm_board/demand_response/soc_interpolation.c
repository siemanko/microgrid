#include "soc_interpolation.h"

#include <math.h>

// how many different state of charge curves to we have
#define NUM_CURVES 9
// we should use curve <x> if
// battery_capcity_q * curve_charge_rate[x] is close to
// current_in - current_out
static const float curve_charge_rate[NUM_CURVES] = {
    -1.0/3.0,   // discharge: C/3
    -1.0/5.0,   // discharge: C/5
    -1.0/10.0,  // discharge: C/10
    -1.0/20.0,  // discharge: C/20
    -1.0/100.0, // discharge: C/100
    1.0/40.0,   // charge: c/40
    1.0/20.0,   // charge: c/20
    1.0/10.0,   // charge: c/10
    1.0/5.0    // charge c/5
};

#define CHARGE_TO_DISCHARGE_BRACKET 5

static const int curve_length[NUM_CURVES] = {
    42,
    44,
    44,
    47,
    39,
    45,
    44,
    38,
    44
};

float curve_voltage1[] = {9.49, 9.57, 9.66, 9.75, 9.83, 9.90, 9.98, 10.08, 10.17, 10.25, 10.32, 10.39, 10.48, 10.58, 10.66, 10.75, 10.80, 10.89, 10.95, 11.02, 11.05, 11.10, 11.15, 11.19, 11.24, 11.29, 11.32, 11.37, 11.42, 11.46, 11.50, 11.54, 11.56, 11.60, 11.63, 11.66, 11.68, 11.70, 11.73, 11.74, 11.76, 11.77 };
float curve_voltage2[] = {10.20, 10.27, 10.33, 10.43, 10.50, 10.57, 10.66, 10.71, 10.78, 10.85, 10.90, 10.95, 11.00, 11.06, 11.10, 11.15, 11.19, 11.24, 11.28, 11.32, 11.37, 11.39, 11.44, 11.49, 11.53, 11.58, 11.63, 11.66, 11.70, 11.75, 11.79, 11.82, 11.85, 11.88, 11.90, 11.92, 11.95, 11.98, 11.99, 12.01, 12.03, 12.04, 12.06, 12.07 };
float curve_voltage3[] = {10.98, 11.06, 11.14, 11.21, 11.26, 11.31, 11.38, 11.44, 11.49, 11.53, 11.58, 11.62, 11.68, 11.71, 11.76, 11.81, 11.85, 11.89, 11.92, 11.95, 12.00, 12.03, 12.06, 12.09, 12.11, 12.13, 12.15, 12.17, 12.19, 12.21, 12.23, 12.26, 12.29, 12.31, 12.34, 12.36, 12.39, 12.41, 12.43, 12.45, 12.46, 12.48, 12.48, 12.50 };
float curve_voltage4[] = {11.46, 11.50, 11.54, 11.56, 11.60, 11.65, 11.70, 11.75, 11.79, 11.83, 11.87, 11.91, 11.94, 11.98, 12.01, 12.06, 12.08, 12.11, 12.13, 12.16, 12.17, 12.20, 12.23, 12.24, 12.27, 12.29, 12.31, 12.33, 12.35, 12.36, 12.38, 12.40, 12.43, 12.45, 12.47, 12.49, 12.51, 12.53, 12.55, 12.56, 12.58, 12.58, 12.59, 12.59, 12.60, 12.61, 12.61 };
float curve_voltage5[] = {11.74, 11.77, 11.81, 11.84, 11.87, 11.90, 11.94, 11.97, 12.00, 12.04, 12.06, 12.10, 12.12, 12.16, 12.20, 12.24, 12.28, 12.30, 12.34, 12.36, 12.39, 12.41, 12.45, 12.46, 12.48, 12.51, 12.53, 12.56, 12.57, 12.60, 12.61, 12.63, 12.63, 12.65, 12.65, 12.66, 12.66, 12.67, 12.67 };
float curve_voltage6[] = {11.78, 11.89, 11.99, 12.13, 12.20, 12.28, 12.34, 12.42, 12.47, 12.54, 12.58, 12.64, 12.68, 12.71, 12.75, 12.78, 12.82, 12.85, 12.88, 12.92, 12.93, 12.94, 12.95, 12.95, 12.99, 12.99, 13.01, 13.05, 13.09, 13.15, 13.25, 13.33, 13.44, 13.58, 13.78, 13.94, 14.17, 14.33, 14.46, 14.62, 14.75, 14.85, 14.95, 15.02, 15.07 };
float curve_voltage7[] = {12.08, 12.16, 12.25, 12.31, 12.37, 12.43, 12.53, 12.63, 12.70, 12.75, 12.80, 12.86, 12.91, 12.94, 12.99, 13.03, 13.06, 13.10, 13.12, 13.15, 13.15, 13.16, 13.18, 13.22, 13.25, 13.31, 13.37, 13.45, 13.52, 13.62, 13.75, 13.91, 14.12, 14.30, 14.49, 14.67, 14.80, 14.94, 15.07, 15.17, 15.28, 15.36, 15.48, 15.54 };
float curve_voltage8[] = {12.35, 12.44, 12.56, 12.66, 12.74, 12.83, 12.94, 13.01, 13.06, 13.12, 13.19, 13.21, 13.27, 13.29, 13.34, 13.39, 13.44, 13.54, 13.64, 13.72, 13.79, 13.90, 14.01, 14.14, 14.38, 14.58, 14.79, 15.03, 15.20, 15.32, 15.46, 15.58, 15.68, 15.73, 15.81, 15.87, 15.93, 15.95 };
float curve_voltage9[] = {12.58, 12.64, 12.70, 12.75, 12.79, 12.86, 12.91, 12.94, 12.98, 13.05, 13.13, 13.19, 13.23, 13.27, 13.33, 13.37, 13.43, 13.48, 13.52, 13.57, 13.65, 13.72, 13.85, 14.01, 14.17, 14.35, 14.51, 14.65, 14.79, 14.90, 15.03, 15.14, 15.27, 15.36, 15.53, 15.78, 15.93, 15.97, 16.07, 16.17, 16.26, 16.33, 16.37, 16.39 };


static const float* curve_voltage[NUM_CURVES] = {
    // discharge: c/3
    curve_voltage1,
    // discharge: c/5
    curve_voltage2,
    // discharge: c/10
    curve_voltage3,
    // discharge: c/20
    curve_voltage4,
    // discharge: c/100
    curve_voltage5,
    // charge: c/40
    curve_voltage6,
    // charge: c/20
    curve_voltage7,
    // charge: c/10
    curve_voltage8,
    // charge: c/5
    curve_voltage9,
};

float curve_soc1[] = {0.13, 1.82, 3.38, 5.46, 6.89, 8.45, 10.14, 12.48, 14.30, 16.12, 17.95, 19.90, 22.63, 25.10, 27.44, 30.43, 32.90, 35.37, 38.36, 40.70, 42.91, 45.38, 47.85, 50.33, 53.06, 55.66, 58.26, 61.25, 65.02, 67.62, 70.22, 72.82, 75.42, 78.15, 80.62, 83.22, 85.96, 88.82, 91.16, 94.15, 97.01, 99.22 };
float curve_soc2[] = {0.00, 1.17, 2.73, 4.94, 6.37, 8.58, 11.05, 12.87, 15.08, 17.17, 19.38, 21.33, 23.02, 25.49, 27.31, 29.00, 31.08, 33.03, 34.85, 37.19, 39.40, 41.09, 43.56, 46.55, 49.15, 52.02, 55.27, 58.52, 60.86, 64.76, 67.36, 70.22, 72.43, 74.51, 76.98, 79.19, 81.27, 83.75, 86.22, 88.56, 90.64, 92.98, 95.97, 99.22 };
float curve_soc3[] = {0.00, 2.08, 4.55, 6.76, 8.84, 10.66, 14.04, 17.04, 19.51, 21.46, 24.06, 26.27, 28.74, 30.82, 33.42, 35.76, 38.10, 40.31, 42.39, 44.73, 47.72, 50.46, 52.28, 54.23, 56.31, 58.39, 60.08, 62.29, 63.85, 65.67, 68.01, 70.35, 72.95, 75.16, 78.15, 79.84, 82.31, 84.92, 88.04, 90.64, 92.98, 95.58, 97.92, 99.74 };
float curve_soc4[] = {0.26, 1.43, 2.99, 4.03, 5.33, 7.67, 9.62, 11.57, 13.65, 15.73, 17.30, 19.25, 20.81, 22.76, 24.32, 26.53, 28.35, 30.04, 31.86, 33.55, 35.24, 37.06, 39.66, 41.48, 43.69, 45.38, 47.46, 49.28, 50.98, 52.80, 54.49, 57.22, 59.30, 62.16, 64.63, 67.10, 69.57, 71.78, 74.25, 77.63, 80.36, 82.70, 85.44, 87.65, 90.90, 97.14, 99.74 };
float curve_soc5[] = {1.04, 2.47, 4.16, 5.33, 6.37, 8.06, 9.62, 11.18, 12.74, 14.43, 15.99, 18.21, 19.90, 22.11, 24.45, 27.31, 29.78, 31.99, 35.24, 37.71, 39.79, 42.91, 45.51, 47.98, 50.59, 54.10, 56.83, 59.69, 62.81, 65.54, 69.05, 71.65, 75.42, 78.54, 81.40, 83.88, 86.74, 93.89, 99.74 };
float curve_soc6[] = {10.15, 11.90, 13.66, 16.30, 18.65, 20.99, 22.60, 25.09, 27.00, 30.08, 32.42, 34.91, 37.70, 40.19, 42.68, 45.75, 48.83, 52.50, 56.45, 60.99, 64.07, 67.59, 70.37, 73.74, 76.97, 79.75, 82.68, 85.61, 88.25, 90.45, 93.82, 96.75, 98.95, 101.58, 103.78, 105.39, 106.71, 108.18, 109.35, 111.40, 113.01, 114.77, 116.68, 118.58, 119.90 };
float curve_soc7[] = {10.59, 12.78, 14.98, 16.74, 18.79, 20.84, 23.48, 27.44, 30.37, 33.30, 35.79, 38.43, 41.36, 44.00, 46.78, 50.15, 53.52, 58.65, 61.73, 65.39, 67.59, 70.67, 73.45, 75.79, 78.14, 80.63, 82.83, 85.76, 88.25, 91.33, 94.70, 97.19, 99.53, 101.00, 102.46, 103.64, 104.95, 106.57, 108.03, 109.94, 111.70, 113.75, 116.53, 118.73 };
float curve_soc8[] = {9.85, 13.66, 18.35, 22.90, 26.56, 30.37, 34.91, 39.01, 41.94, 45.46, 49.27, 52.79, 56.89, 59.97, 64.36, 69.35, 72.57, 76.82, 80.19, 82.97, 84.59, 86.78, 88.84, 90.45, 92.50, 93.82, 95.43, 97.04, 98.95, 100.56, 103.05, 105.69, 108.03, 110.52, 112.43, 115.51, 118.29, 119.76 };
float curve_soc9[] = {9.85, 13.37, 16.74, 19.52, 21.72, 24.80, 27.88, 29.64, 30.95, 33.74, 36.52, 39.89, 42.24, 44.58, 47.07, 49.71, 53.08, 56.89, 59.82, 63.49, 69.64, 72.72, 76.53, 79.46, 81.66, 83.27, 84.29, 85.17, 86.05, 86.93, 87.96, 88.69, 89.57, 90.74, 92.06, 95.28, 98.51, 100.12, 102.90, 106.27, 109.79, 113.16, 116.82, 119.90 };


static const float* curve_soc[NUM_CURVES] = {
    // discharge: c/3
    curve_soc1,
    // discharge: c/5
    curve_soc2,
    // discharge: c/10
    curve_soc3,
    // discharge: c/20
    curve_soc4,
    // discharge: c/100
    curve_soc5,
    // charge: c/40
    curve_soc6,
    // charge: c/20
    curve_soc7,
    // charge: c/10
    curve_soc8,
    // charge: c/5
    curve_soc9
};

static int get_bracket(float target, const float* values, const int num_values) {
    // bracket 0: less than value 0
    // bracket idx: between value idx-1 and curve idx
    // bracket (num_curves): more than value (num_curves - 1)

    int bracket_result;
    int bracket_idx;
    for(bracket_idx = 0; bracket_idx <= num_values; ++bracket_idx) {
        float value_lower_bound = -1.0/0.0; // -inf
        if (bracket_idx > 0) value_lower_bound = values[bracket_idx -1];
        float value_upper_bound = 1.0/0.0; // -inf
        if (bracket_idx < num_values) value_upper_bound = values[bracket_idx];
        // printf("bracket %d: %f,%f\n", bracket_idx, value_lower_bound, value_upper_bound);
        if (value_lower_bound <= target && target < value_upper_bound) {
            bracket_result = bracket_idx;
            break;
        }
    }

    return bracket_result;
}

float linear_interpolation(const float xtarget, const float x1, const float x2, const float y1, const float y2) {
    const float ratio = (xtarget - x1) / (x2-x1);
    return y1 + ratio * (y2-y1);
}

static float float_abs(float x) {
    return x > 0 ? x : -x;
}

static float get_soc_from_curve(float voltage, int curve_idx) {
    int voltage_bracket = get_bracket(voltage, curve_voltage[curve_idx], curve_length[curve_idx]);
    if (voltage_bracket == 0)
        return curve_soc[curve_idx][voltage_bracket];
    if (voltage_bracket == curve_length[curve_idx])
        return curve_soc[curve_idx][voltage_bracket - 1];

    const float v1 = curve_voltage[curve_idx][voltage_bracket - 1];
    const float v2 = curve_voltage[curve_idx][voltage_bracket];
    const float soc1 = curve_soc[curve_idx][voltage_bracket-1];
    const float soc2 = curve_soc[curve_idx][voltage_bracket];

    return linear_interpolation(voltage, v1, v2, soc1, soc2);
}

float get_soc_estimate(float voltage, 
                       float current_in,
                       float current_out,
                       float battery_capacity_q) {
    float rate_of_charge = (current_in - current_out) / battery_capacity_q;

    int curve_bracket = get_bracket(rate_of_charge, curve_charge_rate, NUM_CURVES);
    
    float estimate_percent;
    
    if (curve_bracket == 0) {
        estimate_percent = get_soc_from_curve(voltage, curve_bracket);
    } else if (curve_bracket == NUM_CURVES) {
        estimate_percent = get_soc_from_curve(voltage, curve_bracket-1);
    } else if (curve_bracket == CHARGE_TO_DISCHARGE_BRACKET) {
        if (rate_of_charge < 0.0) {
            estimate_percent = get_soc_from_curve(voltage, curve_bracket-1);
        } else {
            estimate_percent = get_soc_from_curve(voltage, curve_bracket);
        }
    } else {
        float log_my_roc = log(float_abs(rate_of_charge));
        float log_roc1 = log(float_abs(curve_charge_rate[curve_bracket-1]));
        float log_roc2 = log(float_abs(curve_charge_rate[curve_bracket]));
        float soc1 = get_soc_from_curve(voltage, curve_bracket- 1);
        float soc2 = get_soc_from_curve(voltage, curve_bracket);

        estimate_percent = linear_interpolation(log_my_roc, log_roc1, log_roc2, soc1, soc2);
    }
    
    return estimate_percent/100.0 * battery_capacity_q;

}
