#include "RD53Emulator/RunNumber.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace RD53A;

RunNumber::RunNumber(){
  m_RunNumber=0;
}

RunNumber::~RunNumber(){}

uint32_t RunNumber::GetNextRunNumber(bool update){
  if(!update){ m_RunNumber++; return m_RunNumber; }
  m_RunNumber = GetRunNumber(update)+1;
  std::string home = getenv("HOME");
  std::fstream oF((home + "/.itk-felix-sw/RunNumber").c_str(), std::ios::out);
  oF << m_RunNumber << std::endl;
  oF.close();
  return m_RunNumber;
}

uint32_t RunNumber::GetRunNumber(bool update){
  if (!update) return m_RunNumber;
  if (system("mkdir -p ~/.itk-felix-sw") < 0) {
    std::cerr << "#ERROR# Loading run number from ~/.itk-felix-sw!" << std::endl;
  }
  std::string home = getenv("HOME");
  std::fstream iF((home + "/.itk-felix-sw/RunNumber").c_str(), std::ios::in);
  if (iF) {
    iF >> m_RunNumber;
  } else {
    system("echo \"1\n\" > ~/.itk-felix-sw/RunNumber");
    m_RunNumber = 1;
  }
  iF.close();
  return m_RunNumber;
}

string RunNumber::GetRunNumberString(uint32_t numdigits,bool update){
  std::ostringstream os;
  os<<std::setfill('0')<<setw(numdigits)<<GetRunNumber(update);
  return os.str();
}

string RunNumber::GetNextRunNumberString(uint32_t numdigits, bool update){
  std::ostringstream os;
  os<<std::setfill('0')<<setw(numdigits)<<GetNextRunNumber(update);
  return os.str();
}

void RunNumber::SetRunNumber(uint32_t runnumber){
  m_RunNumber = runnumber;
  std::string home = getenv("HOME");
  std::fstream oF((home + "/.itk-felix-sw/RunNumber").c_str(), std::ios::out);
  oF << m_RunNumber << std::endl;
  oF.close();
}
