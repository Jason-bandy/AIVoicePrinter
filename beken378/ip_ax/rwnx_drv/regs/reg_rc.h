#ifndef _REG_RC_H_
#define _REG_RC_H_

/*************************************************************************/
/* memory mapping for modules                                            */
/*************************************************************************/
#define RC_BASE                                                 0x0100C000
#define TRX_BASE                                                0x0100C200
#define POWTBL_BASE                                             0x0100c400
#define DPDTBL_BASE                                             0x0100c800
#define LDPCRXCFGMEM_BASE                                       0x01009000
#define AGCMEM_BASE                                             0x0100A000
#define PEAKCWMEM_BASE                                          0x0100D000

/*************************************************************************/
/* register structure for modules                                        */
/*************************************************************************/
/* RC */
typedef struct
{
    volatile unsigned int REG0X0                                ;
    volatile unsigned int REG0X1                                ;
    volatile unsigned int REG0X2                                ;
    volatile unsigned int REG0X3                                ;
    volatile unsigned int REG0X4                                ;
    volatile unsigned int REG0X5                                ;
    volatile unsigned int REG0X6                                ;
    volatile unsigned int REG0X7                                ;
    volatile unsigned int REG0X8                                ;
    volatile unsigned int REG0X9                                ;
    volatile unsigned int REG0XA                                ;
    volatile unsigned int REG0XB                                ;
    volatile unsigned int REG0XC                                ;
    volatile unsigned int REG0XD                                ;
    volatile unsigned int REG0XE                                ;
    volatile unsigned int REG0XF                                ;
    volatile unsigned int REG0X10                               ;
    volatile unsigned int REG0X11                               ;
    volatile unsigned int REG0X12                               ;
    volatile unsigned int REG0X13                               ;
    volatile unsigned int REG0X14                               ;
    volatile unsigned int REG0X15                               ;
    volatile unsigned int REG0X16                               ;
    volatile unsigned int REG0X17                               ;
    volatile unsigned int REG0X18                               ;
    volatile unsigned int REG0X19                               ;
    volatile unsigned int REG0X1A                               ;
    volatile unsigned int REG0X1B                               ;
    volatile unsigned int REG0X1C                               ;
             unsigned int RES0X1D_To_0X1E[2]                    ;
    volatile unsigned int REG0X1F                               ;
    volatile unsigned int REG0X20                               ;
    volatile unsigned int REG0X21                               ;
    volatile unsigned int REG0X22                               ;
    volatile unsigned int REG0X23                               ;
    volatile unsigned int REG0X24                               ;
    volatile unsigned int REG0X25                               ;
    volatile unsigned int REG0X26                               ;
    volatile unsigned int REG0X27                               ;
    volatile unsigned int REG0X28                               ;
    volatile unsigned int REG0X29                               ;
    volatile unsigned int REG0X2A                               ;
    volatile unsigned int REG0X2B                               ;
    volatile unsigned int REG0X2C                               ;
    volatile unsigned int REG0X2D                               ;
    volatile unsigned int REG0X2E                               ;
    volatile unsigned int REG0X2F                               ;
    volatile unsigned int REG0X30                               ;
    volatile unsigned int REG0X31                               ;
    volatile unsigned int REG0X32                               ;
} RC_TypeDef;

/* TRX */
typedef struct
{
    volatile unsigned int REG0X00                               ;
    volatile unsigned int REG0X01                               ;
    volatile unsigned int REG0X02                               ;
    volatile unsigned int REG0X03                               ;
    volatile unsigned int REG0X04                               ;
    volatile unsigned int REG0X05                               ;
    volatile unsigned int REG0X06                               ;
    volatile unsigned int REG0X07                               ;
    volatile unsigned int REG0X08                               ;
    volatile unsigned int REG0X09                               ;
    volatile unsigned int REG0X0A                               ;
    volatile unsigned int REG0X0B                               ;
    volatile unsigned int REG0X0C                               ;
             unsigned int RES0XD_To_0X11[5]                     ;
    volatile unsigned int REG0X12                               ;
    volatile unsigned int REG0X13                               ;
} TRX_TypeDef;

