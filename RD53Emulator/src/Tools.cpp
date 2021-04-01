#include "RD53Emulator/Tools.h"

#include <iostream>

using namespace std;
using namespace RD53A;

Tools::Tools(){}

Tools::~Tools(){}

uint32_t Tools::injToVcal(double charge){
  // Computing V=Q/C
  double C = 8.2; // fF
  double vcal = ((charge / C) * 1.6/10. + 1.); // the numerical factors convert fF*mV into electrons
  return (unsigned) vcal / 0.215;
}

double Tools::injToCharge(double vcal){
  // Computing Q=CV
  double C = 8.2; // fF
  double V = (-1. + 0.215 * vcal); // mV, using linear approximation
  return C*V*10./1.6; // the numerical factors convert fF*mV into electrons
}

uint32_t Tools::thrToVth(double charge, uint32_t ccol){
  double par[2];
  getThCalibrationParameters(par, 2, ccol);
  return (charge-par[1])/par[0]; // simply linear
}

double Tools::thrToCharge(double vth, uint32_t ccol){
  double par[2];
  getThCalibrationParameters(par, 2, ccol);
  return par[1] + par[0] * vth; // simply linear
}

void Tools::getThCalibrationParameters(double *par, unsigned int nPar, uint32_t ccol){
  if(ccol>= 0 and ccol<16){ // syn
    par[0] = 10.3;
    par[1] = -241.6;
  }
  else if(ccol>= 16 and ccol<33){ // lin
    par[0] = 45.7;
    par[1] = -15910.;
  }
  else{ // diff
    par[0] = 3.2;
    par[1] = 298.;
  }
}


uint32_t Tools::chargeToToT(double DAC, double charge, uint32_t ccol){
  double par[4];
  getToTCalibrationParameters(par, 4, ccol);
  return (par[0] * DAC + par[1] + par[2] * charge + par[3])/2; 
}

void Tools::getToTCalibrationParameters(double *par, unsigned int nPar, uint32_t ccol){
  if(ccol>= 0 and ccol<16){ // syn
    par[0] = -0.07;
    par[1] = 15.6;
    par[2] = 0.0008;
    par[3] = 0.03;
  }
  else if(ccol>= 16 and ccol<33){ // lin
    par[0] = -0.17;
    par[1] = 17.2;
    par[2] = 0.0008;
    par[3] = 0.2;
  }
  else{ // diff
    par[0] = -0.12;
    par[1] = 18.5;
    par[2] = 0.0004;
    par[3] = 3.6;
  }
}
