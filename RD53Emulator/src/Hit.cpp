#include "RD53Emulator/Hit.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace RD53A;

Hit::Hit(){
  m_col=0;
  m_row=0;
  m_tot=0;
  m_tid=0;
  m_ttag=0;
  m_bcid=0;
  m_evnum=0;
}

Hit::Hit(uint32_t tid, uint32_t ttag, uint32_t bcid){
  m_col=0;
  m_row=0;
  m_tot=0;
  m_tid=tid;
  m_ttag=ttag;
  m_bcid=bcid;
  m_evnum=0;
}

Hit::~Hit(){}

Hit * Hit::Clone(){
  Hit * hit = new Hit();
  hit->m_col=m_col;
  hit->m_row=m_row;
  hit->m_tot=m_tot;
  hit->m_tid=m_tid;
  hit->m_ttag=m_ttag;
  hit->m_bcid=m_bcid;
  hit->m_evnum=m_evnum;
  return hit;
}

void Hit::Update(uint32_t tid, uint32_t ttag, uint32_t bcid){
  m_tid=tid;
  m_ttag=ttag;
  m_bcid=bcid;
}

void Hit::Set(uint32_t col, uint32_t row, uint32_t tot){
  m_col=col;
  m_row=row;
  m_tot=tot;
}

uint32_t Hit::GetCol(){
  return m_col;
}

uint32_t Hit::GetRow(){
  return m_row;
}

uint32_t Hit::GetTOT(){
  return m_tot;
}

uint32_t Hit::GetTID(){
  return m_tid;
}

uint32_t Hit::GetTTag(){
  return m_ttag;
}

uint32_t Hit::GetBCID(){
  return m_bcid;
}

uint32_t Hit::GetEvNum(){
  return m_evnum;
}

void Hit::SetCol(uint32_t col){
  m_col=col;
}

void Hit::SetRow(uint32_t row){
  m_row=row;
}

void Hit::SetTOT(uint32_t tot){
  m_tot=tot;
}

void Hit::SetTID(uint32_t tid){
  m_tid=tid;
}

void Hit::SetTTag(uint32_t ttag){
  m_ttag=ttag;
}

void Hit::SetBCID(uint32_t bcid){
  m_bcid=bcid;
}

void Hit::SetEvNum(uint32_t evnum){
  m_evnum=evnum;
}

std::string Hit::ToString(){
  ostringstream os;
  os << "Hit "
     << "evnum: " << m_evnum << " "
     << "col: " << m_col << " "
     << "row: " << m_row << " "
     << "tot: " << m_tot << " "
     << "tid: " << m_tid << " "
     << "ttag: " << m_ttag << " "
     << "bcid: " << m_bcid << " ";
  return os.str();
}