/* POWTBL */
typedef struct
{
    volatile unsigned int REG0X00                               ;
    volatile unsigned int REG0X01                               ;
    volatile unsigned int REG0X02                               ;
    volatile unsigned int REG0X03                               ;
    volatile unsigned int REG0X04                               ;
    volatile unsigned int REG0X05                               ;
    volatile unsigned int REG0X06                               ;
    volatile unsigned int REG0X07                               ;
    volatile unsigned int REG0X08                               ;
    volatile unsigned int REG0X09                               ;
    volatile unsigned int REG0X0A                               ;
    volatile unsigned int REG0X0B                               ;
    volatile unsigned int REG0X0C                               ;
    volatile unsigned int REG0X0D                               ;
    volatile unsigned int REG0X0E                               ;
    volatile unsigned int REG0X0F                               ;
    volatile unsigned int REG0X10                               ;
    volatile unsigned int REG0X11                               ;
    volatile unsigned int REG0X12                               ;
    volatile unsigned int REG0X13                               ;
    volatile unsigned int REG0X14                               ;
    volatile unsigned int REG0X15                               ;
    volatile unsigned int REG0X16                               ;
    volatile unsigned int REG0X17                               ;
    volatile unsigned int REG0X18                               ;
    volatile unsigned int REG0X19                               ;
    volatile unsigned int REG0X1A                               ;
    volatile unsigned int REG0X1B                               ;
    volatile unsigned int REG0X1C                               ;
    volatile unsigned int REG0X1D                               ;
    volatile unsigned int REG0X1E                               ;
    volatile unsigned int REG0X1F                               ;
    volatile unsigned int REG0X20                               ;
    volatile unsigned int REG0X21                               ;
    volatile unsigned int REG0X22                               ;
    volatile unsigned int REG0X23                               ;
    volatile unsigned int REG0X24                               ;
    volatile unsigned int REG0X25                               ;
    volatile unsigned int REG0X26                               ;
    volatile unsigned int REG0X27                               ;
    volatile unsigned int REG0X28                               ;
    volatile unsigned int REG0X29                               ;
    volatile unsigned int REG0X2A                               ;
    volatile unsigned int REG0X2B                               ;
    volatile unsigned int REG0X2C                               ;
    volatile unsigned int REG0X2D                               ;
    volatile unsigned int REG0X2E                               ;
    volatile unsigned int REG0X2F                               ;
    volatile unsigned int REG0X30                               ;
    volatile unsigned int REG0X31                               ;
    volatile unsigned int REG0X32                               ;
    volatile unsigned int REG0X33                               ;
    volatile unsigned int REG0X34                               ;
    volatile unsigned int REG0X35                               ;
    volatile unsigned int REG0X36                               ;
    volatile unsigned int REG0X37                               ;
    volatile unsigned int REG0X38                               ;
    volatile unsigned int REG0X39                               ;
    volatile unsigned int REG0X3A                               ;
    volatile unsigned int REG0X3B                               ;
    volatile unsigned int REG0X3C                               ;
    volatile unsigned int REG0X3D                               ;
    volatile unsigned int REG0X3E                               ;
    volatile unsigned int REG0X3F                               ;
    volatile unsigned int REG0X40                               ;
    volatile unsigned int REG0X41                               ;
    volatile unsigned int REG0X42                               ;
    volatile unsigned int REG0X43                               ;
    volatile unsigned int REG0X44                               ;
    volatile unsigned int REG0X45                               ;
    volatile unsigned int REG0X46                               ;
    volatile unsigned int REG0X47                               ;
    volatile unsigned int REG0X48                               ;
    volatile unsigned int REG0X49                               ;
    volatile unsigned int REG0X4A                               ;
    volatile unsigned int REG0X4B                               ;
    volatile unsigned int REG0X4C                               ;
    volatile unsigned int REG0X4D                               ;
    volatile unsigned int REG0X4E                               ;
    volatile unsigned int REG0X4F                               ;
    volatile unsigned int REG0X50                               ;
    volatile unsigned int REG0X51                               ;
    volatile unsigned int REG0X52                               ;
    volatile unsigned int REG0X53                               ;
    volatile unsigned int REG0X54                               ;
    volatile unsigned int REG0X55                               ;
    volatile unsigned int REG0X56                               ;
    volatile unsigned int REG0X57                               ;
    volatile unsigned int REG0X58                               ;
    volatile unsigned int REG0X59                               ;
    volatile unsigned int REG0X5A                               ;
    volatile unsigned int REG0X5B                               ;
    volatile unsigned int REG0X5C                               ;
    volatile unsigned int REG0X5D                               ;
    volatile unsigned int REG0X5E                               ;
    volatile unsigned int REG0X5F                               ;
    volatile unsigned int REG0X60                               ;
    volatile unsigned int REG0X61                               ;
    volatile unsigned int REG0X62                               ;
    volatile unsigned int REG0X63                               ;
    volatile unsigned int REG0X64                               ;
    volatile unsigned int REG0X65                               ;
    volatile unsigned int REG0X66                               ;
    volatile unsigned int REG0X67                               ;
    volatile unsigned int REG0X68                               ;
    volatile unsigned int REG0X69                               ;
    volatile unsigned int REG0X6A                               ;
    volatile unsigned int REG0X6B                               ;
    volatile unsigned int REG0X6C                               ;
    volatile unsigned int REG0X6D                               ;
    volatile unsigned int REG0X6E                               ;
    volatile unsigned int REG0X6F                               ;
    volatile unsigned int REG0X70                               ;
    volatile unsigned int REG0X71                               ;
    volatile unsigned int REG0X72                               ;
    volatile unsigned int REG0X73                               ;
    volatile unsigned int REG0X74                               ;
    volatile unsigned int REG0X75                               ;
    volatile unsigned int REG0X76                               ;
    volatile unsigned int REG0X77                               ;
    volatile unsigned int REG0X78                               ;
    volatile unsigned int REG0X79                               ;
    volatile unsigned int REG0X7A                               ;
    volatile unsigned int REG0X7B                               ;
    volatile unsigned int REG0X7C                               ;
    volatile unsigned int REG0X7D                               ;
    volatile unsigned int REG0X7E                               ;
    volatile unsigned int REG0X7F                               ;
} POWTBL_TypeDef;

