#include "rda1846.h"
#include <stdio.h>
#include <string.h>

// Buffer for I2C transmission
uint8_t rda1846_tx_buffer[3];
uint8_t rda1846_rx_buffer[32];
// Buffer for UART DMA transmission
char uart_tx_buffer[128];

/**
 * @brief PMR446 channel to frequency conversion table.
 */
const uint32_t PMR446_Frequencies[16] = { 446006250, // Channel 1
		446018750, // Channel 2
		446031250, // Channel 3
		446043750, // Channel 4
		446056250, // Channel 5
		446068750, // Channel 6
		446081250, // Channel 7
		446093750, // Channel 8
		446106250, // Channel 9
		446118750, // Channel 10
		446131250, // Channel 11
		446143750, // Channel 12
		446156250, // Channel 13
		446168750, // Channel 14
		446181250, // Channel 15
		446193750  // Channel 16
		};

/**
 * @brief Typical CTCSS tone frequencies used in PMR446.
 */
//const uint16_t CTCSS_Tones[39] = {
//    0, 67, 71.9, 74.4, 77, 79.7, 82.5, 85.4, 88.5, 91.5,
//    94.8, 97.4, 100.0, 103.5, 107.2, 110.9, 114.8, 118.8, 123.0, 127.3,
//    131.8, 136.5, 141.3, 146.2, 151.4, 156.7, 162.2, 167.9, 173.8, 179.9,
//    186.2, 192.8, 203.5, 210.7, 218.1, 225.7, 233.6, 241.8, 250.3
//};
const uint16_t CTCSS_Tones[39] = { 0, 4390, 4712, 4875, 5046, 5223, 5406, 5596,
		5799, 5996, 6212, 6383, 6553, 6782, 7025, 7267, 7523, 7785, 8060, 8342,
		8637, 8945, 9260, 9581, 9922, 10269, 10629, 11003, 11390, 11789, 12202,
		12635, 13336, 13808, 14293, 14791, 15309, 15846, 16403 };

/**
 * @brief Writes a value to an RDA1846 register using I2C.
 * @param reg Register address.
 * @param value Value to write.
 */
void RDA1846_WriteRegister(uint16_t reg, uint16_t value) {
	if (reg > 0x7F) {
		reg = reg - 0x80;
		// Switch address
		rda1846_tx_buffer[0] = 0x7F;
		rda1846_tx_buffer[1] = 0x00;
		rda1846_tx_buffer[2] = 0x01;
		HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1,
				rda1846_tx_buffer, 3, HAL_MAX_DELAY);
		// Send data to new register
		rda1846_tx_buffer[0] = reg;
		rda1846_tx_buffer[1] = (value >> 8) & 0xFF;
		rda1846_tx_buffer[2] = value & 0xFF;
		HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1,
				rda1846_tx_buffer, 3, HAL_MAX_DELAY);
		// Switch back
		rda1846_tx_buffer[0] = 0x7F;
		rda1846_tx_buffer[1] = 0x00;
		rda1846_tx_buffer[2] = 0x00;
		HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1,
				rda1846_tx_buffer, 3, HAL_MAX_DELAY);
	} else {
		// Send data to register
		rda1846_tx_buffer[0] = reg;
		rda1846_tx_buffer[1] = (value >> 8) & 0xFF;
		rda1846_tx_buffer[2] = value & 0xFF;
		HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1,
				rda1846_tx_buffer, 3, HAL_MAX_DELAY);
	}
}

/**
 * @brief Reads a register from RDA1846.
 * @param reg Register address to read.
 * @return 16-bit register value.
 */
uint16_t RDA1846_ReadRegister(uint8_t reg) {
	uint8_t txData[1] = { reg };
	uint8_t rxData[2] = { 0 };

	HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, txData, 1,
	HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, RDA1846_I2C_ADDR << 1, rxData, 2,
	HAL_MAX_DELAY);

	return ((uint16_t) rxData[0] << 8) | rxData[1];
}

/**
 * @brief Reads a value from an RDA1846 register using I2C DMA.
 * @param reg Register address.
 * @param value Pointer to store the read value.
 */
//void RDA1846_ReadRegister(uint16_t reg, uint16_t *value) {
//    rda1846_tx_buffer[0] = (reg >> 8) & 0xFF;
//    rda1846_tx_buffer[1] = reg & 0xFF;
//    HAL_I2C_Master_Transmit_DMA(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 2);
//    HAL_I2C_Master_Receive_DMA(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_rx_buffer, 2);
//    *value = (rda1846_rx_buffer[0] << 8) | rda1846_rx_buffer[1];
//}
/**
 * @brief Sets the operating frequency of the RDA1846.
 * @param freq Frequency in Hz.
 */
