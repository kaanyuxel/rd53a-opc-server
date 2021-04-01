#include "RD53Emulator/Matrix.h"

using namespace std;
using namespace RD53A;

Matrix::Matrix(){

  for(uint32_t col=0; col<400; col++){
    vector<Pixel*> vrow;
    for(uint32_t row=0; row<192; row++){
      if     (col<128)  { vrow.push_back(new Pixel(Pixel::Sync)); }
      else if(col>=264) { vrow.push_back(new Pixel(Pixel::Diff)); }
      else              { vrow.push_back(new Pixel(Pixel::Lin)); }
    }
    m_pixels.push_back(vrow);
  }

}

Matrix::~Matrix(){

  for(auto col : m_pixels){
      for (auto p : col){
        delete p;
    }
  }

}

void Matrix::SetQuad(uint32_t address, uint32_t value){

  uint32_t core_col = (address >> 10) & 0x1F;
  uint32_t core_row = (address >>  4) & 0x1F;
  uint32_t core_reg = (address >>  0) & 0x0F;
  SetPair(core_col, core_row, core_reg, 0, (value>>0)&0xFFFF);
  SetPair(core_col, core_row, core_reg, 1, (value>>8)&0xFFFF);

}

void Matrix::SetPair(uint32_t double_col, uint32_t row, uint32_t value){

  GetPixel(double_col*2+0,row)->SetValue((value>>0)&0xFF);
  GetPixel(double_col*2+1,row)->SetValue((value>>8)&0xFF);

}

void Matrix::SetPair(uint32_t core_col, uint32_t core_row, uint32_t core_reg, uint32_t reg_pair, uint32_t value){

  uint32_t col=(core_col<<2) | ((core_reg&0x1)<<1) | (reg_pair<<0);
  uint32_t row=(core_row<<3) | (core_reg>>1);

  GetPixel(col+0,row)->SetValue((value>>0)&0xFF);
  GetPixel(col+1,row)->SetValue((value>>8)&0xFF);

}

uint32_t Matrix::GetQuad(uint32_t address){

  uint32_t core_col = (address >> 10) & 0x1F;
  uint32_t core_row = (address >>  4) & 0x1F;
  uint32_t core_reg = (address >>  0) & 0x0F;
  return (GetPair(core_col, core_row, core_reg, 1) << 8) | GetPair(core_col, core_row, core_reg, 0);

}

uint32_t Matrix::GetPair(uint32_t double_col, uint32_t row){

  return (GetPixel(double_col*2+1,row)->GetValue()<<8) | (GetPixel(double_col*2+0,row)->GetValue());

}

uint32_t Matrix::GetPair(uint32_t core_col, uint32_t core_row, uint32_t core_reg, uint32_t reg_pair ){

  uint32_t col=(core_col<<2) | ((core_reg&0x1)<<1) | (reg_pair<<0);
  uint32_t row=(core_row<<3) | (core_reg>>1);
  return (GetPixel(col+1,row)->GetValue()<<8) | (GetPixel(col+0,row)->GetValue());

}

Pixel * Matrix::GetPixel(uint32_t col, uint32_t row){
  return m_pixels[col][row];
}