/* DPDTBL */
typedef struct
{
    volatile unsigned int REG0X00                               ;
    volatile unsigned int REG0X01                               ;
    volatile unsigned int REG0X02                               ;
    volatile unsigned int REG0X03                               ;
    volatile unsigned int REG0X04                               ;
    volatile unsigned int REG0X05                               ;
    volatile unsigned int REG0X06                               ;
    volatile unsigned int REG0X07                               ;
    volatile unsigned int REG0X08                               ;
    volatile unsigned int REG0X09                               ;
    volatile unsigned int REG0X0A                               ;
    volatile unsigned int REG0X0B                               ;
    volatile unsigned int REG0X0C                               ;
    volatile unsigned int REG0X0D                               ;
    volatile unsigned int REG0X0E                               ;
    volatile unsigned int REG0X0F                               ;
    volatile unsigned int REG0X10                               ;
    volatile unsigned int REG0X11                               ;
    volatile unsigned int REG0X12                               ;
    volatile unsigned int REG0X13                               ;
    volatile unsigned int REG0X14                               ;
    volatile unsigned int REG0X15                               ;
    volatile unsigned int REG0X16                               ;
    volatile unsigned int REG0X17                               ;
    volatile unsigned int REG0X18                               ;
    volatile unsigned int REG0X19                               ;
    volatile unsigned int REG0X1A                               ;
    volatile unsigned int REG0X1B                               ;
    volatile unsigned int REG0X1C                               ;
    volatile unsigned int REG0X1D                               ;
    volatile unsigned int REG0X1E                               ;
    volatile unsigned int REG0X1F                               ;
    volatile unsigned int REG0X20                               ;
    volatile unsigned int REG0X21                               ;
    volatile unsigned int REG0X22                               ;
    volatile unsigned int REG0X23                               ;
    volatile unsigned int REG0X24                               ;
    volatile unsigned int REG0X25                               ;
    volatile unsigned int REG0X26                               ;
    volatile unsigned int REG0X27                               ;
    volatile unsigned int REG0X28                               ;
    volatile unsigned int REG0X29                               ;
    volatile unsigned int REG0X2A                               ;
    volatile unsigned int REG0X2B                               ;
    volatile unsigned int REG0X2C                               ;
    volatile unsigned int REG0X2D                               ;
    volatile unsigned int REG0X2E                               ;
    volatile unsigned int REG0X2F                               ;
    volatile unsigned int REG0X30                               ;
    volatile unsigned int REG0X31                               ;
    volatile unsigned int REG0X32                               ;
    volatile unsigned int REG0X33                               ;
    volatile unsigned int REG0X34                               ;
    volatile unsigned int REG0X35                               ;
    volatile unsigned int REG0X36                               ;
    volatile unsigned int REG0X37                               ;
    volatile unsigned int REG0X38                               ;
    volatile unsigned int REG0X39                               ;
    volatile unsigned int REG0X3A                               ;
    volatile unsigned int REG0X3B                               ;
    volatile unsigned int REG0X3C                               ;
    volatile unsigned int REG0X3D                               ;
    volatile unsigned int REG0X3E                               ;
    volatile unsigned int REG0X3F                               ;
    volatile unsigned int REG0X40                               ;
    volatile unsigned int REG0X41                               ;
    volatile unsigned int REG0X42                               ;
    volatile unsigned int REG0X43                               ;
    volatile unsigned int REG0X44                               ;
    volatile unsigned int REG0X45                               ;
    volatile unsigned int REG0X46                               ;
    volatile unsigned int REG0X47                               ;
    volatile unsigned int REG0X48                               ;
    volatile unsigned int REG0X49                               ;
    volatile unsigned int REG0X4A                               ;
    volatile unsigned int REG0X4B                               ;
    volatile unsigned int REG0X4C                               ;
    volatile unsigned int REG0X4D                               ;
    volatile unsigned int REG0X4E                               ;
    volatile unsigned int REG0X4F                               ;
    volatile unsigned int REG0X50                               ;
    volatile unsigned int REG0X51                               ;
    volatile unsigned int REG0X52                               ;
    volatile unsigned int REG0X53                               ;
    volatile unsigned int REG0X54                               ;
    volatile unsigned int REG0X55                               ;
    volatile unsigned int REG0X56                               ;
    volatile unsigned int REG0X57                               ;
    volatile unsigned int REG0X58                               ;
    volatile unsigned int REG0X59                               ;
    volatile unsigned int REG0X5A                               ;
    volatile unsigned int REG0X5B                               ;
    volatile unsigned int REG0X5C                               ;
    volatile unsigned int REG0X5D                               ;
    volatile unsigned int REG0X5E                               ;
    volatile unsigned int REG0X5F                               ;
    volatile unsigned int REG0X60                               ;
    volatile unsigned int REG0X61                               ;
    volatile unsigned int REG0X62                               ;
    volatile unsigned int REG0X63                               ;
    volatile unsigned int REG0X64                               ;
    volatile unsigned int REG0X65                               ;
    volatile unsigned int REG0X66                               ;
    volatile unsigned int REG0X67                               ;
    volatile unsigned int REG0X68                               ;
    volatile unsigned int REG0X69                               ;
    volatile unsigned int REG0X6A                               ;
    volatile unsigned int REG0X6B                               ;
    volatile unsigned int REG0X6C                               ;
    volatile unsigned int REG0X6D                               ;
    volatile unsigned int REG0X6E                               ;
    volatile unsigned int REG0X6F                               ;
    volatile unsigned int REG0X70                               ;
    volatile unsigned int REG0X71                               ;
    volatile unsigned int REG0X72                               ;
    volatile unsigned int REG0X73                               ;
    volatile unsigned int REG0X74                               ;
    volatile unsigned int REG0X75                               ;
    volatile unsigned int REG0X76                               ;
    volatile unsigned int REG0X77                               ;
    volatile unsigned int REG0X78                               ;
    volatile unsigned int REG0X79                               ;
    volatile unsigned int REG0X7A                               ;
    volatile unsigned int REG0X7B                               ;
    volatile unsigned int REG0X7C                               ;
    volatile unsigned int REG0X7D                               ;
    volatile unsigned int REG0X7E                               ;
    volatile unsigned int REG0X7F                               ;
    volatile unsigned int REG0X80                               ;
    volatile unsigned int REG0X81                               ;
    volatile unsigned int REG0X82                               ;
    volatile unsigned int REG0X83                               ;
    volatile unsigned int REG0X84                               ;
    volatile unsigned int REG0X85                               ;
    volatile unsigned int REG0X86                               ;
    volatile unsigned int REG0X87                               ;
    volatile unsigned int REG0X88                               ;
    volatile unsigned int REG0X89                               ;
    volatile unsigned int REG0X8A                               ;
    volatile unsigned int REG0X8B                               ;
    volatile unsigned int REG0X8C                               ;
    volatile unsigned int REG0X8D                               ;
    volatile unsigned int REG0X8E                               ;
    volatile unsigned int REG0X8F                               ;
    volatile unsigned int REG0X90                               ;
    volatile unsigned int REG0X91                               ;
    volatile unsigned int REG0X92                               ;
    volatile unsigned int REG0X93                               ;
    volatile unsigned int REG0X94                               ;
    volatile unsigned int REG0X95                               ;
    volatile unsigned int REG0X96                               ;
    volatile unsigned int REG0X97                               ;
    volatile unsigned int REG0X98                               ;
    volatile unsigned int REG0X99                               ;
    volatile unsigned int REG0X9A                               ;
    volatile unsigned int REG0X9B                               ;
    volatile unsigned int REG0X9C                               ;
    volatile unsigned int REG0X9D                               ;
    volatile unsigned int REG0X9E                               ;
    volatile unsigned int REG0X9F                               ;
    volatile unsigned int REG0XA0                               ;
    volatile unsigned int REG0XA1                               ;
    volatile unsigned int REG0XA2                               ;
    volatile unsigned int REG0XA3                               ;
    volatile unsigned int REG0XA4                               ;
    volatile unsigned int REG0XA5                               ;
    volatile unsigned int REG0XA6                               ;
    volatile unsigned int REG0XA7                               ;
    volatile unsigned int REG0XA8                               ;
    volatile unsigned int REG0XA9                               ;
    volatile unsigned int REG0XAA                               ;
    volatile unsigned int REG0XAB                               ;
    volatile unsigned int REG0XAC                               ;
    volatile unsigned int REG0XAD                               ;
    volatile unsigned int REG0XAE                               ;
    volatile unsigned int REG0XAF                               ;
    volatile unsigned int REG0XB0                               ;
    volatile unsigned int REG0XB1                               ;
    volatile unsigned int REG0XB2                               ;
    volatile unsigned int REG0XB3                               ;
    volatile unsigned int REG0XB4                               ;
    volatile unsigned int REG0XB5                               ;
    volatile unsigned int REG0XB6                               ;
    volatile unsigned int REG0XB7                               ;
    volatile unsigned int REG0XB8                               ;
    volatile unsigned int REG0XB9                               ;
    volatile unsigned int REG0XBA                               ;
    volatile unsigned int REG0XBB                               ;
    volatile unsigned int REG0XBC                               ;
    volatile unsigned int REG0XBD                               ;
    volatile unsigned int REG0XBE                               ;
    volatile unsigned int REG0XBF                               ;
    volatile unsigned int REG0XC0                               ;
    volatile unsigned int REG0XC1                               ;
    volatile unsigned int REG0XC2                               ;
    volatile unsigned int REG0XC3                               ;
    volatile unsigned int REG0XC4                               ;
    volatile unsigned int REG0XC5                               ;
    volatile unsigned int REG0XC6                               ;
    volatile unsigned int REG0XC7                               ;
    volatile unsigned int REG0XC8                               ;
    volatile unsigned int REG0XC9                               ;
    volatile unsigned int REG0XCA                               ;
    volatile unsigned int REG0XCB                               ;
    volatile unsigned int REG0XCC                               ;
    volatile unsigned int REG0XCD                               ;
    volatile unsigned int REG0XCE                               ;
    volatile unsigned int REG0XCF                               ;
    volatile unsigned int REG0XD0                               ;
    volatile unsigned int REG0XD1                               ;
    volatile unsigned int REG0XD2                               ;
    volatile unsigned int REG0XD3                               ;
    volatile unsigned int REG0XD4                               ;
    volatile unsigned int REG0XD5                               ;
    volatile unsigned int REG0XD6                               ;
    volatile unsigned int REG0XD7                               ;
    volatile unsigned int REG0XD8                               ;
    volatile unsigned int REG0XD9                               ;
    volatile unsigned int REG0XDA                               ;
    volatile unsigned int REG0XDB                               ;
    volatile unsigned int REG0XDC                               ;
    volatile unsigned int REG0XDD                               ;
    volatile unsigned int REG0XDE                               ;
    volatile unsigned int REG0XDF                               ;
    volatile unsigned int REG0XE0                               ;
    volatile unsigned int REG0XE1                               ;
    volatile unsigned int REG0XE2                               ;
    volatile unsigned int REG0XE3                               ;
    volatile unsigned int REG0XE4                               ;
    volatile unsigned int REG0XE5                               ;
    volatile unsigned int REG0XE6                               ;
    volatile unsigned int REG0XE7                               ;
    volatile unsigned int REG0XE8                               ;
    volatile unsigned int REG0XE9                               ;
    volatile unsigned int REG0XEA                               ;
    volatile unsigned int REG0XEB                               ;
    volatile unsigned int REG0XEC                               ;
    volatile unsigned int REG0XED                               ;
    volatile unsigned int REG0XEE                               ;
    volatile unsigned int REG0XEF                               ;
    volatile unsigned int REG0XF0                               ;
    volatile unsigned int REG0XF1                               ;
    volatile unsigned int REG0XF2                               ;
    volatile unsigned int REG0XF3                               ;
    volatile unsigned int REG0XF4                               ;
    volatile unsigned int REG0XF5                               ;
    volatile unsigned int REG0XF6                               ;
    volatile unsigned int REG0XF7                               ;
    volatile unsigned int REG0XF8                               ;
    volatile unsigned int REG0XF9                               ;
    volatile unsigned int REG0XFA                               ;
    volatile unsigned int REG0XFB                               ;
    volatile unsigned int REG0XFC                               ;
    volatile unsigned int REG0XFD                               ;
    volatile unsigned int REG0XFE                               ;
    volatile unsigned int REG0XFF                               ;
} DPDTBL_TypeDef;

