#ifndef RD53A_FRONTEND_H
#define RD53A_FRONTEND_H

#include "RD53Emulator/Decoder.h"
#include "RD53Emulator/Encoder.h"
#include "RD53Emulator/Command.h"
#include "RD53Emulator/Configuration.h"
#include "RD53Emulator/Matrix.h"
#include "RD53Emulator/Hit.h"
#include "RD53Emulator/TemperatureSensor.h"
#include "RD53Emulator/RadiationSensor.h"

#include <queue>
#include <vector>
#include <mutex>


namespace RD53A{

/**
 * This class controls an RD53A pixel detector that is identified by
 * a chip ID (FrontEnd::GetChipID, FrontEnd::SetChipID),
 * and has a name (FrontEnd::GetName, FrontEnd::SetName),
 * by making use of a Command Encoder, a Frame Decoder, a Field Configuration, and a Pixel Matrix.
 *
 * The Encoder encodes the operations on the FrontEnd like
 * loading the global configuration (FrontEnd::SetGlobalConfig),
 * loading the per pixel configuration (FrontEnd::SetPixelConfig),
 * setting different mask stages for calibration (FrontEnd::SetMask),
 * and triggering the detector (FrontEnd::Trigger),
 * into the corresponding Command objects.
 * These are converted into a byte stream on request (FrontEnd::ProcessCommands),
 * and available for the communication layer (FrontEnd::GetBytes, FrontEnd::GetLength).
 *
 * The Decoder decodes the data from the communication layer (FrontEnd::HandleData),
 * and assigns the read-back registers (RegisterFrame) to the global Configuration,
 * and converts the rest of the data (DataFrame) into Hit objects that are stored in a FIFO.
 * The status of the FIFO can be checked (FrontEnd::HasHits), and polled (FrontEnd::GetHit, FrontEnd::NextHit).
 *
 * The Configuration contains the global Register objects that can be accessed directly,
 * or through the virtual Field objects in which the Register objects are divided.
 *
 * The Matrix represents the pixel matrix configuration.
 *
 * @verbatim

   FrontEnd fe;

   fe.SetGlobalConfig(...);
   fe.ProcessCommands();
   fe.Trigger();

   uint8_t * out_bytes = fe.GetBytes();
   uin32_t out_length = fe.GetLength();

   fe.HandleData(in_bytes, in_length);

   while(fe.HasHits()){
     Hit * hit = fe.GetHit();
     hit.NextHit();
   }

   @endverbatim
 *
 * @todo Improve class documentation
 * @brief RD53A FrontEnd controller.
 * @author Carlos.Solans@cern.ch
 * @author Enrico.Junior.Schioppa@cern.ch
 * @date September 2020
 **/

  class FrontEnd {

  public:

    /**
     * Create a new RD53A FrontEnd
     * Allocates the Encoder and Decoder byte stream converters, and a Configuration object.
     **/
    FrontEnd();

    /**
     * Delete the Encoder, Decoder, and Configuration objects.
     */
    ~FrontEnd();

    /**
     * Enable the verbose mode
     * @param enable Enable verbose mode if true
     **/
    void SetVerbose(bool enable);

    /**
     * Get the ID for this emulated front-end.
     * @return The ID for this emulated front-end from 0 to 7.
     **/
    uint32_t GetChipID();

    /**
     * Set the ID for this emulated front-end from 0 to 7.
     * @return The ID for this emulated front-end from 0 to 7.
     **/
    void SetChipID(uint32_t chipid);

    /**
     * Set the common name of this FrontEnd.
     * @param name The common name for this FrontEnd.
     **/
    void SetName(std::string name);

    /**
     * Get the common name of this FrontEnd.
     * @return The common name for this FrontEnd.
     **/
    std::string GetName();

    /**
     * Get the global Configuration pointer for this FrontEnd.
     * It allows direct access to all the global 16-bit Register objects,
     * and the virtual Field objects associated like the HitOr
     * (FrontEnd::GetConfig()->GetField(Configuration::REGION_ROW)->GetValue()).
     * @return The global Configuration pointer
     */
    Configuration * GetConfig();

    /**
     * Clear the Command Encoder and the Record Decoder
     */
    void Clear();
    
    /**
     * Kaan's TEST to get commands for quasar 
     **/
    std::vector<Command*> & GetCommands();   
    
