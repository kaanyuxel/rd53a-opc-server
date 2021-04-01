#include "RD53Emulator/NetioClient.h"
#include <netio/netio.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace RD53A;

struct FromFELIXHeader{
  FromFELIXHeader(uint16_t len, uint32_t elink) : length(len+8), status(0), elinkid(elink){};
  uint16_t length;
  uint16_t status;
  uint32_t elinkid;
};

struct ToFELIXHeader {
  uint32_t length;
  uint32_t reserved;
  uint64_t elinkid;
};

NetioClient::NetioClient(){
  cout << "NetioClient::Constructor" << endl;
  /*
    m_config = netio::sockcfg::cfg();
    m_config(netio::sockcfg::BUFFER_PAGES_PER_CONNECTION, 16);
    m_config(netio::sockcfg::PAGESIZE, 2000);
  */

  m_verbose = false;
  m_context = new netio::context("posix");
  m_decoder = new Decoder();
  m_encoder = new Encoder();

  //FIXME: Stop the loop
  m_bg_thread = thread([&](){m_context->event_loop()->run_forever();});

}

NetioClient::~NetioClient(){
  cout << "NetioClient::Destructor" << endl;
  for(auto it: m_data_socks){
    if(m_data_socks[it.first]==0) continue;
    cout << "Disconnect: " << it.first << endl;
    Disconnect(it.first);
    cout << "Clear: " << it.first << endl;
    Clear(it.first);
  }
  cout << "Stop event loop" << endl;
  m_context->event_loop()->stop();
  cout << "Bg thread join" << endl;
  m_bg_thread.join();
  cout << "Delete context" << endl;
  delete m_context;
  cout << "Delete encoder" << endl;
  delete m_encoder;
  cout << "Delete decoder" << endl;
  delete m_decoder;
}

void NetioClient::Connect(uint32_t quad,
                               string server, uint32_t data_port, uint32_t cmd_port, 
                               uint32_t data_elink1, uint32_t data_elink2, uint32_t data_elink3, uint32_t data_elink4,
                               uint32_t cmd_elink){
  
  m_quad_server[quad]=server;
  m_quad_data_port[quad]=data_port;
  m_quad_cmd_port[quad]=cmd_port;
  
  m_data_socks[quad] = new netio::low_latency_subscribe_socket(m_context, [&,quad](netio::endpoint& ep, netio::message& msg){ 
      cout << "Received data from " << ep.address() << ":" << ep.port() << " size:" << msg.size() << endl;
      vector<uint8_t> data = msg.data_copy();
      //We should remove any potential header before decoding
      FromFELIXHeader hdr(0,0);
      memcpy(&hdr,(const void*)&data[0], sizeof(hdr));
      uint32_t pos=sizeof(hdr);
      m_decoder->SetBytes(&data[pos],msg.size()-pos);
      if(m_verbose){cout << "Decode" << endl;}
      m_data_lock[quad].lock();
      m_decoder->Decode();  
      if(m_verbose){cout << "Decoded frames: " << endl;}
      for(auto frame: m_decoder->GetFrames()){
        if(m_verbose){cout << "-- " << frame->ToString() << endl;}
        if(frame->GetType()==Frame::REGISTER){
          RegisterFrame reg(dynamic_cast<RegisterFrame*>(frame));
          m_data_regs[quad].push_back(reg);
        }
      }
      m_data_lock[quad].unlock();
      
    });

  if(data_elink1!=0xFFFFFFFF){
    cout << "Subscribe to data elink: " << data_elink1 << " at " << server << ":" << data_port << endl;
    m_data_socks[quad]->subscribe(data_elink1, netio::endpoint(server, data_port));
    m_quad_data_elinks[quad].push_back(data_elink1);
  }
  if(data_elink2!=0xFFFFFFFF){
    cout << "Subscribe to data elink: " << data_elink2 << " at " << server << ":" << data_port << endl;
    m_data_socks[quad]->subscribe(data_elink2, netio::endpoint(server, data_port));
    m_quad_data_elinks[quad].push_back(data_elink2);
  }
  if(data_elink3!=0xFFFFFFFF){
    cout << "Subscribe to data elink: " << data_elink3 << " at " << server << ":" << data_port << endl;
    m_data_socks[quad]->subscribe(data_elink3, netio::endpoint(server, data_port));
    m_quad_data_elinks[quad].push_back(data_elink3);
  }
  if(data_elink4!=0xFFFFFFFF){
    cout << "Subscribe to data elink: " << data_elink4 << " at " << server << ":" << data_port << endl;
    m_data_socks[quad]->subscribe(data_elink4, netio::endpoint(server, data_port));
    m_quad_data_elinks[quad].push_back(data_elink4);
  }
  
  cout << "Connect to cmd elink: " << cmd_elink << " at " << server << ":" << data_port << endl;
  m_cmd_socks[quad] = new netio::low_latency_send_socket(m_context);
  try{m_cmd_socks[quad]->connect(netio::endpoint(server, cmd_port));}catch(...){}
  m_quad_cmd_elink[quad]=cmd_elink;
  
}