/* LDPCRXCFGMEM */
typedef struct
{
    volatile unsigned int REG0X00                               ;
             unsigned int RES0X1_To_0X14D[333]                  ;
    volatile unsigned int REG0X14E                              ;
} LDPCRXCFGMEM_TypeDef;

/* AGCMEM */
typedef struct
{
    volatile unsigned int REG0X00                               ;
             unsigned int RES0X1_To_0X1FE[510]                  ;
    volatile unsigned int REG0X1FF                              ;
} AGCMEM_TypeDef;

/* PEAKCWMEM */
typedef struct
{
    volatile unsigned int REG0X00                               ;
             unsigned int RES0X1_To_0X7E[126]                   ;
    volatile unsigned int REG0X7F                               ;
    volatile unsigned int REG0X80                               ;
             unsigned int RES0X81_To_0XFE[126]                  ;
    volatile unsigned int REG0XFF                               ;
} PEAKCWMEM_TypeDef;


/**********************************************************************/
/* bit definition for RC                                              */
/**********************************************************************/

/* RC_Reg0x0 */
#define POS_RC_RF_LNA_SAT                                       (29U)
#define BIT_RC_RF_LNA_SAT                                       (0x20000000U)
#define POS_RC_RF_PLL_UNLOCK                                    (28U)
#define BIT_RC_RF_PLL_UNLOCK                                    (0x10000000U)
#define POS_RC_RF_STATE                                         (24U)
#define BIT_RC_RF_STATE                                         (0xF000000U)
#define POS_RC_SPI_PRESCALER                                    (8U)
#define BIT_RC_SPI_PRESCALER                                    (0x3F00U)
#define POS_RC_RF_SPI_RESET                                     (1U)
#define BIT_RC_RF_SPI_RESET                                     (0x2U)
#define POS_RC_RF_EN                                            (0U)
#define BIT_RC_RF_EN                                            (0x1U)

