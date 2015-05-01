#include "utils.h"


double double_abs(double a) {
    if ( a >= 0.0) return a;
    else return -a;
}

double double_max(double a, double b) {
    if (a>b) return a;
    else return b;
}