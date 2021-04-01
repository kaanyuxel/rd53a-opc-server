#ifndef RD53A_HANDLER_H
#define RD53A_HANDLER_H

#include "RD53Emulator/FrontEnd.h"

#include <vector>
#include <cstdint>
#include <string>
#include <thread>

namespace netio{
  class low_latency_send_socket;
  class low_latency_subscribe_socket;
  class context;
}

class TH1I;
class TH2I;
class TFile;

namespace RD53A{

class RunNumber;

/**
 * A Handler is a tool to communicate with a FrontEnd through NETIO.
 * The configuration requires a mapping file, that contains a list of front-ends
 * described by the following:
 *
 * | Variable |   Type | Description                              |
 * | -------- |   ---- | -----------                              |
 * |     name | string | Front-end position (A_BM_01_1)           |
 * |   config | string | JSON configuration file (A_BM_01_1.json) |
 * |       rx |    int | Data e-link                              |
 * |       tx |    int | Command e-link                           |
 * |     host | string | FELIX host                               |
 * | cmd_port |    int | FELIX command port                       |
 * | data_port|    int | FELIX data port                          |
 *
 * The Handler allows to add any combination of front-ends (Handler::AddFE).
 * The configuration of each FrontEnd will be read out from the file specified in the mapping file.
 * And retrieved from a search path that built by the following directories in decreasing order of priority,
 * the tuned version of the configuration files is preferred, which is searched in the tuned
 * folder relative to the current working directory.
 *
 *  - Current working directory
 *  - ITK_PATH environment variable
 *  - share/data/config in the installed directory
 *
 * It is necessary to connect to FELIX (Handler::Connect) once all the FrontEnd objects have been added.
 * Once connected, all the FrontEnd objects can be configured (Handler::Config).
 * This will also create an output ROOT file that can contain the results of the scan.
 * Finally the run method (Handler::Run) implements the specific scan procedure.
 *
 * At the end of the scan, the output of each FrontEnd can be saved to the output directory.
 * This includes any ROOT histograms created during the scan, that are written to the output ROOT file.
 * The output data path is defined by the ITK_DATA_PATH, and can be overwritten by the user (Handler::SetOutPath).
 * This path contains
 *
 *  - The tuned configuration file (A_BM_01_1.json)
 *  - The results ROOT file (output.root)
 *  - The metadata file (metadata.txt)
 *
 * An example on how to use the Handler class is the following:
 *
 * @verbatim

  Handler * handler = new Handler();

  handler->SetVerbose(true);
  handler->SetContext("posix");
  handler->SetInterface("eth0");
  handler->SetMapping("mapping.json");

  handler->AddFE("A_BM_01_1");
  handler->AddFE("A_BM_01_2");
  handler->AddFE("A_BM_01_3");
  handler->AddFE("A_BM_01_4");

  //Connect to FELIX
  handler->Connect();

  //Configure the front-ends
  handler->Config();

  //Run the Handler
  handler->Run();

  //Disconnect from FELIX
  handler->Disconnect();

  //Save the results and config files
  handler->Save();

  delete handler;
  @endverbatim
 *
 * @brief RD53A Handler
 * @author Carlos.Solans@cern.ch
 * @author Enrico.Junior.Schioppa@cern.ch
 * @date June 2020
 */

class Handler{

public:

  /**
   * Create an empty Handler
   */
  Handler();

  /**
   * Delete the Handler
   */
  virtual ~Handler();

  int logFileFuntion(std::ofstream& file);
  /**
   * Enable the verbose mode
   * @param enable Enable the verbose mode if true
   */
  void SetVerbose(bool enable);

  /**
   * Set the netio::context as a string
   * @param context Back-end for the netio communication: posix or rdma
   */
  void SetContext(std::string context);

  /**
   * Set the network interface to use
   * @param interface The network interface to use.
   */
  void SetInterface(std::string interface);

  /**
   * Load a connectivity map file to the Handler. Structure should be the following:
   *
   * - mapping : List of FrontEnd configurations
   *   - []
   *     - name : Chip name (A_BM_05_1)
   *     - config : Relative path to the config file (A_BM_05_1.json)
   *     - rx : Data e-link (integer)
   *     - tx : Command e-link (integer)
   *     - host : FELIX host name
   *     - cmd_port : FELIX command port number (integer)
   *     - data_port : FELIX data port number (integer)
   *
   *
   * The search path is relative to the current working directory,
   * then to the ITK_PATH environment variable,
   * and finally to the installed directory.
   *
   * @param mapping path to the file to load
   */
  void SetMapping(std::string mapping);

