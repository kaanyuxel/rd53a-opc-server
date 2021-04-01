#include "RD53Emulator/Handler.h"
#include "RD53Emulator/RunNumber.h"
#include "netio/netio.hpp"
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "TFile.h"

using json=nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int32_t, std::uint32_t, float>;
using namespace std;
using namespace RD53A;

struct FelixDataHeader{
  uint16_t length;
  uint16_t status;
  uint32_t elink;
};

struct FelixCmdHeader{
  uint32_t length;
  uint32_t reserved;
  uint64_t elink;
};

Handler::Handler(){

  string itkpath = getenv("ITK_PATH");
  m_config_path.push_back("./");
  m_config_path.push_back("./tuned/");
  m_config_path.push_back(itkpath+"/");
  m_config_path.push_back(itkpath+"/tuned/");
  //m_config_path.push_back(itkpath+"/config/");
  m_config_path.push_back(itkpath+"/installed/share/data/config/");

  m_verbose = false;
  m_backend = "posix";
  m_retune = false;
  m_enable = false;
  m_rootfile = 0;
  m_outpath = (getenv("ITK_DATA_PATH")?string(getenv("ITK_DATA_PATH")):".");
  m_rn = new RunNumber();
  m_output = true;
  m_nrow = 192;
  m_ncol = 400;
  m_fulloutpath = "";
}

Handler::~Handler(){
  while(!m_fes.empty()){
    FrontEnd* fe=m_fes.back();
    m_fes.pop_back();
    delete fe;
  }
  m_fes.clear();
  if(m_rootfile) delete m_rootfile;
  delete m_rn;
}

void Handler::SetVerbose(bool enable){
  m_verbose=enable;
  for(auto fe: m_fes){
    fe->SetVerbose(enable);
  }
}

void Handler::SetContext(string context){
  m_backend = context;
}

void Handler::SetInterface(string interface){
  m_interface = interface;
}

void Handler::SetRetune(bool enable){
  m_retune=enable;
}

void Handler::SetEnable(bool enable){
  m_enable=enable;
}

void Handler::EnableOutput(bool enable){
  m_output=enable;
}

void Handler::SetCharge(uint32_t charge){
  m_charge=charge;
}

uint32_t Handler::GetToT(){
  return m_ToT;
}

void Handler::SetToT(uint32_t ToT){
  m_ToT=ToT;
}

void Handler::GetScan(string scan){
  m_scan=scan;
}

uint32_t Handler::GetThreshold(){
  return m_threshold;
}

void Handler::SetThreshold(uint32_t threshold){
  m_threshold=threshold;
}

void Handler::SetOutPath(string path){
  m_outpath=path;
}

std::string Handler::GetFullOutPath(){
  return m_fulloutpath;
}

void Handler::SetMapping(string mapping){

  for(string const& sdir : m_config_path){
   if(m_verbose) cout << "Handler::SetMapping" << " Testing: " << sdir << mapping << endl;
   std::ifstream fr(sdir+mapping);
   if(!fr){continue;}
   cout << "Handler::SetMapping" << " Loading: " << sdir << mapping << endl;
   json config;
   fr >> config;
   for(auto node : config["connectivity"]){
     AddMapping(node["name"],node["config"],node["tx"],node["rx"],node["host"],node["cmd_port"],node["host"],node["data_port"]);
   }
   fr.close();
   break;
  }

}

FrontEnd* Handler::GetFE(string name){
  return m_fe[name];
}

std::vector<FrontEnd*> Handler::GetFEs(){
  return m_fes;
}

bool Handler::GetEnable(){
  return m_enable;
}

bool Handler::GetEnableOutput(){
  return m_output;
}

bool Handler::GetRetune(){
  return m_retune;
}

void Handler::AddMapping(string name, string config, uint32_t cmd_elink, uint32_t data_elink, string cmd_host, uint32_t cmd_port, string data_host, uint32_t data_port){
  if(m_verbose){
    cout << "Handler::AddMapping"
         << "name: " << name << ", "
         << "config: " << config << ", "
         << "cmd_host: " << cmd_host << ", "
         << "cmd_port: " << cmd_port << ", "
         << "cmd_elink: " << cmd_elink << ", "
         << "data_host: " << data_host << ", "
         << "data_port: " << data_port << ", "
         << "data_elink: " << data_elink << " "
         << endl;
  }
  m_fe[name]=0;
  m_enabled[name]=false;
  m_configs[name]=config;
  m_fe_rx[name]=data_elink;
  m_fe_tx[name]=cmd_elink;
  m_data_port[data_elink]=data_port;
  m_data_host[data_elink]=data_host;
  m_cmd_host[cmd_elink]=cmd_host;
  m_cmd_port[cmd_elink]=cmd_port;
}