/* RC_Reg0x1 */
#define POS_RC_FORCE_PRE_GAIN                                   (4U)
#define BIT_RC_FORCE_PRE_GAIN                                   (0x3FF0U)
#define POS_RC_FORCE_TX_ON                                      (3U)
#define BIT_RC_FORCE_TX_ON                                      (0x8U)
#define POS_RC_FORCE_RX_ON                                      (2U)
#define BIT_RC_FORCE_RX_ON                                      (0x4U)
#define POS_RC_FORCE_SHUTDOWN                                   (1U)
#define BIT_RC_FORCE_SHUTDOWN                                   (0x2U)
#define POS_RC_FORCE_EN                                         (0U)
#define BIT_RC_FORCE_EN                                         (0x1U)

/* RC_Reg0x2 */
#define POS_RC_RX_OFF_DELAY                                     (16U)
#define BIT_RC_RX_OFF_DELAY                                     (0x3FF0000U)
#define POS_RC_RX_ON_DELAY                                      (0U)
#define BIT_RC_RX_ON_DELAY                                      (0x3FFU)

/* RC_Reg0x3 */
#define POS_RC_TX_OFF_DELAY                                     (16U)
#define BIT_RC_TX_OFF_DELAY                                     (0x3FF0000U)
#define POS_RC_TX_ON_DELAY                                      (0U)
#define BIT_RC_TX_ON_DELAY                                      (0x3FFU)

/* RC_Reg0x4 */
#define POS_RC_PA_OFF_DELAY                                     (16U)
#define BIT_RC_PA_OFF_DELAY                                     (0x3FF0000U)
#define POS_RC_PA_ON_DELAY                                      (0U)
#define BIT_RC_PA_ON_DELAY                                      (0x3FFU)

/* RC_Reg0x5 */
#define POS_RC_SHDN_OFF_DELAY                                   (16U)
#define BIT_RC_SHDN_OFF_DELAY                                   (0x3FF0000U)
#define POS_RC_SHDN_ON_DELAY                                    (0U)
#define BIT_RC_SHDN_ON_DELAY                                    (0x3FFU)

/* RC_Reg0x6 */
#define POS_RC_RX2TX_DELAY                                      (0U)
#define BIT_RC_RX2TX_DELAY                                      (0x3FFU)