  /**
   * Add a FrontEnd to the mapping, but do not load it into memory.
   * @param name Name of the FrontEnd
   * @param config Path to the configuration file
   * @param cmd_elink Command e-link number
   * @param data_elink Data e-link number
   * @param cmd_host Command host name
   * @param cmd_port Command port number
   * @param data_host Data host name
   * @param data_port Data port number
   */
  void AddMapping(std::string name, std::string config,
      uint32_t cmd_elink, uint32_t data_elink,
      std::string cmd_host, uint32_t cmd_port,
      std::string data_host, uint32_t data_port);

  /**
   * Add to the list of enabled front-ends the given FrontEnd object with the given name.
   * @param name Name of the FrontEnd
   * @param fe FrontEnd pointer
   */
  void AddFE(std::string name, FrontEnd* fe);

  /**
   * Add to the list of enabled front-ends a new FrontEnd object with the given name, and a given configuration path.
   * If the configuration path is not given the name must be available in the mapping file.
   *
   * Search path is relative to the current working directory,
   * then to the ITK_PATH environment variable,
   * and finally to the installed directory.
   *
   * The configuration file is a JSON file with the following contents:
   *
   * - RD53A
   *   - name : Chip name (A_BM_05_1)
   *   - Parameter
   *     - chipId : Wire-bonded chip ID
   *   - GlobalConfig : Map of field and value (see fei4b::Configuration::FieldType)
   *   - PixelConfig : List of pixel settings per column
   *     - []
   *       - Col : Row number (from 1 to 399)
   *       - Enable : Enable the pixel in the matrix 
   *       - Hitbus : Enable the pixel in the hit bus
   *       - InjEn : Enable the pixel for injection
   *       - TDAC : Pixel threshold setting 
   *       
   * @param name Name of the FrontEnd
   * @param path Path to the configuration file (Optional).
   */
  void AddFE(std::string name, std::string path="");

  /**
   * Save the FrontEnd configuration to the given path.
   * The tuned version of the configuration files is preferred,
   * thus the configuration file will be searched in the tuned
   * folder before the current working directory.
   * @param fe Pointer to the FrontEnd
   * @param path Path to the configuration file (Optional).
   */
  void SaveFE(FrontEnd * fe, std::string path="");

  /**
   * Configure the FrontEnd objects added to the Handler.
   * Start a new run by invoking the RunNumber class.
   * Set the front-ends in configuration mode (FrontEnd::SetRunMode).
   * Write the global registers (FrontEnd::ConfigGlobal).
   * Write the pixel registers one double column at a time (FrontEnd::ConfigDoubleColumn).
   * Reset the front-end ECR and BCR counters (FrontEnd::Reset)
   * Create the output directory in the output data path followed by the run number.
   * Create the output ROOT file inside the output directory.
   */
  void Config();

  /**
   * Create a netio::low_latency_send_socket to send commands (Command) to the FrontEnd,
   * and a netio::low_latency_subscribe_socket to receive data (Record) from the FrontEnd.
   * Subscribe to the data elink of each FrontEnd.
   * The received data is handled directly by the corresponding FrontEnd object (FrontEnd::HandleData),
   * the AddressRecord and ValueRecord are parsed automatically into the FrontEnd Configuration,
   * and the rest of the Record fragments are converted into Hit objects (FrontEnd::GetHit, FrontEnd::NextHit).
   */
  void Connect();

  /**
   * Disconnect the netio::low_latency_send_socket, and unsubscribe from the data elink of each FrontEnd.
   */
  void Disconnect();

  /**
   * Virtual method to  implement a PreScan sequence.
   */
  virtual void PreScan();

  /**
   * Virtual method to implement a Scan sequence by an extended class.
   */
  virtual void Run();

  /**
   * Write and delete histograms.
   */
  virtual void Analysis();

  /**
   * Save the configuration for each FrontEnd in the output folder.
   * @param time If "before" saves the configuration used at the beginning of the scan/tuning,
   * if "after" the one resulting from the scan/tuning.
   */
  void SaveConfig(std::string time);

  /**
   * Save the configuration for each FrontEnd in the tuned folder.
   */
  void Save();

  /**
   * Prepare the Trigger sequence for all the modules from the first FrontEnd.
   * Build the corresponding netio::message.
   * Here we assume that the front-ends are all connected to the same TX.
   * @param cal_delay Delay between the Command::Cal and the Command::Trigger in ns
   */
  void PrepareTrigger(uint32_t cal_delay=49);

