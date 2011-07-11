#pragma once


    //template <class _controller_configuration = definecontroller> 
    class RC
    {
        public:
union {
    //---------------------------------------------------------------
    struct { _offset : 0x139*8;
        union {
            uint8_t trxpr;                  // (0x139)
            struct {
                bool    trxrst              : 1;
                bool    slptr               : 1;
                uint8_t                     : 6;
            };
        };
    };
    //---------------------------------------------------------------
    struct { _offset : 0x13c*8;
        union {
            uint8_t aesCtrl;                // (0x13c)
            struct {
                uint8_t                     : 2;
                bool    aes_im              : 1;
                bool    aes_dir             : 1;
                bool                        : 1;
                bool    aes_mode            : 1;
                bool                        : 1;
                bool    aes_request         : 1;
            };
        };
        union {
            uint8_t aesStatus;              // (0x13d)
            struct {
                bool    aes_done            : 1;
                uint8_t                     : 6;
                bool    aes_er              : 1;
            };
        };
        uint8_t aesState;                   // (0x13e)
        uint8_t aesKey;                     // (0x13f)
    };
    //---------------------------------------------------------------
    struct { _offset : 0x141*8;
        union {
            uint8_t trxStatus;             // (0x141)
            struct {
                uint8_t trx_status          : 5;
                bool    tst_status          : 1;
                bool    cca_status          : 1;
                bool    cca_done            : 1;
            };
        };
        union {
            uint8_t trxState;              // (0x142)
            struct {
                uint8_t trx_cmd             : 5;
                uint8_t trac_status         : 3;
            };
        };
        uint8_t trxCtrl0;                 // (0x143)
        union {
            uint8_t trxCtrl1;             // (0x144)
            struct {
                uint8_t                     : 5;
                bool    tx_auto_crc_on      : 1;
                bool    irq_2_ext_en        : 1;
                bool    pa_ext_en           : 1;
            };
        };
        union {
            uint8_t phyTxPwr;             // (0x145)
            struct {
                uint8_t tx_pwr              : 4;
                uint8_t pa_lt               : 2;
                uint8_t pa_buf              : 2;
            };
        };
        union {
            uint8_t phyRssi;               // (0x146)
            struct {
                uint8_t rssi                : 5;
                uint8_t rnd_value           : 2;
                bool    rx_crc_valid        : 1;
            };
        };
        uint8_t phyEdLevel;               // (0x147)
        union {
            uint8_t phyCcCca;             // (0x148)
            struct {
                uint8_t channel             : 5;
                uint8_t cca_mode            : 2;
                bool    cca_request         : 1;
            };
        };
        union {
            uint8_t ccaThres;              // (0x149)
            struct {
                uint8_t cca_ed_thres        : 4;
                uint8_t cca_cs_thres        : 4;
            };
        };
        union {
            uint8_t rxCtrl;                // (0x14a)
            struct {
                uint8_t pdt_thres           : 4;
                uint8_t                     : 4;
            };
        };
        uint8_t sfdValue;                  // (0x14b)
        union {
            uint8_t trxCtrl2;              // (0x14c)
            struct {
                uint8_t oqpsk_data_rate     : 2;
                uint8_t                     : 5;
                bool    rx_safe_mode        : 1;
            };
        };
        union {
            uint8_t antDiv;                // (0x14d)
            struct {
                uint8_t ant_ctrl            : 2;
                bool    ant_ext_sw_en       : 1;
                bool    ant_div_en          : 1;
                uint8_t                     : 3;
                bool    ant_sel             : 1;
            };
        };
        union {
            uint8_t irqMask;               // (0x14e)
            struct {
                bool    pll_lock_en         : 1;
                bool    pll_unlock_en       : 1;
                bool    rx_start_en         : 1;
                bool    rx_end_en           : 1;
                bool    cca_ed_done_en      : 1;
                bool    ami_en              : 1;
                bool    tx_end_en           : 1;
                bool    awaken_en           : 1;
            };
        };
        union {
            uint8_t irqStatus;             // (0x14f)
            struct {
                bool    pll_lock            : 1;
                bool    pll_unlock          : 1;
                bool    rx_start            : 1;
                bool    rx_end              : 1;
                bool    cca_ed_done         : 1;
                bool    ami                 : 1;
                bool    tx_end              : 1;
                bool    awaken              : 1;
            };
        };
        union {
            uint8_t vregCtrl;              // (0x150)
            struct {
                uint8_t                     : 2;
                bool    dvdd_ok             : 1;
                uint8_t                     : 3;
                bool    avdd_ok             : 1;
                bool    avreg_ext           : 1;
            };
        };
        union {
            uint8_t batMon;                 // (0x151)
            struct {
                uint8_t batmon_vth          : 4;
                bool    batmon_hr           : 1;
                bool    batmon_ok           : 1;
                bool    bat_low_en          : 1;
                bool    bat_low             : 1;
            };
        };
        union {
            uint8_t xoscCtrl;              // (0x152)
            struct {
                uint8_t xtal_trim           : 4;
                uint8_t xtal_mode           : 4;
            };
        };
    };
    //---------------------------------------------------------------
    struct { _offset : 0x155*8;
        union {
            uint8_t rxSyn;                 // (0x155)
            struct {
                uint8_t rx_pdt_level        : 4;
                uint8_t                     : 3;
                bool    rx_pdt_dis          : 1;
            };
        };
    };
    //---------------------------------------------------------------
    struct { _offset : 0x157*8;
        union {
            uint8_t xahCtrl1;               // (0x157)
            struct {
                bool                        : 1;
                bool    aack_prom_mode      : 1;
                bool    aack_ack_time       : 1;
                bool                        : 1;
                bool    aack_upld_res_ft    : 1;
                bool    aack_fltr_res_ft    : 1;
                uint8_t                     : 2;
            };
        };
        union {
            uint8_t ftnCtrl;                // (0x158)
            struct {
                uint8_t                     : 7;
                bool    ftn_start           : 1;
            };
        };
    };
    //---------------------------------------------------------------
    struct { _offset : 0x15a*8;
        union {
            uint8_t pllCf;                  // (0x15a)
            struct {
                uint8_t                     : 7;
                bool    pll_cf_start        : 1;
            };
        };
        union {
            uint8_t pllDcu;                 // (0x15b)
            struct {
                uint8_t                     : 7;
                bool    pll_dcu_start       : 1;
            };
        };
        uint8_t partNum;                    // (0x15c)
        uint8_t versionNum;                 // (0x15d)
        uint8_t manId0;                     // (0x15e)
        uint8_t manId1;                     // (0x15f)
        uint8_t shortAddr0;                 // (0x160)
        uint8_t shortAddr1;                 // (0x161)
        uint8_t panId0;                     // (0x162)
        uint8_t panId1;                     // (0x163)
        uint8_t ieeeAddr0;                  // (0x164)
        uint8_t ieeeAddr1;                  // (0x165)
        uint8_t ieeeAddr2;                  // (0x166)
        uint8_t ieeeAddr3;                  // (0x167)
        uint8_t ieeeAddr4;                  // (0x168)
        uint8_t ieeeAddr5;                  // (0x169)
        uint8_t ieeeAddr6;                  // (0x16a)
        uint8_t ieeeAddr7;                  // (0x16b)
        union {
            uint8_t xahCtrl0;             // (0x16c)
            struct {
                bool    slotted_operation   : 1;
                uint8_t max_csma_retries    : 3;
                uint8_t max_frame_retries   : 4;
            };
        };
        uint8_t csmaSeed0;                  // (0x16d)
        union {
            uint8_t csmaSeed1;              // (0x16e)
            struct {
                uint8_t csma_seed_1         : 3;
                bool    aack_i_am_coord     : 1;
                bool    aack_dis_ack        : 1;
                bool    aack_set_pd         : 1;
                uint8_t aack_fvn_mode       : 2;
            };
        };
        union {
            uint8_t csmaBe;                 // (0x16f)
            struct {
                uint8_t min_be              : 4;
                uint8_t max_be              : 4;
            };
        };
    };
    //---------------------------------------------------------------
    struct { _offset : 0x176*8;
        union {
            uint8_t tstCtrlDigi;            // (0x176)
            struct {
                uint8_t tst_ctrl_dig        : 4;
                uint8_t                     : 4;
            };
        };
    };
    struct { _offset : 0x17b*8;
        uint8_t tstRxLength;                // (0x17b)
    };
    struct { _offset : 0x180*8;
        uint8_t trxfbst;                    // (0x180)
    };
    struct { _offset : 0x180*8;
        uint8_t trxFifo[128];               // (0x180)
    };
    struct { _offset : 0x1ff*8;
        uint8_t trxfbend;                   // (0x1ff)
    };
};// ==========================================================================
    }__attribute__((packed));