/* RC_Reg0x8 */
#define POS_RC_RX_ADC_SMP_EDGE                                  (22U)
#define BIT_RC_RX_ADC_SMP_EDGE                                  (0x400000U)
#define POS_RC_RX_AVG_MODE                                      (21U)
#define BIT_RC_RX_AVG_MODE                                      (0x200000U)
#define POS_RC_RX_COMP_EN                                       (20U)
#define BIT_RC_RX_COMP_EN                                       (0x100000U)
#define POS_RC_RX_CALIB_EN                                      (19U)
#define BIT_RC_RX_CALIB_EN                                      (0x80000U)
#define POS_RC_RX_DC_CALC_EN                                    (18U)
#define BIT_RC_RX_DC_CALC_EN                                    (0x40000U)
#define POS_RC_RX_IQ_SWAP                                       (16U)
#define BIT_RC_RX_IQ_SWAP                                       (0x10000U)
#define POS_RC_TX_DPD_ADDR_SCALE                                (6U)
#define BIT_RC_TX_DPD_ADDR_SCALE                                (0x40U)
#define POS_RC_TX_DPD_EN                                        (5U)
#define BIT_RC_TX_DPD_EN                                        (0x20U)
#define POS_RC_TX_POWTBL_EN                                     (4U)
#define BIT_RC_TX_POWTBL_EN                                     (0x10U)
#define POS_RC_TX_COMP_DISABLE                                  (3U)
#define BIT_RC_TX_COMP_DISABLE                                  (0x8U)
#define POS_RC_TX_IQ_SWAP                                       (2U)
#define BIT_RC_TX_IQ_SWAP                                       (0x4U)
#define POS_RC_TX_PATTERN                                       (0U)
#define BIT_RC_TX_PATTERN                                       (0x3U)

/* RC_Reg0x9 */
#define POS_RC_TX_CONST_I                                       (16U)
#define BIT_RC_TX_CONST_I                                       (0xFFF0000U)
#define POS_RC_TX_CONST_Q                                       (0U)
#define BIT_RC_TX_CONST_Q                                       (0xFFFU)

/* RC_Reg0xa */
#define POS_RC_TX_TRIANGLE_AMP                                  (24U)
#define BIT_RC_TX_TRIANGLE_AMP                                  (0x1000000U)
#define POS_RC_TX_SIN_FREQ                                      (8U)
#define BIT_RC_TX_SIN_FREQ                                      (0x3FF00U)
#define POS_RC_TX_SIN_AMP                                       (4U)
#define BIT_RC_TX_SIN_AMP                                       (0xF0U)
#define POS_RC_TX_SIN_MODE                                      (0U)
#define BIT_RC_TX_SIN_MODE                                      (0x3U)

/* RC_Reg0xb */
#define POS_RC_TX_DC_I_COMP                                     (16U)
#define BIT_RC_TX_DC_I_COMP                                     (0xFFF0000U)
#define POS_RC_TX_DC_Q_COMP                                     (0U)
#define BIT_RC_TX_DC_Q_COMP                                     (0xFFFU)

/* RC_Reg0xc */
#define POS_RC_TX_GAIN_I_COMP                                   (16U)
#define BIT_RC_TX_GAIN_I_COMP                                   (0xFFF0000U)
#define POS_RC_TX_GAIN_Q_COMP                                   (0U)
#define BIT_RC_TX_GAIN_Q_COMP                                   (0xFFFU)

/* RC_Reg0xd */
#define POS_RC_TX_PHASE_COMP                                    (16U)
#define BIT_RC_TX_PHASE_COMP                                    (0xFFF0000U)
#define POS_RC_TX_TY2_COMP                                      (0U)
#define BIT_RC_TX_TY2_COMP                                      (0xFFFU)

/* RC_Reg0xe */
#define POS_RC_RX_AVG_I_EST                                     (12U)
#define BIT_RC_RX_AVG_I_EST                                     (0xFFF000U)
#define POS_RC_RX_AVG_Q_EST                                     (0U)
#define BIT_RC_RX_AVG_Q_EST                                     (0xFFFU)

/* RC_Reg0xf */
#define POS_RC_RX_DC_I_EST                                      (12U)
#define BIT_RC_RX_DC_I_EST                                      (0xFFF000U)
#define POS_RC_RX_DC_Q_EST                                      (0U)
#define BIT_RC_RX_DC_Q_EST                                      (0xFFFU)

/* RC_Reg0x10 */
#define POS_RC_RX_AMP_ERR_EST                                   (16U)
#define BIT_RC_RX_AMP_ERR_EST                                   (0x3FF0000U)
#define POS_RC_RX_PHASE_ERR_EST                                 (0U)
#define BIT_RC_RX_PHASE_ERR_EST                                 (0x3FFU)

/* RC_Reg0x11 */
#define POS_RC_RX_TY2_EST                                       (0U)
#define BIT_RC_RX_TY2_EST                                       (0x3FFU)

/* RC_Reg0x12 */
#define POS_RC_RX_DC_I_COMP                                     (12U)
#define BIT_RC_RX_DC_I_COMP                                     (0xFFF000U)
#define POS_RC_RX_DC_Q_COMP                                     (0U)
#define BIT_RC_RX_DC_Q_COMP                                     (0xFFFU)

/* RC_Reg0x13 */
#define POS_RC_RX_AMP_ERR_COMP                                  (16U)
#define BIT_RC_RX_AMP_ERR_COMP                                  (0x3FF0000U)
#define POS_RC_RX_PHASE_ERR_COMP                                (0U)
#define BIT_RC_RX_PHASE_ERR_COMP                                (0x3FFU)