void Handler::AddFE(string name, FrontEnd* fe){

  fe->SetName(name);
  fe->SetActive(true);
  m_fe[name]=fe;
  m_fes.push_back(fe);
  m_enabled[name]=true;
}

void Handler::AddFE(string name, string path){

  if(path==""){path=m_configs[name];}
  if(path==""){path=name+".json";}

  FrontEnd * fe = 0;
  for(string const& sdir : m_config_path){
   if(m_verbose) cout << "Handler::AddFE" << " Testing: " << sdir << path << endl;
   std::ifstream fr(sdir+path);
   if(!fr){
     //if(m_verbose) cout << "Handler::AddFE" << " File not found: " << sdir << path << endl;
     continue;
   }
   cout << "Handler::AddFE" << " Loading: " << sdir << path << endl;
   json config;
   fr >> config;
   //Create the front-end
   if(m_verbose) cout << "Handler::AddFE" << " Create front-end" << endl;
   fe = new FrontEnd();
   fe->SetVerbose(m_verbose);
   fe->SetName(name);

   if(m_verbose) cout << "Handler::AddFE" << " Reading configuration file" << endl;

   //Actually read the file
   fe->SetChipID(config["RD53A"]["Parameter"]["ChipId"]);
   fe->SetActive(true);

   if(m_verbose) cout << "Handler::AddFE" << " Reading global registers" << endl;
   fe->SetGlobalConfig(config["RD53A"]["GlobalConfig"]);

   if(m_verbose) cout << "Handler::AddFE" << " Reading pixel bits" << endl;
   for(uint32_t row=0;row<192;row++){
     for(uint32_t col=0;col<400;col++){
       // EJS 2020-10-27, json.hpp has problems dealing with booleans, had to workaround it like this
       fe->SetPixelEnable(col,row, (config["RD53A"]["PixelConfig"][col]["Enable"][row] == 0 && !m_enable?false:true));
       fe->SetPixelHitbus(col,row, (config["RD53A"]["PixelConfig"][col]["Hitbus"][row] == 0?false:true));
       fe->SetPixelInject(col,row, (config["RD53A"]["PixelConfig"][col]["InjEn"][row] == 0?false:true));
       fe->SetPixelThreshold(col,row, config["RD53A"]["PixelConfig"][col]["TDAC"][row]);
     }
   }

   if(m_verbose and false){
     if(m_verbose) cout << "Handler::AddFE" << " Enable mask:" << endl;
     for(uint32_t row=0;row<192;row++){
       for(uint32_t col=0;col<400;col++){
         cout << fe->GetPixelEnable(col,row);
       }
       cout << endl;
     }
   }

   m_fe[name]=fe;
   m_fes.push_back(fe);
   m_enabled[name]=true;

   if(m_fe_rx.count(name)==0 or m_fe_tx.count(name)==0){
     cout << "Handler::AddFE Configuration error. Connectivity file does not contains FE: " << name << endl;
     m_enabled[name]=false;
   }

   //File was found, no need to keep looking for it
   break;
  }
  if(!fe){cout << "Handler::AddFE File not found: " << path << endl;}
  else{
    if(m_verbose) cout << "Handler::AddFE File correctly loaded: " << path << endl;
  }
}

void Handler::SaveFE(FrontEnd * fe, string path){

  cout << "Handler::SaveFE " << fe->GetName() << " in: " << path << endl;
  json config;
  config["RD53A"]["name"]=fe->GetName();
  config["RD53A"]["Parameter"]["ChipId"]=fe->GetChipID();
  config["RD53A"]["GlobalConfig"]=fe->GetGlobalConfig();

  for(uint32_t row=0;row<192;row++){
    for(uint32_t col=0;col<400;col++){
      config["RD53A"]["PixelConfig"][col]["Enable"][row]=(uint32_t)fe->GetPixelEnable(col,row);
      config["RD53A"]["PixelConfig"][col]["Hitbus"][row]=(uint32_t)fe->GetPixelHitbus(col,row);
      config["RD53A"]["PixelConfig"][col]["InjEn"][row]=(uint32_t)fe->GetPixelInject(col,row);
      config["RD53A"]["PixelConfig"][col]["TDAC"][row]=(uint32_t)fe->GetPixelThreshold(col,row);
    }
  }

  ofstream fw(path);
  fw << setw(4) << config;
  fw.close();

}

