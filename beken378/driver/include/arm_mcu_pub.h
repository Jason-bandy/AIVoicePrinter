#ifndef _ARM_MCU_PUB_H_
#define _ARM_MCU_PUB_H_
#include "sys_config.h"

#if (SOC_BK7271 == CFG_SOC_NAME)
#define W_SYS_CTRL_BASE_ADDR            0x00800000
#define W_PMU_BASE_ADDR                 0x00800200
#define W_LCD_BASE_ADDR                 0x00800280
#define W_GPIO_BASE_ADDR                0x00800300
#define W_SDIO_DMA_BASE_ADDR            0x00801000
#define W_APB_BUS_BASE_ADDR             0x00802000
#define W_ICU_BASE_ADDR                 0x00802000
#define W_UART1_BASE_ADDR               0x00802100
#define W_UART2_BASE_ADDR               0x00802140
#define W_UART3_BASE_ADDR               0x00802180
#define W_FM_I2C_BASE_ADDR              0x00802200
#define W_I2C1_BASE_ADDR                0x00802240
#define W_I2C2_BASE_ADDR                0x00802280
#define W_SDIO_HOST_BASE_ADDR           0x00802300
#define W_SPI1_BASE_ADDR                0x00802500
#define W_SPI2_BASE_ADDR                0x00802540
#define W_SPI3_BASE_ADDR                0x00802580
#define W_WDT_BASE_ADDR                 0x00802700
#define W_TRNG_BASE_ADDR                0x00802720
#define W_EFUSE_BASE_ADDR               0x00802740
#define W_IRDA_BASE_ADDR                0x00802760
#define W_TIMER1_BASE_ADDR              0x00802780
#define W_TIMER2_BASE_ADDR              0x008027C0
#define W_PWM1_BASE_ADDR                0x00802800
#define W_PWM2_BASE_ADDR                0x00802840
#define W_PWM3_BASE_ADDR                0x00802880
#define W_PWM4_BASE_ADDR                0x008028C0
#define W_SADC_BASE_ADDR                0x00802900
#define W_CEC_BASE_ADDR                 0x00802A00
#define W_FLASH_BASE_ADDR               0x00803000
#define W_USB1_BASE_ADDR                0x00804000
#define W_USB2_BASE_ADDR                0x00804800
#define W_GENER_DMA_BASE_ADDR           0x00805000
#define W_SECURITY_BASE_ADDR            0x00806000
#define W_JPEG_BASE_ADDR                0x00807000
#define W_MDM_CFG_BASE_ADDR             0x00900000
#define W_MDM_STAT_BASE_ADDR            0x00900000
#define W_RC_BEKEN_BASE_ADDR            0x00950000
#define W_TRX_BEKEN_BASE_ADDR           0x00950080
#define W_MACPHY_BYPASS_BASE_ADDR       0x00960000
#define W_MAC_CORE_BASE_ADDR            0x00A00000
#define W_MAC_PL_BASE_ADDR              0x00A08000
#define W_YUV_MEM_BASE_ADDR             0x00B00000
#define W_WIFI_DTCM_512KB_BASE_ADDR     0x00400000
#define W_WIFI_ITCM_32KB_BASE_ADDR      0x003F8000
#define W_BUS_SMEM_128KB_BASE_ADDR      0x04000000
#define W_DSP_DMEM_64KB_BASE_ADDR       0x0C000000
#define W_DSP_CPM_BASE_ADDR             0x0C400000
#define W_DSP_SMEM_1_5MB_BASE_ADDR      0x0C800000
#define W_PSRAM_BASE_ADDR               0x0D000000
#define W_DMA_BASE_ADDR                 0x0E800000
#define W_FFT_BASE_ADDR                 0x0E810000
#define W_APBD_BUS_BASE_ADDR            0x0E8F0000
#define W_ICUD_BASE_ADDR                0x0E8F0000
#define W_AUDIO_BASE_ADDR               0x0E8F1000
#define W_SPDIF_BASE_ADDR               0x0E8F2000
#define W_EQ_BASE_ADDR                  0x0E8F3000
#define W_I2S1_BASE_ADDR                0x0E8F4000
#define W_WDTD_BASE_ADDR                0x0E8F5000
#define W_RSV_BASE_ADDR                 0x0E8F6000
#define W_DSP_CTRL_BASE_ADDR            0x0E8F7000
#define W_I2S2_BASE_ADDR                0x0E8F8000
#define W_I2S3_BASE_ADDR                0x0E8F9000
#define W_PSRAM_CTRL_BASE_ADDR          0x0E8FA000
#define W_BT_IMEM_384KB_BASE_ADDR       0x10000000
#define W_BT_DMEM_96KB_BASE_ADDR        0x10400000
#endif