void RDA1846_SetFrequency(uint32_t freq) {
	if((freq >= 134000000) & (freq <= 174000000))
		RDA1846_WriteRegister(RDA1846_BAND_REG, RDA1846_BAND_134_174MHZ);
	if((freq >= 200000000) & (freq <= 260000000))
		RDA1846_WriteRegister(RDA1846_BAND_REG, RDA1846_BAND_200_260MHZ);
	if((freq >= 400000000) & (freq <= 520000000))
		RDA1846_WriteRegister(RDA1846_BAND_REG, RDA1846_BAND_400_520MHZ);
	uint32_t freq_reg = freq * 8;
	freq_reg = freq_reg / 1000;
	uint16_t freq_reg_high = (uint16_t) ((freq >> 16) & 0x3FFF);
	uint16_t freq_reg_low = (uint16_t) (freq & 0xFFFF);
	RDA1846_WriteRegister(RDA1846_FREQ_HIGH_VALUE, freq_reg_high);
	RDA1846_WriteRegister(RDA1846_FREQ_LOW_VALUE, freq_reg_low);
}

///**
// * @brief Sets the RF band.
// * @param band Band selection value.
// */
//void RDA1846_SetBand(uint8_t band) {
//	RDA1846_WriteRegister(RDA1846_BAND_REG, band);
//}
//
///**
// * @brief Sets the reference clock frequency.
// * @param clock_source Clock frequency value in kHz.
// */
void RDA1846_SetReferenceClock(uint16_t clock_frequency) {
	// 12~14MHz
	if ((clock_frequency >= 12000) & (clock_frequency <= 14000)) {
		RDA1846_WriteRegister(RDA1846_CLOCK_MODE_REG,
		RDA1846_CLOCK_MODE_12_14MHZ);
		RDA1846_WriteRegister(RDA1846_CLOCK_XTAL_REG, clock_frequency);
		RDA1846_WriteRegister(RDA1846_CLOCK_ADCLK_REG, clock_frequency / 2);
	}
	// 24~28MHz
	if ((clock_frequency >= 24000) & (clock_frequency <= 28000)) {
		RDA1846_WriteRegister(RDA1846_CLOCK_MODE_REG,
		RDA1846_CLOCK_MODE_24_28MHZ);
		RDA1846_WriteRegister(RDA1846_CLOCK_XTAL_REG, clock_frequency / 2);
		RDA1846_WriteRegister(RDA1846_CLOCK_ADCLK_REG, clock_frequency / 4);
	}

}

/**
 * @brief Sets the device to transmit mode.
 */
void RDA1846_SetTxMode(RDA1846_REG_0x30 *settings, RDA1846_ctcss *ctcss) {
	uint16_t reg;
	reg = settings->soft_reset || settings->chip_cal_en || settings->pdn
			|| settings->sq_on || settings->vox_on ||
			RDA1846_TX_ON || settings->mute || settings->st_mode
			|| settings->tail_elim_en || settings->channel_mode;
	RDA1846_WriteRegister(0x30, reg);
	if(ctcss->tx_CTCSS != 0){
		RDA1846_WriteRegister(RDA1846_CTCSS_CDCSS_MODE_REG, ctcss->tx_CTCSS);
		RDA1846_SetCTCSS_TONE(ctcss);
	}
}

/**
 * @brief Sets the device to receive mode.
 */
void RDA1846_SetRxMode(RDA1846_REG_0x30 *settings, RDA1846_ctcss *ctcss) {
	uint16_t reg;
	reg = settings->soft_reset || settings->chip_cal_en || settings->pdn
			|| settings->sq_on || settings->vox_on ||
			RDA1846_RX_ON || settings->mute || settings->st_mode
			|| settings->tail_elim_en || settings->channel_mode;
	RDA1846_WriteRegister(0x30, reg);
	if(ctcss->rx_CTCSS != 0){
		RDA1846_WriteRegister(RDA1846_CTCSS_CDCSS_MODE_REG, ctcss->rx_CTCSS);
		RDA1846_SetCTCSS_TONE(ctcss);
	}
}


