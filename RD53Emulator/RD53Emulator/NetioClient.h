#ifndef RD53ANETIOCLIENT_H
#define RD53ANETIOCLIENT_H

#include "RD53Emulator/Decoder.h"
#include "RD53Emulator/Encoder.h"
#include <queue>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <deque>
#include <thread>

namespace netio{
class context;
class low_latency_subscribe_socket;
class low_latency_send_socket;
}

namespace RD53A{

/**
 * A Helper class to handle the communication with felix-core.
 * @deprecated
 *
 * @brief NetioClient
 * @author Carlos.Solans@cern.ch
 * @author Kaan.Oyulmaz@cern.ch
 * @date June 2020
 **/

class NetioClient {

 public:

  NetioClient();

  ~NetioClient();

  /**
   * Connect to a FELIX server
   * Should be replaced by Connect(RID1,RID2,RID3,RID4,TX_RID)
   * @param quad User defined quad identifier. Not relevant for netio
   * @param server Hostname of the FELIX server
   * @param data_port Port number for read data (subscription) on the FELIX server
   * @param cmd_port Port number to send commands on the FELIX server
   * @param data_elink1 Dummy elink number for the communication with FELIX server
   * @param data_elink2 Dummy elink number for the communication with FELIX server
   * @param data_elink3 Dummy elink number for the communication with FELIX server
   * @param data_elink4 Dummy elink number for the communication with FELIX server
   * @param cmd_elink Dummy elink number for the communication with FELIX server
   **/
  void Connect(uint32_t quad, 
               std::string server, uint32_t data_port, uint32_t cmd_port, 
               uint32_t data_elink1, uint32_t data_elink2, uint32_t data_elink3, uint32_t data_elink4,
               uint32_t cmd_elink);

  /**
   * Disconnect from the FELIX server
   * @param quad User defined quad identifier. Not relevant for netio
   **/
  void Disconnect(uint32_t quad);

  /**
   * Send a command to the FELIX server of the quad
   * @param quad User defined quad identifier. Not relevant for netio
   * @param cmd RD53ACommand to send
   **/
  void SendCommand(uint32_t quad, Command * cmd);

  /**
   * Send a list of commands to the FELIX server of the quad
   * @param quad User defined quad identifier. Not relevant for netio
   * @param bytes pointer of bytes
   * @param length size of the pointer of bytes
   **/
  void SendBytes(uint32_t quad, uint8_t *bytes, uint32_t length);

  /**
   * Send bytes of commands to the FELIX server of the quad
   * @param quad User defined quad identifier. Not relevant for netio
   * @param cmds Vector of RD53ACommands to send
   **/
  void SendCommands(uint32_t quad, std::vector<Command *> cmds);

  /**
   * Check if there is more register frames received.
   * @param quad User defined quad identifier. Not relevant for netio
   * @return True if there is more registers
   **/
  bool HasNextRegister(uint32_t quad);

  /**
   * Get the next register frames received for the quad.
   * @param quad User defined quad identifier. Not relevant for netio
   * @return The next register frame
   **/
  RegisterFrame GetNextRegister(uint32_t quad);

  /**
   * Clear the list of register frames received for the quad
   * @param quad User defined quad identifier. Not relevant for netio
   */
  void Clear(uint32_t quad);

  /**
   * Set verbose mode
   * @param enable Enable verbose mode if true
   **/
  void SetVerbose(bool enable);

  /**
   * Read Sensor
   */
  void ReadSensor(uint32_t quad, uint32_t pos);

 private:

  bool m_verbose;
  netio::context * m_context;
  std::thread m_bg_thread;
  Encoder * m_encoder;
  Decoder * m_decoder;
  std::map<uint32_t, netio::low_latency_subscribe_socket*> m_data_socks;
  std::map<uint32_t, netio::low_latency_send_socket*> m_cmd_socks;
  std::map<uint32_t, std::deque<RegisterFrame> > m_data_regs;
  std::map<uint32_t, std::mutex > m_data_lock;
  std::map<uint32_t, std::string> m_quad_server;
  std::map<uint32_t, uint32_t> m_quad_data_port;
  std::map<uint32_t, uint32_t> m_quad_cmd_port;
  std::map<uint32_t, uint32_t> m_quad_cmd_elink;
  std::map<uint32_t, std::vector<uint32_t> > m_quad_data_elinks;

  void Send(uint32_t quad);
  //netio::sockcfg m_config;

};

}
#endif
