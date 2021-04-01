#ifndef RD53A_CONFIGURATION_H
#define RD53A_CONFIGURATION_H

#include "RD53Emulator/Field.h"
#include "RD53Emulator/Register.h"

#include <iostream>
#include <map>
#include <vector>

namespace RD53A{

/**
 * The Configuration of the RD53A is contained in a Register table of 16-bits.
 * A Field is associated to a number of bits of a Register. 
 * The behavior of the RD53A is determined by the values of the registers.
 * The default value is the same one as on the real RD53A.
 * 
 * Each Register can be accessed (Configuration::GetRegister, Configuration::SetRegister),
 * and also each Field (Configuration::GetField). 
 * Note the index to access a given Field is given by Configuration::FieldType.
 *
 * Global register 0 (Configuration::PIX_PORTAL) is a virtual register to read and write
 * Pixel data. Each Pixel holds 8 or fewer bits of local configuration, and is addressed
 * as half of the 16-bit register value.
 * Multiple Pixels can be addressed at a time by the broadcast (Configuration::B_MASK),
 * and the mode (Configuration::PIX_MODE), column (Configuration::REGION_COL)
 * and row (Configuration::REGION_ROW).
 *
 * @brief RD53A Configuration
 * @author Carlos.Solans@cern.ch
 * @author Enrico.Junior.Schioppa@cern.ch
 * @date May 2020
 **/

class Configuration{
	
  public:

	enum FieldType{
	  PIX_PORTAL,         /**< Address  0, bits 0xF to 0x0 **/
	  REGION_COL,         /**< Address  1, bits 0x7 to 0x0 **/
	  REGION_ROW,         /**< Address  2, bits 0x8 to 0x0 **/
	  PIX_MODE,           /**< Address  3, bits 0x5 to 0x3 **/
	  PIX_BCAST_EN,       /**< Address  3, bits 0x5 **/
	  PIX_AUTO_COL,       /**< Address  3, bits 0x4 **/
	  PIX_AUTO_ROW,       /**< Address  3, bits 0x3 **/
	  PIX_BCAST_MASK,     /**< Address  3, bits 0x2 to 0x0 **/
	  PIX_DEFAULT_CONFIG, /**< Address  4, bits 0xF to 0x0 **/
	  IBIASP1_SYNC,       /**< Address  5, bits 0x8 to 0x0 **/
	  IBIASP2_SYNC,       /**< Address  6, bits 0x8 to 0x0 **/
	  IBIAS_SF_SYNC,      /**< Address  7, bits 0x8 to 0x0 **/
	  IBIAS_KRUM_SYNC,    /**< Address  8, bits 0x8 to 0x0 **/
	  IBIAS_DISC_SYNC,    /**< Address  9, bits 0x8 to 0x0 **/
	  ICTRL_SYNCT_SYNC,   /**< Address 10, bits 0x0 to 0x9 **/
	  VBL_SYNC,           /**< Address 11, bits 0x0 to 0x9 **/
	  VTH_SYNC,           /**< Address 12, bits 0x0 to 0x9 **/
	  VREF_KRUM_SYNC,     /**< Address 13, bits 0x0 to 0x9 **/
	  AUTO_ZERO,          /**< Address 30, bits 0x3 to 0x4 **/
	  SEL_C2F,            /**< Address 30, bits 0x2 **/
	  SEL_C4F,            /**< Address 30, bits 0x1 **/
	  FAST_TOT,           /**< Address 30, bits 0x0 **/
	  PA_IN_BIAS_LIN,     /**< Address 14, bits 0x8 to 0x0 **/
	  FC_BIAS_LIN,        /**< Address 15, bits 0x7 to 0x0 **/
	  KRUM_CURR_LIN,      /**< Address 16, bits 0x8 to 0x0 **/
	  LDAC_LIN,           /**< Address 17, bits 0x9 to 0x0 **/
	  COMP_LIN,           /**< Address 18, bits 0x8 to 0x0 **/
	  REF_KRUM_LIN,       /**< Address 19, bits 0x9 to 0x0 **/
	  VTH_LIN,            /**< Address 20, bits 0x9 to 0x0 **/
	  PRMP_DIFF,          /**< Address 21, bits 0x9 to 0x0 **/
	  FOL_DIFF,           /**< Address 22, bits 0x8 to 0x0 **/
	  PRECOMP_DIFF,       /**< Address 23, bits 0x8 to 0x0 **/
	  COMP_DIFF,          /**< Address 24, bits 0x8 to 0x0 **/
	  VFF_DIFF,           /**< Address 25, bits 0x8 to 0x0 **/
	  VTH1_DIFF,          /**< Address 26, bits 0x8 to 0x0 **/
	  VTH2_DIFF,          /**< Address 27, bits 0x8 to 0x0 **/
	  LCC_DIFF,           /**< Address 28, bits 0x8 to 0x0 **/
	  LCC_EN,             /**< Address 29, bits 0x1 **/
	  DIFF_FB_CAP_EN,     /**< Address 29, bits 0x0 **/
	  SLDO_ANALOG_TRIM,   /**< Address 31, bits 0x9 to 0x5 **/
	  SLDO_DIGITAL_TRIM,  /**< Address 31, bits 0x4 to 0x0 **/
	  EN_CORE_COL_SYNC,   /**< Address 32, bits 0xF to 0x0 **/
	  EN_CORE_COL_LIN_1,  /**< Address 33, bits 0xF to 0x0 **/
	  EN_CORE_COL_LIN_2,  /**< Address 34, bits 0x0 **/
	  EN_CORE_COL_DIFF_1, /**< Address 35, bits 0xF to 0x0 **/
	  EN_CORE_COL_DIFF_2, /**< Address 36, bits 0x0 **/
	  LATENCY_CONFIG,     /**< Address 37, bits 0x8 to 0x0 **/
	  WR_SYNC_DELAY_SYNC, /**< Address 38, bits 0x4 to 0x0 **/
	  INJ_MODE_ANA,       /**< Address 39, bits 0x5 **/
	  INJ_MODE_DIG,       /**< Address 39, bits 0x4 **/
	  INJ_FINE_DELAY,     /**< Address 39, bits 0x3 to 0x0 **/
	  VCAL_HIGH,          /**< Address 41, bits 0xB to 0x0 **/
	  VCAL_MED,           /**< Address 42, bits 0xB to 0x0 **/
	  CAL_COLPR_SYNC_1,   /**< Address 46, bits 0xF to 0x0 **/
	  CAL_COLPR_SYNC_2,   /**< Address 47, bits 0xF to 0x0 **/
	  CAL_COLPR_SYNC_3,   /**< Address 48, bits 0xF to 0x0 **/
	  CAL_COLPR_SYNC_4,   /**< Address 49, bits 0xF to 0x0 **/
	  CAL_COLPR_LIN_1,    /**< Address 50, bits 0xF to 0x0 **/
	  CAL_COLPR_LIN_2,    /**< Address 51, bits 0xF to 0x0 **/
	  CAL_COLPR_LIN_3,    /**< Address 52, bits 0xF to 0x0 **/
	  CAL_COLPR_LIN_4,    /**< Address 53, bits 0xF to 0x0 **/
	  CAL_COLPR_LIN_5,    /**< Address 54, bits 0x3 to 0x0 **/
    CAL_COLPR_DIFF_1,   /**< Address 55, bits 0xF to 0x0 **/
    CAL_COLPR_DIFF_2,   /**< Address 56, bits 0xF to 0x0 **/
    CAL_COLPR_DIFF_3,   /**< Address 57, bits 0xF to 0x0 **/
    CAL_COLPR_DIFF_4,   /**< Address 58, bits 0xF to 0x0 **/
    CAL_COLPR_DIFF_5,   /**< Address 59, bits 0x3 to 0x0 **/
    CLK_DATA_DELAY,     /**< Address 40, bits 0x8 to 0x0 **/
    CLK_DELAY,          /**< Address 40, bits 0x8 **/
    CLK_DELAY_SEL,      /**< Address 40, bits 0x7 to 0x4 **/
    CMD_DELAY,          /**< Address 40, bits 0x3 to 0x0 **/
    CH_SYNC_CONF,       /**< Address 43, bits 0xB to 0x0 **/
    CH_SYNC_PHASE,      /**< Address 43, bits 0xB to 0xA **/
    CH_SYNC_LOCK,       /**< Address 43, bits 0x9 to 0x5 **/
    CH_SYNC_UNLOCK,     /**< Address 43, bits 0x4 to 0x0 **/
    GLOBAL_PULSE_RT,    /**< Address 44, bits 0xF to 0x0 **/
    DEBUG_CONFIG,       /**< Address 60, bits 0x1 to 0x0 **/
    ENABLE_EXT_CAL,     /**< Address 60, bits 0x1 **/
    OUT_DATA_DELAY,     /**< Address 61, bits 0x8 to 0x7 **/
    OUT_SERIAL_TYPE,    /**< Address 61, bits 0x6 **/
    OUT_ACTIVE_LANES,   /**< Address 61, bits 0x5 to 0x2 **/
    OUT_FORMAT,         /**< Address 61, bits 0x1 to 0x0 **/
    OUT_PAD_CONFIG,     /**< Address 62, bits 0xD to 0x0 **/
    GP_LVDS_ROUTE,      /**< Address 63, bits 0x2 to 0x0 **/
    CDR_CONFIG,         /**< Address 64, bits 0xD to 0x0 **/
    VCO_BUFF_BIAS,      /**< Address 65, bits 0x9 to 0x0 **/
    CDR_CP_IBIAS,       /**< Address 66, bits 0x9 to 0x0 **/
    VCO_IBIAS,          /**< Address 67, bits 0xD to 0x0 **/
    SER_SEL_OUT,        /**< Address 68, bits 0x7 to 0x0 **/
    SER_SEL_OUT_3,      /**< Address 68, bits 0x7 to 0x6 **/
    SER_SEL_OUT_2,      /**< Address 68, bits 0x5 to 0x4 **/
    SER_SEL_OUT_1,      /**< Address 68, bits 0x3 to 0x2 **/
    SER_SEL_OUT_0,      /**< Address 68, bits 0x1 to 0x0 **/
    CML_CONFIG,         /**< Address 69, bits 0x7 to 0x0 **/
    CML_INV_TAP,        /**< Address 69, bits 0x7 to 0x6 **/
    CML_EN_TAP,         /**< Address 69, bits 0x5 to 0x4 **/
    CML_EN,             /**< Address 69, bits 0x3 to 0x0 **/
    CML_TAP_BIAS_1,     /**< Address 70, bits 0x9 to 0x0 **/
    CML_TAP_BIAS_2,     /**< Address 71, bits 0x9 to 0x0 **/
    CML_TAP_BIAS_3,     /**< Address 72, bits 0x9 to 0x0 **/
    AURORA_CC_CFG,      /**< Address 73, bits 0x7 to 0x0 **/
    AURORA_CC_WAIT,     /**< Address 73, bits 0x7 to 0x2 **/
    AURORA_CC_SEND,     /**< Address 73, bits 0x1 to 0x0 **/
    AURORA_CB_CFG_1,    /**< Address 74, bits 0x7 to 0x0 **/
    AURORA_CB_WAIT_1,   /**< Address 74, bits 0x7 to 0x4 **/
    AURORA_CB_SEND,     /**< Address 74, bits 0x3 to 0x0 **/
    AURORA_CB_CFG_0,    /**< Address 75, bits 0xF to 0x0 **/
    AURORA_CB_WAIT_0,   /**< Address 75, bits 0xF to 0x0 **/
    AURORA_INIT_WAIT,   /**< Address 76, bits 0xA to 0x0 **/
    MON_FRAME_SKIP,     /**< Address 45, bits 0x7 to 0x0 **/
    AUTO_READ_A0,       /**< Address 101, bits 0x8 to 0x0 **/
    AUTO_READ_B0,       /**< Address 102, bits 0x8 to 0x0 **/
    AUTO_READ_A1,       /**< Address 103, bits 0x8 to 0x0 **/
    AUTO_READ_B1,       /**< Address 104, bits 0x8 to 0x0 **/
    AUTO_READ_A2,       /**< Address 105, bits 0x8 to 0x0 **/
    AUTO_READ_B2,       /**< Address 106, bits 0x8 to 0x0 **/
    AUTO_READ_A3,       /**< Address 107, bits 0x8 to 0x0 **/
    AUTO_READ_B3,       /**< Address 108, bits 0x8 to 0x0 **/
    MONITOR_MUX,        /**< Address 77, bits 0xD to 0x0 **/
    MONITOR_EN,         /**< Address 77, bits 0xD **/
    MONITOR_IMON_MUX,   /**< Address 77, bits 0xC to 0x5 **/
    MONITOR_VMON_MUX,   /**< Address 77, bits 0x6 to 0x0 **/
    HITOR_MASK_SYNC_1,  /**< Address 78, bits 0xF to 0x0 **/
    HITOR_MASK_SYNC_2,  /**< Address 79, bits 0xF to 0x0 **/
    HITOR_MASK_SYNC_3,  /**< Address 80, bits 0xF to 0x0 **/
    HITOR_MASK_SYNC_4,  /**< Address 81, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_11,  /**< Address 82, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_12,  /**< Address 83, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_21,  /**< Address 84, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_22,  /**< Address 85, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_31,  /**< Address 86, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_32,  /**< Address 87, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_41,  /**< Address 88, bits 0xF to 0x0 **/
    HITOR_MASK_LIN_42,  /**< Address 89, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_11, /**< Address 90, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_12, /**< Address 91, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_21, /**< Address 92, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_22, /**< Address 93, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_31, /**< Address 94, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_32, /**< Address 95, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_41, /**< Address 96, bits 0xF to 0x0 **/
    HITOR_MASK_DIFF_42, /**< Address 97, bits 0xF to 0x0 **/
    ADC_CONFIG,         /**< Address 98, bits 0xA to 0x0 **/
    BANDGAP_TRIM,       /**< Address 98, bits 0xA to 0x6 **/
    ADC_TRIM,           /**< Address 98, bits 0x0 to 0x5 **/
    SENSOR_CONFIG_0,    /**< Address 99, bits 0xB to 0x0 **/
    SENSOR_BIAS_0,      /**< Address 99, bits 0x0 **/
    SENSOR_CURRENT_0,   /**< Address 99, bits 0x4 to 0x1 **/
    SENSOR_ENABLE_0,    /**< Address 99, bits 0x5 **/
    SENSOR_BIAS_1,      /**< Address 99, bits 0x6 **/
    SENSOR_CURRENT_1,   /**< Address 99, bits 0xA to 0x7 **/
    SENSOR_ENABLE_1,    /**< Address 99, bits 0xB **/
    SENSOR_CONFIG_1,    /**< Address 100, bits 0xB to 0x0 **/
    SENSOR_BIAS_2,      /**< Address 100, bits 0x0 **/
    SENSOR_CURRENT_2,   /**< Address 100, bits 0x4 to 0x1 **/
    SENSOR_ENABLE_2,    /**< Address 100, bits 0x5 **/
    SENSOR_BIAS_3,      /**< Address 100, bits 0x6 **/
    SENSOR_CURRENT_3,   /**< Address 100, bits 0xA to 0x7 **/
    SENSOR_ENABLE_3,    /**< Address 100, bits 0xB **/
    RING_OSC_ENABLE,    /**< Address 109, bits 0xB to 0x0 **/
    RING_OSC_1,         /**< Address 110, bits 0xF to 0x0 **/
    RING_OSC_2,         /**< Address 111, bits 0xF to 0x0 **/
    RING_OSC_3,         /**< Address 112, bits 0xF to 0x0 **/
    RING_OSC_4,         /**< Address 113, bits 0xF to 0x0 **/
    RING_OSC_5,         /**< Address 114, bits 0xF to 0x0 **/
    RING_OSC_6,         /**< Address 115, bits 0xF to 0x0 **/
    RING_OSC_7,         /**< Address 116, bits 0xF to 0x0 **/
    RING_OSC_8,         /**< Address 117, bits 0xF to 0x0 **/
    BC_CTR,             /**< Address 118, bits 0xF to 0x0 **/
    TRIG_CTR,           /**< Address 119, bits 0xF to 0x0 **/
    LCK_LOSS_CTR,       /**< Address 120, bits 0xF to 0x0 **/
    BFLIP_WARN_CTR,     /**< Address 121, bits 0xF to 0x0 **/
    BFLIP_ERR_CTR,      /**< Address 122, bits 0xF to 0x0 **/
    CMD_ERR_CTR,        /**< Address 123, bits 0xF to 0x0 **/
    FIFO_FULL_CTR_1,    /**< Address 124, bits 0xF to 0x8 **/
    FIFO_FULL_CTR_2,    /**< Address 124, bits 0x7 to 0x0 **/
    FIFO_FULL_CTR_3,    /**< Address 125, bits 0xF to 0x8 **/
    FIFO_FULL_CTR_4,    /**< Address 125, bits 0x7 to 0x0 **/
    FIFO_FULL_CTR_5,    /**< Address 126, bits 0xF to 0x8 **/
    FIFO_FULL_CTR_6,    /**< Address 126, bits 0x7 to 0x0 **/
    FIFO_FULL_CTR_7,    /**< Address 127, bits 0xF to 0x8 **/
    FIFO_FULL_CTR_8,    /**< Address 127, bits 0x7 to 0x0 **/
    AI_PIX_COL,         /**< Address 128, bits 0x7 to 0x0 **/
    AI_PIX_ROW,         /**< Address 129, bits 0x8 to 0x0 **/
    HITOR_CNT_1,        /**< Address 130, bits 0xF to 0x0 **/
    HITOR_CNT_2,        /**< Address 131, bits 0xF to 0x0 **/
    HITOR_CNT_3,        /**< Address 132, bits 0xF to 0x0 **/
    HITOR_CNT_4,        /**< Address 133, bits 0xF to 0x0 **/
    SKP_TRIG_CNT,       /**< Address 134, bits 0xF to 0x0 **/
    ERR_MASK,           /**< Address 135, bits 0xD to 0x0 **/
    ADC_READ,           /**< Address 136, bits 0xB to 0x0 **/
    SELF_TRIG_EN,       /**< Address 137, bits 0xF to 0x0 **/
    NONE
	};