void Handler::Connect(){

  //Connect to FELIX
  cout << "Handler::Connect Create the context" << endl;
  m_context = new netio::context(m_backend.c_str());
  m_context_thread = thread([&](){m_context->event_loop()->run_forever();});

  //TX
  for(auto it : m_fe_tx){
    if(m_enabled[it.first]==false){continue;}
    uint32_t tx_elink = it.second;
    if(m_tx.count(tx_elink)==0){
      cout << "Handler::Connect Connect to cmd elink: " << tx_elink << " at " << m_cmd_host[tx_elink] << ":" << m_cmd_port[tx_elink] << endl;
      m_tx[tx_elink]=new netio::low_latency_send_socket(m_context);
      m_tx[tx_elink]->connect(netio::endpoint(m_cmd_host[tx_elink],m_cmd_port[tx_elink]));
    }
    m_tx_fes[tx_elink].push_back(m_fe[it.first]);
  }

  //RX
  for(auto it : m_fe_rx){
    if(m_enabled[it.first]==false){continue;}
    uint32_t rx_elink = it.second;
    m_rx_fe[rx_elink] = m_fe[it.first];
    m_mutex[rx_elink].unlock();
    m_rx[rx_elink] = new netio::low_latency_subscribe_socket(m_context, [&,rx_elink](netio::endpoint& ep, netio::message& msg){
      m_mutex[rx_elink].lock();
      if(m_verbose) cout << "Handler::Connect Received data from " << ep.address() << ":" << ep.port() << " size:" << msg.size() << endl;
      vector<uint8_t> data = msg.data_copy();
      //We should remove any potential header before decoding
      FelixDataHeader hdr;
      memcpy(&hdr,(const void*)&data[0], sizeof(hdr));
      m_rx_fe[rx_elink]->HandleData(&data[sizeof(hdr)],data.size()-sizeof(hdr));
      m_mutex[rx_elink].unlock();
    });
    cout << "Handler::Connect Subscribe to data elink: " << rx_elink << " at " << m_data_host[rx_elink] << ":" << m_data_port[rx_elink] << endl;
    m_rx[rx_elink]->subscribe(rx_elink, netio::endpoint(m_data_host[rx_elink], m_data_port[rx_elink]));
  }

  if(!m_output) return;

  //Start a new run
  cout << "Starting run number: " << m_rn->GetNextRunNumberString(6,true) << endl;

  //Create the output directory
  if(m_verbose) cout << "Handler::Connect Creating output directory (if doesn't exist yet)" << endl;
  ostringstream cmd;
  cmd << "mkdir -p " << m_outpath << "/" << m_rn->GetRunNumberString(6) << "_" << m_scan;
  system(cmd.str().c_str());
  m_fulloutpath = m_outpath + "/" + m_rn->GetRunNumberString(6) + "_" + m_scan;

  //Open the output ROOT file
  ostringstream opath;
  opath << m_outpath << "/" << m_rn->GetRunNumberString(6) << "_" << m_scan << "/" << "output.root";
  if(m_verbose) cout << "Handler::Connect Creating root file " << opath.str().c_str() << endl;
  m_rootfile=TFile::Open(opath.str().c_str(),"RECREATE");
  if(m_verbose) cout << "Handler::Connect Created root file " << opath.str().c_str() << endl;

}

void Handler::Config(){

  cout<< "Handler::Config" << endl;

  for(auto fe : m_fes){
    //configure global registers
    fe->WriteGlobal();
    Send(fe);

    //configure pixel registers
    fe->WritePixels();
    Send(fe);
  }

}

void Handler::PrepareTrigger(uint32_t cal_delay){
  cout << "Handler::PrepareTrigger: Preparing trigger with delay " << cal_delay << endl;
  for(auto it : m_tx_fes){
    if(m_verbose) cout << "Handler::PrepareTrigger: Composing trigger message for tx " << it.first << endl;
    FrontEnd * fe = it.second.at(0);
    fe->Trigger(cal_delay);
    fe->ProcessCommands();
    m_trigger_msgs[it.first]=vector<uint8_t>();
    for(uint32_t i=0;i<fe->GetLength();i++){m_trigger_msgs[it.first].push_back(fe->GetBytes()[i]);}
    fe->Clear();
  }
}

