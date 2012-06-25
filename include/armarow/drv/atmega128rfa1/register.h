#pragma once

#include <stdint.h>
#include <avr-halib/regmaps/base/localRegMap.h>
#include "spec.h"

namespace avr_halib
{
namespace regmaps
{
namespace local
{
namespace atmega128rfa1
{

/** \brief Regmap for radio framebuffer **/
class Registers : public base::LocalRegMap
{
enum AESModes
{
	ecb = 0,
	cbc = 1
};
enum AESdirections
{
	encrypt = 0,
	decrypt = 1
};

public:
	union
	{
		struct
		{
			uint8_t __pad_trxpr[0x139];
			union
			{
				struct{ 
					bool reset :1;
					bool sleep :1;
				};
				uint8_t trxpr;
			};
		};
		struct
		{
			uint8_t __pad_aes[0x13c];
			union
			{
				struct
				{
					uint8_t :2;
					bool aes_interrupt :1;
					bool aes_direction :1;
					uint8_t :1;
					bool aes_mode :1;
					uint8_t :1;
					bool aes_request :1;
				};
				uint8_t aes_ctrl;
			};
			union
			{
				struct{
					bool aes_done :1;
					uint8_t :6;
					bool aes_error :1;
				};
				uint8_t aes_status;
			};
			uint8_t aes_state;
			uint8_t aes_key;
		};
		struct
		{
			uint8_t __pad_trx_status[0x141];
			union{
				struct{
					uint8_t trx_status  : 5;
					bool    tst_status  : 1;
					bool    cca_status  : 1;
					bool    cca_done    : 1;
				};
				uint8_t trx_status_r;
			};
			union
			{
				struct
				{
					uint8_t trx_cmd     : 5;
					uint8_t trac_status : 3;
				};
				uint8_t trx_state;
			};
		};
		struct
		{
			uint8_t __pad_trx_ctrl1[0x144];
			union{
				struct{
					uint8_t :5;
					bool tx_auto_crc_on :1;
					bool irq2_ext_enable :1;
					bool pa_ext_enable :1;
				};
				uint8_t trx_ctrl1;
			};
		};
		struct
		{
			uint8_t __pad_phy_tx_pwr[0x145];
			union{
				struct{
					uint8_t tx_power :4;
					uint8_t pa_leadtime :2;
					uint8_t pa_bufferd_leadtime :2;
				};
				uint8_t phy_tx_pwr;
			};
		};
		struct
		{
			uint8_t __pad_phy_rssi[0x146];
			union{
				struct{
					uint8_t tx_rssi :5;
					uint8_t random :2;
					bool rx_crc_valid :1;
				};
				uint8_t phy_rssi;
			};
		};
		struct
		{
			uint8_t __pad_phy_ed_level[0x147];
			uint8_t phy_ed_level;
		};
		struct
		{
			uint8_t __pad_phy_cca[0x148];
			union{
				struct{
                    uint8_t channel  :5;
					uint8_t cca_mode :2;
					bool cca_request :1;
				};
				uint8_t phy_cc_cca;
			};
			uint8_t cca_threshold :4;
            uint8_t               :4;
		};
		struct
		{
			uint8_t __pad_rx_ctrl[0x14a];
			union{
				struct{
					uint8_t pdt_threshold :4;
				};
				uint8_t rx_ctrl;
			};
		};
		struct
		{
			uint8_t __pad_sfd_value[0x14b];
			uint8_t sfd_value;
		};
		struct
		{
			uint8_t __pad_trx_ctrl2[0x14c];
			union{
				struct{
					uint8_t oqpsk_data_rate :2;
					uint8_t :5;
					bool rx_safe_mode :1;
				};
				uint8_t trx_ctrl2;
			};
		};
		struct
		{
			uint8_t __pad_ant_div[0x14d];
			union{
				struct{
					uint8_t ant_ctrl :2;
					bool ant_ext_switch_enable :1;
					bool ant_div_enable :1;
					uint8_t :3;
					bool ant_sellect :1;
				};
				uint8_t ant_div;
			};
		};
		struct
		{
			uint8_t __pad_irq_mask[0x14e];
			union{
				struct{
					bool pllLock :1;
					bool pllUnlock :1;
					bool rxStart :1;
					bool rxEnd :1;