	enum GlobalPulseRoutes{
	  GP_RESET_CHN,        /**< Reset channel synchronizer **/
	  GP_RESET_CMD,        /**< Reset command decoder **/
	  GP_RESET_CONFIG,     /**< Reset global configuration **/
	  GP_RESET_MON,        /**< Clear monitoring data **/
	  GP_RESET_AURORA,     /**< Reset aurora and start channel bonding sequence **/
	  GP_RESET_SERILIZER,  /**< Reset serializer **/
	  GP_RESET_ADC,        /**< Reset ADC **/
	  GP_7,                /**< Unused **/
	  GP_START_MON,        /**< Start monitoring **/
	  GP_9,                /**< Unused **/
	  GP_10,               /**< Unused **/
	  GP_11,               /**< Unused **/
	  GP_START_ADC,        /**< Start the ADC **/
	  GP_START_RING_OSC,   /**< Activate the ring oscillators **/
	  GP_START_ZERO_LEVEL, /**< Start the zero leve in Synchronous front-end **/
	  GP_RESET_ZERO_LEVEL  /**< Reset auto zeroing in Synchronous front-end **/
	};

	static const uint32_t VMUX_TEMP_1 = 3;
	static const uint32_t VMUX_TEMP_2 = 5;
	static const uint32_t VMUX_TEMP_3 = 15;
	static const uint32_t VMUX_TEMP_4 = 7;
	static const uint32_t VMUX_RAD_1 = 4;
  static const uint32_t VMUX_RAD_2 = 6;
  static const uint32_t VMUX_RAD_3 = 14;
  static const uint32_t VMUX_RAD_4 = 8;