  /**
   * Send a Trigger sequence to the FrontEnd through netio
   */
  void Trigger();

  /**
   * Send the pending Command messages to the selected FrontEnd
   * @param fe FrontEnd to send the pending messages to
   */
  void Send(FrontEnd *fe);

  /**
   * Get a single FrontEnd objects from this Handler
   * @param name of the frontend
   * @return the list of FrontEnd objects
   */
  FrontEnd* GetFE(std::string name);

  /**
   * Get the list of FrontEnd objects in this Handler
   * @return the list of FrontEnd objects
   */
  std::vector<FrontEnd*> GetFEs();

  /**
   * Define the path for the output of the results
   * @param path to the output
   */
  void SetOutPath(std::string path);

  /**
   * Get the output path
   * @return The path to the output
   */
  std::string GetOutPath();

  /**
   * Get the full output path
   * @return The full path to the output directory
   */
  std::string GetFullOutPath();

  /**
   * Enable or disable the re-tune flag for the scans.
   * This flag controls the initial values of the tuning.
   * If enabled, the initial values will be read from the configuration files.
   * @param enable Enable or disable the re-tune flag
   */
  void SetRetune(bool enable);

  /**
   * Get the re-tune flag for the scans.
   * This flag controls the initial values of the tuning.
   * If enabled, the initial values will be read from the configuration files.
   * @return The re-tune flag
   */
  bool GetRetune();

  /**
   * Sets the enable mask to 1 if true.
   * @param enable Set the enable flag
   */
  void SetEnable(bool enable);

  /**
   * Sets the enable mask to 1 if true.
   * @return The enable flag
   */
  bool GetEnable();

  /**
   * Enable/Disable the output of the Handler
   * @param enable Enable the output if true
   */
  void EnableOutput(bool enable);

  /**
   * Get the output enabled of the Handler
   * @return True if the output is enabled
   */
  bool GetEnableOutput();

  /**
   * Set the threshold charge.
   * Maximum charge is 100k electrons.
   * @param charge The charge in electrons.
   */
  void SetCharge(uint32_t charge);

  /**
   * Get the threshold charge.
   * Maximum charge is 100k electrons.
   * @return The charge in electrons.
   */
  uint32_t GetCharge();

  /**
   * Set the time over threshold.
   * Maximum ToT is 15 bc.
   * @param ToT The ToT in bc.
   */
  void SetToT(uint32_t ToT);

  /**
   * Get the time over threshold.
   * Maximum ToT is 15 bc.
   * @return The ToT in bc.
   */
  uint32_t GetToT();
  
  /**
   * Get the requested scan.
   * @param scan The scan name.
   */
  void GetScan(std::string scan);

  /**
   * Set the threshold.
   * @param threshold The threshold target in electrons.
   **/
  void SetThreshold(uint32_t threshold);

  /**
   * Get the threshold.
   * @return The threshold target in electrons.
   **/
  uint32_t GetThreshold();

protected:

  bool m_verbose;
  std::string m_backend;
  std::string m_interface;
  netio::context * m_context;
  std::thread m_context_thread;
  std::vector<std::string> m_config_path;
  TFile * m_rootfile;
  RunNumber * m_rn;

  bool m_retune;
  uint32_t m_charge;
  uint32_t m_ToT;
  uint32_t m_threshold;
  std::string m_scan;
  std::string m_outpath;
  std::string m_fulloutpath;
  bool m_enable;
  bool m_output;
  uint32_t m_nrow;
  uint32_t m_ncol;
  
  std::vector<FrontEnd*> m_fes;
  std::map<std::string, FrontEnd*> m_fe;
  std::map<std::string, bool>     m_enabled;
  std::map<uint32_t, std::mutex> m_mutex;
  std::map<std::string, std::string> m_configs;
  std::map<std::string, uint32_t> m_fe_tx;
  std::map<std::string, uint32_t> m_fe_rx;
  std::map<uint32_t, std::string> m_cmd_host;
  std::map<uint32_t, uint32_t>    m_cmd_port;
  std::map<uint32_t, std::string> m_data_host;
  std::map<uint32_t, uint32_t>    m_data_port;
  std::map<uint32_t, std::vector<FrontEnd*> > m_tx_fes;
  std::map<uint32_t, FrontEnd*> m_rx_fe;
  std::map<uint32_t, netio::low_latency_send_socket *> m_tx;
  std::map<uint32_t, netio::low_latency_subscribe_socket *> m_rx;
  std::map<uint32_t, std::vector<uint8_t> > m_trigger_msgs;


};

}

#endif