/* Macros for backup ARM registers in exception handlers */

/* Backup [R8~R14], CPSR and SPSR, totally 9 registers */
#define MCU_REG_BACKUP_NUM           9

/* From bottom to top, the registers are stored as:
 * bottom => CPSR, SPSR, R8, R9, R10, R11, R12, R13, R14 => top
 * SP(R13) is in offset 7
 * */
#define MCU_REG_BACKUP_SP_OFFSET     (7 << 2)
#define MCU_REG_BACKUP_ADDR_BASE     0x400020
#define MCU_REG_BACKUP_STACK_LEN     (MCU_REG_BACKUP_NUM << 2)

#define MCU_REG_BACKUP_BOTTOM_SYS    MCU_REG_BACKUP_ADDR_BASE
#define MCU_REG_BACKUP_TOP_SYS       (MCU_REG_BACKUP_BOTTOM_SYS + MCU_REG_BACKUP_STACK_LEN)
#define MCU_REG_BACKUP_SP_SYS        (MCU_REG_BACKUP_BOTTOM_SYS + MCU_REG_BACKUP_SP_OFFSET)

#define MCU_REG_BACKUP_BOTTOM_IRQ    MCU_REG_BACKUP_TOP_SYS
#define MCU_REG_BACKUP_TOP_IRQ       (MCU_REG_BACKUP_BOTTOM_IRQ + MCU_REG_BACKUP_STACK_LEN)
#define MCU_REG_BACKUP_SP_IRQ        (MCU_REG_BACKUP_BOTTOM_IRQ + MCU_REG_BACKUP_SP_OFFSET)

#define MCU_REG_BACKUP_BOTTOM_FIQ    MCU_REG_BACKUP_TOP_IRQ
#define MCU_REG_BACKUP_TOP_FIQ       (MCU_REG_BACKUP_BOTTOM_FIQ + MCU_REG_BACKUP_STACK_LEN)
#define MCU_REG_BACKUP_SP_FIQ        (MCU_REG_BACKUP_BOTTOM_FIQ + MCU_REG_BACKUP_SP_OFFSET)

#define MCU_REG_BACKUP_BOTTOM_ABT    MCU_REG_BACKUP_TOP_FIQ
#define MCU_REG_BACKUP_TOP_ABT       (MCU_REG_BACKUP_BOTTOM_ABT + MCU_REG_BACKUP_STACK_LEN)
#define MCU_REG_BACKUP_SP_ABT        (MCU_REG_BACKUP_BOTTOM_ABT + MCU_REG_BACKUP_SP_OFFSET)

#define MCU_REG_BACKUP_BOTTOM_UND    MCU_REG_BACKUP_TOP_ABT
#define MCU_REG_BACKUP_TOP_UND       (MCU_REG_BACKUP_BOTTOM_UND + MCU_REG_BACKUP_STACK_LEN)
#define MCU_REG_BACKUP_SP_UND        (MCU_REG_BACKUP_BOTTOM_UND + MCU_REG_BACKUP_SP_OFFSET)

#define MCU_REG_BACKUP_BOTTOM_SVC    MCU_REG_BACKUP_TOP_UND
#define MCU_REG_BACKUP_TOP_SVC       (MCU_REG_BACKUP_BOTTOM_SVC + MCU_REG_BACKUP_STACK_LEN)
#define MCU_REG_BACKUP_SP_SVC        (MCU_REG_BACKUP_BOTTOM_SVC + MCU_REG_BACKUP_SP_OFFSET)

#define FIQ_STACK_SIZE               0xFF0
#define IRQ_STACK_SIZE               0xFF0
#define SVC_STACK_SIZE               0x3F0
#define SYS_STACK_SIZE               0x3F0
#define UND_STACK_SIZE               0x280
#define ABT_STACK_SIZE               0x280

#endif //_ARM_MCU_PUB_H_
// eof