/* RC_Reg0x14 */
#define POS_RC_RX_SNR_SIG                                       (16U)
#define BIT_RC_RX_SNR_SIG                                       (0x1FF0000U)
#define POS_RC_RX_SNR_NOISE                                     (0U)
#define BIT_RC_RX_SNR_NOISE                                     (0x1FFU)

/* RC_Reg0x15 */
#define POS_RC_STANDBY_CAL_CAP_I                                (16U)
#define BIT_RC_STANDBY_CAL_CAP_I                                (0xFF0000U)
#define POS_RC_RX_CAL_CAP_I                                     (8U)
#define BIT_RC_RX_CAL_CAP_I                                     (0xFF00U)
#define POS_RC_TX_CAL_CAP_I                                     (0U)
#define BIT_RC_TX_CAL_CAP_I                                     (0xFFU)

/* RC_Reg0x16 */
#define POS_RC_STANDBY_CAL_CAP_Q                                (16U)
#define BIT_RC_STANDBY_CAL_CAP_Q                                (0xFF0000U)
#define POS_RC_RX_CAL_CAP_Q                                     (8U)
#define BIT_RC_RX_CAL_CAP_Q                                     (0xFF00U)
#define POS_RC_TX_CAL_CAP_Q                                     (0U)
#define BIT_RC_TX_CAL_CAP_Q                                     (0xFFU)

/* RC_Reg0x17 */
#define POS_RC_TX_PRE_GAIN_2ND                                  (8U)
#define BIT_RC_TX_PRE_GAIN_2ND                                  (0x3FF00U)

/* RC_Reg0x18 */
#define POS_RC_AGC_ATTENT_VALUE                                 (23U)
#define BIT_RC_AGC_ATTENT_VALUE                                 (0x800000U)
#define POS_RC_AGC_LNA_VALUE                                    (21U)
#define BIT_RC_AGC_LNA_VALUE                                    (0x600000U)
#define POS_RC_AGC_BUF_VALUE                                    (20U)
#define BIT_RC_AGC_BUF_VALUE                                    (0x100000U)
#define POS_RC_AGC_PGA_VALUE                                    (16U)
#define BIT_RC_AGC_PGA_VALUE                                    (0xF0000U)
#define POS_RC_AGC_ATTENT_SET                                   (11U)
#define BIT_RC_AGC_ATTENT_SET                                   (0x800U)
#define POS_RC_AGC_LNA_SET                                      (9U)
#define BIT_RC_AGC_LNA_SET                                      (0x600U)
#define POS_RC_AGC_BUF_SET                                      (8U)
#define BIT_RC_AGC_BUF_SET                                      (0x100U)
#define POS_RC_AGC_PGA_SET                                      (4U)
#define BIT_RC_AGC_PGA_SET                                      (0xF0U)
#define POS_RC_AGC_MANUAL_EN                                    (0U)
#define BIT_RC_AGC_MANUAL_EN                                    (0x1U)

/* RC_Reg0x19 */
#define POS_RC_TSSI_EST                                         (8U)
#define BIT_RC_TSSI_EST                                         (0xFF00U)
#define POS_RC_RX_SNR_HPF_COEF                                  (2U)
#define BIT_RC_RX_SNR_HPF_COEF                                  (0xCU)
#define POS_RC_RX_SNR_CALC_EN                                   (1U)
#define BIT_RC_RX_SNR_CALC_EN                                   (0x2U)
#define POS_RC_TSSI_CALC_EN                                     (0U)
#define BIT_RC_TSSI_CALC_EN                                     (0x1U)

/* RC_Reg0x1a */
#define POS_RC_RF_GAIN_NF_DB                                    (0U)
#define BIT_RC_RF_GAIN_NF_DB                                    (0x3FU)

/* RC_Reg0x1b */
#define POS_RC_RF_ATTENT_GAIN                                   (8U)
#define BIT_RC_RF_ATTENT_GAIN                                   (0x1F00U)
#define POS_RC_RF_PATH_GAIN                                     (0U)
#define BIT_RC_RF_PATH_GAIN                                     (0x1FU)

/* RC_Reg0x1c */
#define POS_RC_RX_ADC_DBM                                       (0U)
#define BIT_RC_RX_ADC_DBM                                       (0xFFU)

/* RC_Reg0x23 */
#define POS_RC_RF_DC_Q_COMP_0DB                                 (8U)
#define BIT_RC_RF_DC_Q_COMP_0DB                                 (0xFF00U)
#define POS_RC_RF_DC_I_COMP_0DB                                 (0U)
#define BIT_RC_RF_DC_I_COMP_0DB                                 (0xFFU)

/* RC_Reg0x24 */
#define POS_RC_RF_DC_Q_COMP_3DB                                 (8U)
#define BIT_RC_RF_DC_Q_COMP_3DB                                 (0xFF00U)
#define POS_RC_RF_DC_I_COMP_3DB                                 (0U)
#define BIT_RC_RF_DC_I_COMP_3DB                                 (0xFFU)

/* RC_Reg0x25 */
#define POS_RC_RF_DC_Q_COMP_6DB                                 (8U)
#define BIT_RC_RF_DC_Q_COMP_6DB                                 (0xFF00U)
#define POS_RC_RF_DC_I_COMP_6DB                                 (0U)
#define BIT_RC_RF_DC_I_COMP_6DB                                 (0xFFU)

