
#ifndef RDA1846_H
#define RDA1846_H

#include "stm32g0xx_hal.h"

typedef struct {
	uint16_t soft_reset;
	uint16_t chip_cal_en;
	uint16_t pdn;
	uint16_t sq_on;
	uint16_t vox_on;
	uint16_t rx_on;
	uint16_t tx_on;
	uint16_t mute;
	uint16_t st_mode;
	uint16_t tail_elim_en;
	uint16_t channel_mode;
} RDA1846_REG_0x30;

typedef struct {
	uint16_t tx_CTCSS;
	uint16_t rx_CTCSS;
	uint16_t tone;
} RDA1846_ctcss;

#define RDA1846_I2C_ADDR  0x44  // Adjust if needed

// RF Band Options
#define RDA1846_BAND_REG 0x0F
#define RDA1846_BAND_400_520MHZ  0x00
#define RDA1846_BAND_200_260MHZ  0x02
#define RDA1846_BAND_134_174MHZ  0x03

// Frequency setting
#define RDA1846_FREQ_HIGH_VALUE 0x29
#define RDA1846_FREQ_LOW_VALUE 0x2A

// Reference Clock Mode
#define RDA1846_CLOCK_XTAL_REG 0x2B
#define RDA1846_CLOCK_ADCLK_REG 0x2C
#define RDA1846_CLOCK_MODE_REG 0x04
#define RDA1846_CLOCK_MODE_12_14MHZ 0x0001
#define RDA1846_CLOCK_MODE_24_28MHZ 0x0000

// TX RX selection
#define RDA1846_TX_ON 0x0040
#define RDA1846_RX_ON 0x0020
#define RDA1846_VOICE_CHANNEL_WIDE  0x3000
#define RDA1846_VOICE_CHANNEL_NARROW  0x0000

//Deep sleep
#define RDA1846_DEEP_SLEEP_ENABLE 0x0004
#define RDA1846_DEEP_SLEEP_DISABLE 0x0000

//TX voice channel
#define RDA1846_TX_VOICE_CHANNEL_REG 0x3C
#define RDA1846_TX_VOICE_CHANNEL_MIC 0x0000
#define RDA1846_TX_VOICE_CHANNEL_TONE 0x4000
#define RDA1846_TX_VOICE_CHANNEL_GPIO1 0x8000
#define RDA1846_TX_VOICE_CHANNEL_NOSIGNAL 0xC000

//PA bias
#define RDA1846_PA_BIAX_REG 0x0A

//Subaudio
#define RDA1846_CTCSS_CDCSS_MODE_REG 0x45
#define RDA1846_CTCSS_INNER_CTCSS_EN 0x0001
#define RDA1846_CTCSS_INNER_CDCSS_EN 0x0002
#define RDA1846_CTCSS_OUTTER_CTCSS_EN 0x0005
#define RDA1846_CTCSS_OUTTER_CDCSS_EN 0x0006

#define RDA1846_CTCSS_FREQ_REG 0x4A



// RDA1846 Flags
#define FLAG_SQUELCH_OPEN      (1 << 0)
#define FLAG_CTCSS_DETECTED    (1 << 1)
#define FLAG_DCS_DETECTED      (1 << 2)
#define FLAG_RX_ACTIVE         (1 << 3)
#define FLAG_TX_ACTIVE         (1 << 4)
#define FLAG_LOW_VOLTAGE       (1 << 5)
#define FLAG_PLL_LOCK          (1 << 6)
#define FLAG_RSSI_HIGH         (1 << 7)

// Global UART Handle
extern UART_HandleTypeDef huart2;

extern I2C_HandleTypeDef hi2c1;

void RDA1846_WriteRegister(uint16_t reg, uint16_t value);
uint16_t RDA1846_ReadRegister(uint8_t reg);
//void RDA1846_ReadRegister(uint16_t reg, uint16_t *value);
void RDA1846_SetFrequency(uint32_t freq);
//void RDA1846_SetBand(uint8_t band);
void RDA1846_SetReferenceClock(uint16_t clock_frequency);
void RDA1846_SetTxMode(RDA1846_REG_0x30 *settings, RDA1846_ctcss *ctcss);
void RDA1846_SetRxMode(RDA1846_REG_0x30 *settings, RDA1846_ctcss *ctcss);
void RDA1846_Set_Reg_0x30(RDA1846_REG_0x30 *settings);
void RDA1846_SetTxVoiceChannel(uint8_t channel);
void RDA1846_SetTxPaBias(uint16_t bias);
//void RDA1846_SetCTCSS(uint16_t freq);
void RDA1846_SetCTCSS_FREQ(RDA1846_ctcss *ctcss, uint16_t freq);
void RDA1846_SetCTCSS_TONE(RDA1846_ctcss *ctcss);
//void RDA1846_SetPreEmphasisDeEmphasis(uint8_t enable);
uint16_t RDA1846_ReadRSSI(void);
uint16_t RDA1846_ReadVSSI(void);
uint16_t RDA1846_ReadFlags(void);
void RDA1846_SendFlagsUART_DMA(uint16_t flags);



#endif // RDA1846_H