    /**
     * Check if the FrontEnd is active
     * An inactive FrontEnd should not be used further during a scan.
     * @return True if it is active
     */
    bool IsActive();

    /**
     * Force the FrontEnd to be active or inactive.
     * An inactive FrontEnd should not be used further during a scan.
     * @param active Activate if True, else deactivate the FrontEnd
     */
    void SetActive(bool active);

    /**
     * Set the global threshold for the given type of front-end
     * @param type The type of front-end (Sync, Linear, Diff)
     * @param threshold The global threshold
     */
    void SetGlobalThreshold(uint32_t type, uint32_t threshold);

    /**
     * Get the global threshold for the given type of front-end
     * @param type The type of front-end (Sync, Linear, Diff)
     * @return The global threshold
     */
    uint32_t GetGlobalThreshold(uint32_t type);

    /**
     * Set the output bit (Pixel::Enable) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @param enable Enable the Pixel output if true
     */
    void SetPixelEnable(uint32_t col, uint32_t row, bool enable);

    /**
     * Get the output bit (Pixel::Enable) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @return True if the Pixel output is enabled
     */
    bool GetPixelEnable(uint32_t col, uint32_t row);

    /**
     * Set the inject bit (Pixel::Inject) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @param enable Enable the Pixel injection if true
     */
    void SetPixelInject(uint32_t col, uint32_t row, bool enable);

    /**
     * Get the inject bit (Pixel::Inject) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @return True if the Pixel injection is enabled
     */
    bool GetPixelInject(uint32_t col, uint32_t row);

    /**
     * Get the hitbus bit (Pixel::Hitbus) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @param enable Enable the Pixel hitbus if true
     */
    void SetPixelHitbus(uint32_t col, uint32_t row, bool enable);

    /**
     * Get the hitbus bit (Pixel::Hitbus) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @return True if the Pixel hitbus is enabled
     */
    bool GetPixelHitbus(uint32_t col, uint32_t row);

    /**
     * Set the in pixel threshold (Pixel::TDAC) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @param threshold The pixel threshold
     */
    void SetPixelThreshold(uint32_t col, uint32_t row, uint32_t threshold);

    /**
     * Get the in pixel threshold (Pixel::TDAC) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @return The pixel threshold
     */
    uint32_t GetPixelThreshold(uint32_t col, uint32_t row);

    /**
     * Set the pixel gain (Pixel::Gain) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @param enable Pixel gain
     */
    void SetPixelGain(uint32_t col, uint32_t row, bool enable);

    /**
     * Set the pixel gain (Pixel::Gain) for the given Pixel
     * @param col The Pixel column
     * @param row The Pixel row
     * @return The pixel gain
     */
    bool GetPixelGain(uint32_t col, uint32_t row);

    /**
     * Set the global registers of this FrontEnd by passing a map of configuration values.
     * This is the default way to load the global configuration into memory.
     * In order to convert the configuration values into Commands call FrontEnd::ConfigGlobal
     * after calling this method.
     * The strings used for this map can be found in Configuration::m_name2index.
     * @param config A map of strings to unsigned 32 bit values
     */
    void SetGlobalConfig(std::map<std::string,uint32_t> config);

    /**
     * Get the global registers of this FrontEnd by passing a map of configuration values.
     * This is the default way to retrieve the global configuration into memory.
     * @return A map of strings to unsigned 32 bit values
     */
    std::map<std::string,uint32_t> GetGlobalConfig();

    /**
     * Convert the updated global registers into a WrReg Command array, and add them to the Encoder.
     */
    void WriteGlobal();

    /**
     * Request the reading of the global registers
     * @todo Might need to put the registers that have been requested into a queue
     * and compare with them in the HandleData
     */
    void ReadGlobal();

    /**
     * Write the in-pixel configuration of all the pixels in the front-end
     */
    void WritePixels();

    /**
     * Write the in-pixel configuration of the selected pixel pair to the front-end
     * @param double_col The pixel column pair (0 to 199)
     * @param row The pixel row (0 to 191)
     */
    void WritePixelPair(uint32_t double_col, uint32_t row);

    /**
     * Read the in-pixel configuration of all the pixels in the front-end
     * @todo Might need to implement a lookup table for the registers that come out
     */
    void ReadPixels();