  /**
   * Create a new configuration object. Initialize the fields to their default values.
   **/
  Configuration();

  /**
   * Clear internal arrays
   **/
  ~Configuration();

  /**                                                                                                                                                                                   
   * Enable the verbose mode                                                                                                                                                            
   * @param enable Enable the verbose mode if true                                                                                                                                      
   */
  void SetVerbose(bool enable);

  /**
   * Get the number of registers in the configuration
   * @return The configuration size
   **/
  uint32_t Size();

  /**
   * Get the whole 16-bit value of a register
   * @param index Register index
   * @return The 16-bit register value
   **/
  uint16_t GetRegister(uint32_t index);

  /**
   * Set the whole 16-bit value of a register
   * @param index Register index
   * @param value The 16-bit register value
   **/
  void SetRegister(uint32_t index, uint16_t value);

  /**
   * Set the Field value from a string. Note it is not necessarily the whole register
   * @param name Field name
   * @param value The new value
   **/
  void SetField(std::string name, uint16_t value);

  /**
   * Set the Field value from a string. Note it is not necessarily the whole register
   * @param index Field index
   * @param value The new value
   **/
  void SetField(uint32_t index, uint16_t value);

  /**
   * Get a Field inside the Configuration given
   * its Field index (ie: Configuration::GateHitOr).
   * @param index The Field index (Configuration::FieldType)
   * @return pointer to the field
   **/
  Field * GetField(uint32_t index);

  /**
   * Get a Field inside the Configuration by name
   * @param name The Field name (Configuration::m_name2index)
   * @return pointer to the field
   **/
  Field * GetField(std::string name);

  /**
   * Get list of addresses that have been updated
   * @return vector of addresses that have been updated
   **/
  std::vector<uint32_t> GetUpdatedRegisters();

  /**
   * Get map of addresses
   * @return map of strings to addresses
   **/
  std::map<std::string,uint32_t> GetRegisters();

  /**
   * Get map of addresses
   * @return map of strings to addresses
   **/
  void SetRegisters(std::map<std::string,uint32_t> config);

 private:

  bool m_verbose;
  std::vector<Register> m_registers;
  std::map<uint32_t, Field*> m_fields;
  static std::map<std::string, uint32_t> m_name2index;
  std::vector<std::string> m_names_yarr;
  std::vector<std::string> m_names_rce;
  
  };

}

#endif