/* RC_Reg0x26 */
#define POS_RC_RF_DC_Q_COMP_9DB                                 (8U)
#define BIT_RC_RF_DC_Q_COMP_9DB                                 (0xFF00U)
#define POS_RC_RF_DC_I_COMP_9DB                                 (0U)
#define BIT_RC_RF_DC_I_COMP_9DB                                 (0xFFU)

/* RC_Reg0x27 */
#define POS_RC_RF_DC_Q_COMP_12DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_12DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_12DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_12DB                                (0xFFU)

/* RC_Reg0x28 */
#define POS_RC_RF_DC_Q_COMP_15DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_15DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_15DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_15DB                                (0xFFU)

/* RC_Reg0x29 */
#define POS_RC_RF_DC_Q_COMP_18DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_18DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_18DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_18DB                                (0xFFU)

/* RC_Reg0x2a */
#define POS_RC_RF_DC_Q_COMP_21DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_21DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_21DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_21DB                                (0xFFU)

/* RC_Reg0x2b */
#define POS_RC_RF_DC_Q_COMP_24DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_24DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_24DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_24DB                                (0xFFU)

/* RC_Reg0x2c */
#define POS_RC_RF_DC_Q_COMP_27DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_27DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_27DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_27DB                                (0xFFU)

/* RC_Reg0x2d */
#define POS_RC_RF_DC_Q_COMP_30DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_30DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_30DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_30DB                                (0xFFU)

/* RC_Reg0x2e */
#define POS_RC_RF_DC_Q_COMP_33DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_33DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_33DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_33DB                                (0xFFU)

/* RC_Reg0x2f */
#define POS_RC_RF_DC_Q_COMP_36DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_36DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_36DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_36DB                                (0xFFU)

/* RC_Reg0x30 */
#define POS_RC_RF_DC_Q_COMP_39DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_39DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_39DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_39DB                                (0xFFU)

/* RC_Reg0x31 */
#define POS_RC_RF_DC_Q_COMP_42DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_42DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_42DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_42DB                                (0xFFU)

/* RC_Reg0x32 */
#define POS_RC_RF_DC_Q_COMP_45DB                                (8U)
#define BIT_RC_RF_DC_Q_COMP_45DB                                (0xFF00U)
#define POS_RC_RF_DC_I_COMP_45DB                                (0U)
#define BIT_RC_RF_DC_I_COMP_45DB                                (0xFFU)

/**********************************************************************/
/* bit definition for TRX                                             */
/**********************************************************************/

/**********************************************************************/
/* bit definition for POWTBL                                          */
/**********************************************************************/

/**********************************************************************/
/* bit definition for DPDTBL                                          */
/**********************************************************************/

/* DPDTBL_Reg0x00 */
#define POS_DPDTBL_DPD_Q_0TH                                    (16U)
#define BIT_DPDTBL_DPD_Q_0TH                                    (0x3FF0000U)
#define POS_DPDTBL_DPD_I_0TH                                    (0U)
#define BIT_DPDTBL_DPD_I_0TH                                    (0x3FFU)

/**********************************************************************/
/* bit definition for LDPCRXCFGMEM                                    */
/**********************************************************************/

/**********************************************************************/
/* bit definition for AGCMEM                                          */
/**********************************************************************/

/**********************************************************************/
/* bit definition for PEAKCWMEM                                       */
/**********************************************************************/

/* PEAKCWMEM_Reg0x00 */
#define POS_PEAKCWMEM_TCW_11G_CFG0                              (0U)
#define BIT_PEAKCWMEM_TCW_11G_CFG0                              (0x7FU)

/* PEAKCWMEM_Reg0x7f */
#define POS_PEAKCWMEM_TCW_11G_CFG127                            (0U)
#define BIT_PEAKCWMEM_TCW_11G_CFG127                            (0x7FU)

/* PEAKCWMEM_Reg0x80 */
#define POS_PEAKCWMEM_TCW_11N_CFG0                              (0U)
#define BIT_PEAKCWMEM_TCW_11N_CFG0                              (0x7FU)

/* PEAKCWMEM_Reg0xff */
#define POS_PEAKCWMEM_TCW_11N_CFG128                            (0U)
#define BIT_PEAKCWMEM_TCW_11N_CFG128                            (0x7FU)

#include "co_int.h"
#include "_reg_rc.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#define REG_RC_COUNT 107

#define REG_RC_DECODING_MASK 0x000001FF

#define RC_CNTL_STAT_ADDR   REG_RC_BASE_ADDR
#define RC_REG0_RF_EN_BIT   (1 << 0)

__INLINE void rc_cntl_stat_set(uint32_t value)
{
    // BK7236 only rc_en[bk7231n rc0:3], no rf_en[bk7231n rc0:1]
    // BK7236 rc_en is rc0:1
    uint32_t reg;
    if(value == 0)
    {
        // close rf?
        reg = REG_PL_RD(RC_CNTL_STAT_ADDR);
        reg &= ~(RC_REG0_RF_EN_BIT);
        REG_PL_WR(RC_CNTL_STAT_ADDR, reg);
    }
    else
    {
        // open rf?
        reg = REG_PL_RD(RC_CNTL_STAT_ADDR);
        reg |= (RC_REG0_RF_EN_BIT);
        REG_PL_WR(RC_CNTL_STAT_ADDR, reg);
    }
}

#endif // _REG_RC_H_