void RDA1846_Set_Reg_0x30(RDA1846_REG_0x30 *settings) {
	uint16_t reg;
	reg = settings->soft_reset || settings->chip_cal_en || settings->pdn
			|| settings->sq_on || settings->vox_on ||
			RDA1846_RX_ON || settings->mute || settings->st_mode
			|| settings->tail_elim_en || settings->channel_mode;
	RDA1846_WriteRegister(0x30, reg);
}

/**
 * @brief Sets the transmit voice channel.
 * @param channel Voice channel selection value.
 */
void RDA1846_SetTxVoiceChannel(uint8_t channel) {
	RDA1846_WriteRegister(RDA1846_TX_VOICE_CHANNEL_REG, channel);
}

/**
 * @brief Sets the PA bias for transmission.
 * @param bias PA bias value 6 bits.
 */
void RDA1846_SetTxPaBias(uint16_t bias) {
	RDA1846_WriteRegister(RDA1846_PA_BIAX_REG, bias && 0x3F);
}

/**
 * @brief Sets the CTCSS tone frequency.
 * Ctcss freq = ctcss_freq*2^16 khz.
 * It must be set to 134.4Hz when use standard cdcss mode.
 * When use ctcss/cdcss, this register must be set both in rx and tx states.
 * @param freq CTCSS frequency in format xxx.x Hz -> freq=xxxx.
 */
void RDA1846_SetCTCSS_FREQ(RDA1846_ctcss *ctcss, uint16_t freq) {
	freq = freq * 2 ^ 16;
	freq = freq / 100;
	ctcss->tone = freq;
	RDA1846_SetCTCSS_TONE(ctcss);
//	RDA1846_WriteRegister(RDA1846_CTCSS_FREQ_REG, freq);
}
/**
 * @brief Sets the CTCSS tone frequency.
 * When use ctcss/cdcss, this register must be set both in rx and tx states.
 * @param freq CTCSS tone from const uint16_t CTCSS_Tones.
 */
void RDA1846_SetCTCSS_TONE(RDA1846_ctcss *ctcss) {
	RDA1846_WriteRegister(RDA1846_CTCSS_FREQ_REG, ctcss->tone);
}

///**
// * @brief Sets the Pre-emphasis/De-emphasis filter.
// * @param enable 1 to enable, 0 to disable.
// */
//void RDA1846_SetPreEmphasisDeEmphasis(uint8_t enable) {
//	RDA1846_WriteRegister(0x0A, enable);
//}

/**
 * @brief Reads the RSSI value.
 * @return RSSI value.
 */
uint16_t RDA1846_ReadRSSI(void) {
	return RDA1846_ReadRegister(0x0B);;
}

/**
 * @brief Reads the Voice Strength Indicator (VSSI) value.
 * @return VSSI value.
 */
uint16_t RDA1846_ReadVSSI(void) {
	return RDA1846_ReadRegister(0x0C);
}

/**
 * @brief Reads status flags.
 * @return Flags value.
 */
uint16_t RDA1846_ReadFlags(void) {
	return RDA1846_ReadRegister(0x0D);
}

/**
 * @brief Parses and sends RDA1846 flags over UART using DMA.
 * @param flags The status flags read from RDA1846.
 */
void RDA1846_SendFlagsUART_DMA(uint16_t flags) {
	int offset = 0;

	offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer),
			"RDA1846 Status Flags:\r\n");

	if (flags & FLAG_SQUELCH_OPEN) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - Squelch is OPEN\r\n");
	} else {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - Squelch is CLOSED\r\n");
	}

	if (flags & FLAG_CTCSS_DETECTED) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - CTCSS Tone Detected\r\n");
	}

	if (flags & FLAG_DCS_DETECTED) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - DCS Code Detected\r\n");
	}

	if (flags & FLAG_RX_ACTIVE) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - Receiving Signal\r\n");
	}

	if (flags & FLAG_TX_ACTIVE) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - Transmitting Signal\r\n");
	}

	if (flags & FLAG_LOW_VOLTAGE) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - Low Voltage Warning!\r\n");
	}

	if (flags & FLAG_PLL_LOCK) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset, " - PLL is Locked\r\n");
	}

	if (flags & FLAG_RSSI_HIGH) {
		offset += snprintf(uart_tx_buffer + offset,
				sizeof(uart_tx_buffer) - offset,
				" - High RSSI Level Detected\r\n");
	}

	// Transmit the buffer over UART using DMA
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) uart_tx_buffer, offset);
}