					bool ccaEdDone :1;
					bool ami :1;
					bool txEnd :1;
					bool awake :1;
				};
				uint8_t value;
			} irqMask;
		};
		struct
		{
			uint8_t __pad_irq_status[0x14f];
			union{
				struct{
					bool pllLock :1;
					bool pllUnlock :1;
					bool rxStart :1;
					bool rxEnd :1;
					bool ccaEdDone :1;
					bool ami :1;
					bool txEnd :1;
					bool awake :1;
				};
				uint8_t value;
			} irqStatus;
		};
		struct
		{
			uint8_t __pad_vreg_ctrl[0x150];
			union{
				struct{
					uint8_t :2;
					bool dvdd_ok :1;
					uint8_t :3;
					bool avdd_ok :1;
					bool avreg_ext :1;
				};
				uint8_t vreg_ctrl;
			};
		};
		struct
		{
			uint8_t __pad_batmon[0x151];
			union{
				struct{
					uint8_t batmon_voltag_threshold :4; //if not high_range then (1.7V + value *0.05V) else (2.550V + value *0.075V)
					bool batmon_high_range :1;
					bool batmon_ok :1;
					bool batmon_low_enable :1;
					bool batmon_low :1;
				};
				uint8_t batmon;
			};
		};
		struct
		{
			uint8_t __pad_xosc_ctrl[0x152];
			union{
				struct{
					uint8_t xtal_trim :4;
					uint8_t xtal_mode :4;
				};
				uint8_t xosc_ctrl;
			};
		};
		struct
		{
			uint8_t __pad_rx_syn[0x155];
			union{
				struct{
					uint8_t rx_pdt_level :4;
					uint8_t :3;
					bool rx_pdt_dis :1;
				};
				uint8_t rx_syn;
			};
		};
		struct
		{
			uint8_t __pad_xah_ctrl_1[0x157];
			union{
				struct{
					bool :1;
					bool aack_prom_mode :1;
					bool reduce_aack_time :1;
					bool :1;
					bool aack_process_reserved_frames :1;
					bool aack_filter_reserved_frames :1;
					
				};
				uint8_t xah_ctrl1;
			};
		};
		struct
		{
			uint8_t __pad_ftn_ctrl[0x158];
			union{
				struct{
					uint8_t :7;
					bool filter_tuning_network_start :1;
					
				};
				uint8_t ftn_ctrl;
			};
		};
		struct
		{
			uint8_t __pad_pll_cf[0x15a];
			union{
				struct{
					uint8_t :7;
					bool pll_center_frequency_callibration_start :1;
					
				};
				uint8_t pll_cf;
			};
		};
		struct
		{
			uint8_t __pad_pll_dcu[0x15b];
			union{
				struct{
					uint8_t :7;
					bool pll_delay_cell_calibration_start :1;
					
				};
				uint8_t pll_dcu;
			};
		};
		struct
		{
			uint8_t __pad_part_num[0x15c];
			uint8_t part_number;
		};
		struct
		{
			uint8_t __pad_version_num[0x15d];
			uint8_t version_number;
		};
		struct
		{
			uint8_t __pad_man_id[0x15e];
			uint16_t man_id;
		};
		struct
		{
			uint8_t __pad_short_addr[0x160];
			uint16_t short_address;
		};
		struct
		{
			uint8_t __pad_pan_id[0x162];
			uint16_t pan_id;
		};
		struct
		{
			uint8_t __pad_ieee_addr[0x164];
			uint64_t ieee_adress;
		};
		struct
		{
			uint8_t __pad_xah_ctrl_0[0x16c];
			union{
				struct{
					bool slotted_opperation :1;
					uint8_t max_csma_retries :3; // from 0 upto 5, never 6, 7 = no csma-ca
					uint8_t max_frame_retries :4;
				};
				uint8_t xah_ctrl0;
			};
		};
		struct
		{
			uint8_t __pad_csma_seed[0x16d];
			union{
				struct{
					uint16_t csma_seed :10;
					bool i_am_pan_coordinator :1;
					bool aack_dissable :1;
					bool aack_set_pending :1;
					uint8_t aack_frame_version :2;
				};
				struct{
					uint8_t csma_seed0;
					uint8_t csma_seed1;
				}; 
				uint16_t csma_seed_full;
			};
		};
		struct
		{
			uint8_t __pad_csma_be[0x16f];
			union{
				struct{
					uint8_t csma_min_backoff_exponent :4; //lower than or same as max_backoff_exponent
					uint8_t csma_max_backoff_exponent :4; //from 3 up to 8
				};
				uint8_t csma_be;
			};
		};
		struct
		{
			uint8_t __pad_tst_ctrl_digi[0x176];
			union{
				struct{
					uint8_t tst_run :4; // 0 or 15 15=continous transmit
				};
				uint8_t tst_ctrl_digi;
			};
		};
		
		struct
		{
			uint8_t __padrxlength[0x17b];
			uint8_t rxlength;
		};
	};
}__attribute__((packed));
}
}
}
}