void NetioClient::Disconnect(uint32_t quad){
  string server=m_quad_server[quad];
  uint32_t data_port=m_quad_data_port[quad];
  //uint32_t cmd_port=m_quad_cmd_port[quad];
  for(auto data_elink : m_quad_data_elinks[quad]){
    cout << "Unsubscribe to data elink: " << data_elink << " at " << server << ":" << data_port << endl;
    m_data_socks[quad]->unsubscribe(data_elink, netio::endpoint(server, data_port));  
  }
  if(m_data_socks[quad]){
    delete m_data_socks[quad];
    m_data_socks[quad]=0;
  }
  if(m_cmd_socks[quad]){
    m_cmd_socks[quad]->disconnect();
    delete m_cmd_socks[quad];
    m_cmd_socks[quad]=0;
  }
}

void NetioClient::SendCommand(uint32_t quad, Command * cmd){
  m_encoder->Clear();
  m_encoder->AddCommand(cmd);
  m_encoder->Encode();
  Send(quad);  
}

void NetioClient::SendCommands(uint32_t quad, vector<Command *> cmds){
  m_encoder->Clear();
  for(uint32_t i=0;i<cmds.size();i++){
    m_encoder->AddCommand(cmds.at(i));
  }
  m_encoder->Encode();
  Send(quad);
}

void NetioClient::SendBytes(uint32_t quad, uint8_t *bytes, uint32_t length){
  if(!m_cmd_socks[quad]){return;}

  ToFELIXHeader hdr;
  hdr.elinkid=m_quad_cmd_elink[quad];
  hdr.length=length;
  netio::message msg;
  msg.add_fragment((uint8_t*)&hdr,sizeof(hdr));
  msg.add_fragment(bytes, length);

  m_cmd_socks[quad]->send(msg);
}

void NetioClient::Send(uint32_t quad){
  if(!m_cmd_socks[quad]){return;}

  cout << "Send command: " << m_encoder->GetByteString() << endl;
  for(auto command: m_encoder->GetCommands()){
    cout << "-- " << command->ToString() << endl; 
  }

  ToFELIXHeader hdr;
  hdr.elinkid=m_quad_cmd_elink[quad];
  hdr.length=m_encoder->GetLength();
  netio::message msg;
  msg.add_fragment((uint8_t*)&hdr,sizeof(hdr));
  msg.add_fragment(m_encoder->GetBytes(), m_encoder->GetLength());

  m_cmd_socks[quad]->send(msg);  
}

bool NetioClient::HasNextRegister(uint32_t quad){
  if(m_verbose) cout << "NetioClient::HasNextRegister" << endl;
  m_data_lock[quad].lock();
  bool empty=m_data_regs[quad].empty();
  m_data_lock[quad].unlock();
  return (!empty);
}

RegisterFrame NetioClient::GetNextRegister(uint32_t quad){
  if(m_verbose) cout << "NetioClient::GetNextRegister" << endl;
  m_data_lock[quad].lock();
  RegisterFrame reg=m_data_regs[quad].front();
  m_data_regs[quad].pop_front();
  m_data_lock[quad].unlock();
  return reg;
}

void NetioClient::Clear(uint32_t quad){
  if(m_verbose) cout << "NetioClient::Clear" << endl;
  m_data_lock[quad].lock();
  while(!m_data_regs[quad].empty()){
    m_data_regs[quad].pop_front();
  }
  m_data_lock[quad].unlock();
}

void NetioClient::SetVerbose(bool enable){
  m_verbose=enable;
}

void NetioClient::ReadSensor(uint32_t quad, uint32_t pos){




}