    /**
     * Read the in-pixel configuration of the selected pixel pair to the front-end
     * @param double_col The pixel column pair (0 to 199)
     * @param row The pixel row (0 to 191)
     */
    void ReadPixelPair(uint32_t double_col, uint32_t row);

    /**
     * Enable a selection of pixels from the whole matrix
     * The mask mode selects how many pixels to skip in the masking
     *  - mode 0: all pixels masked
     *  - mode 1: mask every pixel
     *  - mode 2: mask every pixel out of 2
     *  - mode 3: mask every pixel out of 3
     *  - mode 8: mask every pixel out of 8
     * @param mask_mode How many pixels to skip in the masking
     * @param mask_iter Iteration in the pixel mask (up to mask_mode)
     */
    void SetMask(uint32_t mask_mode, uint32_t mask_iter);

    /**
     * Enable a subset of core columns for the scan.
     * The mode selects how many core columns are selected at a time.
     * The index runs from 0 to the value of mode effectively
     * changing the core column that is selected.
     *
     * @param mode How many core columns are selected at a time (0 to 40)
     * @param index Sequence number between 0 and mode
     * @param frontend the type of front-end affected: syn, lin, diff
     */
    void SetCoreColumn(uint32_t mode, uint32_t index, std::string frontend);

    /**
     * Enable or disable the core column identified by ccol.
     *
     * @param ccol Core column index (0 to 39)
     * @param enable Boolean desired status for the ccol. Enable if true, else disable.
     */
    void SelectCoreColumn(uint32_t ccol, bool enable);

    /**
     * Initialize the ADC of the chip to read the radiation an temperature sensors
     */
    void InitAdc();

    /**
     * Read a given radiation or temperature sensor
     * @param pos Position of the sensor (0 to 3)
     * @param read_radiation_sensor true if is a radiation sensor.
     */
    void ReadSensor(uint32_t pos, bool read_radiation_sensor);

    /**
     * Get a NTC temperature sensor
     * @param index The index of the sensor (0 to 3)
     * @return The TemperatureSensor pointer
     */
    TemperatureSensor * GetTemperatureSensor(uint32_t index);

    /**
     * Get a BJT radiation sensor
     * @param index The index of the sensor (0 to 3)
     * @return The RadiationSensor pointer
     */
    RadiationSensor * GetRadiationSensor(uint32_t index);

    /**
     * Prepare the trigger sequence for the scan.
     * @param delay The number of BCs between CAL and Trigger commands
     */
    void Trigger(uint32_t delay=0);

    /**
     * Get the next Hit from the FIFO
     * @return The next available Hit
     */
    Hit * GetHit();

    /**
     * Pop the next Hit from the FIFO and delete it.
     * @return True if there FIFO is still not empty.
     */
    bool NextHit();

    /**
     * Check if the FIFO of Hits has any hits.
     * @return True if there FIFO has hits.
     */
    bool HasHits();

    /**
     * Handle the data from the communication layer given by a byte array and its size.
     * The byte array will be decoded by the Decoder into a Record array,
     * and interpreted accordingly. The read-back values of the
     * @param recv_data Array of bytes from the communication layer.
     * @param recv_size Number of bytes in the byte array
     */
    void HandleData(uint8_t *recv_data, uint32_t recv_size);

    /**
     * Encode the commands stored in the Encoder in order to obtain the byte array
     * that has to be sent to the communication layer.
     * Calling this method is mandatory after performing any configuration, in order to populate the byte array.
     * The communication with the actual front-end is handled externally to this class.
     */
    void ProcessCommands();

    /**
     * Get the output byte stream from the emulator.
     * @return Byte stream output from the emulator.
     **/
    uint8_t * GetBytes();

    /**
     * Get the length of the output byte stream from the emulator.
     * @return Length of the byte stream output from the emulator.
     **/
    uint32_t GetLength();


  private:

    bool m_verbose;
    bool m_active;
    uint32_t m_chipid;
    std::string m_name;

    Decoder *m_decoder;
    Encoder *m_encoder;
    Configuration *m_config;
    Matrix *m_matrix;
    std::deque<Hit*> m_hits;
    std::mutex m_mutex;

    std::vector<TemperatureSensor*> m_ntcs;
    std::vector<RadiationSensor*> m_bjts;
  
  };

}
#endif