void Handler::Trigger(){

  for(auto it : m_tx){
    if(m_verbose) cout << "Handler::Trigger: Trigger! for tx " << it.first << endl;

    FelixCmdHeader hdr;
    hdr.elink=it.first;
    hdr.length=m_trigger_msgs[it.first].size();
    netio::message msg;
    msg.add_fragment((uint8_t*)&hdr,sizeof(hdr));
    msg.add_fragment(m_trigger_msgs[it.first].data(), m_trigger_msgs[it.first].size());
    //send message
    it.second->send(msg);

    if(m_verbose){
      cout << "Handler::Trigger: Message: 0x" << hex;
      for(uint32_t i=0;i<msg.size();i++){
        cout << setw(2) << setfill('0') << ((uint32_t) msg[i]);
      }
      cout << dec << endl;
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void Handler::Send(FrontEnd * fe){
  //process the commands from the front-end
  fe->ProcessCommands();
  //quick return
  if(fe->GetLength()==0){return;}
  if(!m_enabled[fe->GetName()]){return;}
  //figure out the tx_elink
  uint32_t tx_elink=m_fe_tx[fe->GetName()];
  //copy the message
  vector<uint8_t> payload(fe->GetLength());
  for(uint32_t i=0;i<fe->GetLength();i++){payload[i]=fe->GetBytes()[i];};
  //build message
  FelixCmdHeader hdr;
  hdr.elink=tx_elink;
  hdr.length=fe->GetLength();
  netio::message msg;
  msg.add_fragment((uint8_t*)&hdr,sizeof(hdr));
  //msg.add_fragment(fe->GetBytes(), fe->GetLength());
  msg.add_fragment(payload.data(), payload.size());
  //send message
  m_tx[tx_elink]->send(msg); // EJS: this one hangs, probably because there's no active listener
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  fe->Clear();
}

void Handler::Disconnect(){

  sleep(3); // 2020-11-06: EJS hack to avoid crash at the end of Dig scan (waiting for all data buffers to empty)

  for(auto it : m_tx){
    cout << __PRETTY_FUNCTION__ << "Disconnect from cmd elink: " << it.first << " at " << m_cmd_host[it.first] << ":" << m_cmd_port[it.first] << endl;
    it.second->disconnect();
    delete it.second;
  }
  m_tx.clear();

  for(auto it : m_rx){
    cout << __PRETTY_FUNCTION__ << "Disconnect from data elink: " << it.first << " at " << m_data_host[it.first] << ":" << m_data_port[it.first] << endl;
    //it.second->unsubscribe(it.first, netio::endpoint(m_data_host[it.first], m_data_port[it.first]));
    delete it.second;
  }
  m_rx.clear();

  cout << __PRETTY_FUNCTION__ << "Stop event loop" << endl;
  m_context->event_loop()->stop();

  cout << __PRETTY_FUNCTION__ << "Join context thread" << endl;
  m_context_thread.join();

  cout << __PRETTY_FUNCTION__ << "Delete the context" << endl;
  delete m_context;

}

void Handler::PreScan(){}

void Handler::SaveConfig(std::string time){
  if(!m_output) return;
  for(auto fe: m_fes){
    if(!fe->IsActive()){continue;}
    ostringstream os;
    os << m_fulloutpath << "/" << m_configs[fe->GetName()] << "_" << time << ".json";
    SaveFE(fe,os.str());
  }
}

void Handler::Run(){
  cout << __PRETTY_FUNCTION__ << "FIXME: Handler::Run is supposed to be extended" << endl;
}

void Handler::Analysis(){}

void Handler::Save(){

  if(!m_output) return;
  SaveConfig("after");
  //Lines below not useful with real chip
  std::ifstream logEmulator("logFileEmulator.txt");
  if(logEmulator){
  ostringstream cmdmv;
  cmdmv << "mv ./logFileEmulator.txt " << m_fulloutpath << "/";
  system(cmdmv.str().c_str());
  }
  logEmulator.close();
  std::ifstream trueThrFile("trueThreshold.txt");
  if(trueThrFile) {
    trueThrFile.close();
    ostringstream cmdtmp;
    cmdtmp << "mv ./trueThreshold.txt " << m_fulloutpath << "/";
    system(cmdtmp.str().c_str());
  }
  trueThrFile.close();
  //Lines above not useful with real chip
  string path="./tuned/";

  system("mkdir -p ./tuned");

  for(auto fe: m_fes){
    if(!fe->IsActive()){continue;}
    ostringstream os;
    os << path << m_configs[fe->GetName()];
    //SaveFE(fe,os.str());  
  }


  if(m_rootfile){
    cout << "Save ROOT file: " << m_rootfile->GetName() << endl;
    m_rootfile->Close();
  }
}

