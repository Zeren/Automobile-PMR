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
const uint16_t CTCSS_Tones[39] = { 0, 4390, 4712, 4875, 5046, 5223, 5406, 5596,
        5799, 5996, 6212, 6383, 6553, 6782, 7025, 7267, 7523, 7785, 8060, 8342,
        8637, 8945, 9260, 9581, 9922, 10269, 10629, 11003, 11390, 11789, 12202,
        12635, 13336, 13808, 14293, 14791, 15309, 15846, 16403 };

/**
 * @brief Zápis hodnoty do registru RDA1846 přes I2C s návratovou hodnotou stavu.
 * Obsahuje ochranu a automatické přepínání I2C stránek.
 * @param reg Adresa registru.
 * @param value Hodnota pro zápis.
 * @return HAL_StatusTypeDef Stav I2C operace.
 */
HAL_StatusTypeDef RDA1846_WriteRegister(uint16_t reg, uint16_t value) {
    HAL_StatusTypeDef status;

    if (reg >= RDA1846_ADDR_LIMIT) {
        reg = reg - RDA1846_ADDR_LIMIT;
        
        // Přepnutí adresního prostoru nahoru (Page UP)
        rda1846_tx_buffer[0] = RDA1846_PAGE_REG;
        rda1846_tx_buffer[1] = (RDA1846_PAGE_UP >> 8) & 0xFF;
        rda1846_tx_buffer[2] = RDA1846_PAGE_UP & 0xFF;
        status = HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
        if (status != HAL_OK) return status;

        // Odeslání dat do nového registru
        rda1846_tx_buffer[0] = reg;
        rda1846_tx_buffer[1] = (value >> 8) & 0xFF;
        rda1846_tx_buffer[2] = value & 0xFF;
        status = HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
        if (status != HAL_OK) return status;

        // Přepnutí adresního prostoru zpět (Page DOWN)
        rda1846_tx_buffer[0] = RDA1846_PAGE_REG;
        rda1846_tx_buffer[1] = (RDA1846_PAGE_DOWN >> 8) & 0xFF;
        rda1846_tx_buffer[2] = RDA1846_PAGE_DOWN & 0xFF;
        return HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
    } else {
        // Zápis do běžného registru
        rda1846_tx_buffer[0] = reg;
        rda1846_tx_buffer[1] = (value >> 8) & 0xFF;
        rda1846_tx_buffer[2] = value & 0xFF;
        return HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
    }
}

/**
 * @brief Čtení z registru RDA1846.
 * @param reg Adresa registru.
 * @return 16-bitová hodnota z registru.
 */
uint16_t RDA1846_ReadRegister(uint8_t reg) {
    uint8_t txData[1] = { reg };
    uint8_t rxData[2] = { 0 };

    HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, txData, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, RDA1846_I2C_ADDR << 1, rxData, 2, HAL_MAX_DELAY);

    return ((uint16_t) rxData[0] << 8) | rxData[1];
}

/**
 * @brief Nastavení pracovní frekvence RDA1846 s korektním bitovým maskováním.
 * @param freq Frekvence v Hz.
 */
void RDA1846_SetFrequency(uint32_t freq) {
    if((freq >= 134000000) && (freq <= 174000000))
        RDA1846_WriteRegister(RDA1846_BAND_REG, RDA1846_BAND_134_174MHZ);
    else if((freq >= 200000000) && (freq <= 260000000))
        RDA1846_WriteRegister(RDA1846_BAND_REG, RDA1846_BAND_200_260MHZ);
    else if((freq >= 400000000) && (freq <= 520000000))
        RDA1846_WriteRegister(RDA1846_BAND_REG, RDA1846_BAND_400_520MHZ);
    
    // Výpočet registrové hodnoty (krok 1kHz/8)
    uint32_t freq_reg = (freq * 8) / 1000;
    
    // Extrakce a zápis
    uint16_t freq_reg_high = (uint16_t) ((freq_reg >> 16) & 0x3FFF);
    uint16_t freq_reg_low = (uint16_t) (freq_reg & 0xFFFF);
    
    RDA1846_WriteRegister(RDA1846_FREQ_HIGH_VALUE, freq_reg_high);
    RDA1846_WriteRegister(RDA1846_FREQ_LOW_VALUE, freq_reg_low);
}

void RDA1846_SetReferenceClock(uint16_t clock_frequency) {
    // 12~14MHz
    if ((clock_frequency >= 12000) && (clock_frequency <= 14000)) {
        RDA1846_WriteRegister(RDA1846_CLOCK_MODE_REG, RDA1846_CLOCK_MODE_12_14MHZ);
        RDA1846_WriteRegister(RDA1846_CLOCK_XTAL_REG, clock_frequency);
        RDA1846_WriteRegister(RDA1846_CLOCK_ADCLK_REG, clock_frequency / 2);
    }
    // 24~28MHz
    else if ((clock_frequency >= 24000) && (clock_frequency <= 28000)) {
        RDA1846_WriteRegister(RDA1846_CLOCK_MODE_REG, RDA1846_CLOCK_MODE_24_28MHZ);
        RDA1846_WriteRegister(RDA1846_CLOCK_XTAL_REG, clock_frequency / 2);
        RDA1846_WriteRegister(RDA1846_CLOCK_ADCLK_REG, clock_frequency / 4);
    }
}

/**
 * @brief Nastavení vysílacího (TX) režimu s využitím bitových polí (bitfields).
 */
void RDA1846_SetTxMode(RDA1846_Reg30_t *settings, RDA1846_ctcss *ctcss) {
    settings->bits.tx_on = 1;
    settings->bits.rx_on = 0;
    
    RDA1846_WriteRegister(0x30, settings->value);
    
    if(ctcss != NULL && ctcss->tx_CTCSS != 0){
        RDA1846_WriteRegister(RDA1846_CTCSS_CDCSS_MODE_REG, ctcss->tx_CTCSS);
        RDA1846_SetCTCSS_TONE(ctcss);
    }
}

/**
 * @brief Nastavení přijímacího (RX) režimu s využitím bitových polí (bitfields).
 */
void RDA1846_SetRxMode(RDA1846_Reg30_t *settings, RDA1846_ctcss *ctcss) {
    settings->bits.tx_on = 0;
    settings->bits.rx_on = 1;
    
    RDA1846_WriteRegister(0x30, settings->value);
    
    if(ctcss != NULL && ctcss->rx_CTCSS != 0){
        RDA1846_WriteRegister(RDA1846_CTCSS_CDCSS_MODE_REG, ctcss->rx_CTCSS);
        RDA1846_SetCTCSS_TONE(ctcss);
    }
}

/**
 * @brief Přímý zápis struktury 0x30 registru.
 */
void RDA1846_Set_Reg_0x30(RDA1846_Reg30_t *settings) {
    RDA1846_WriteRegister(0x30, settings->value);
}

void RDA1846_SetTxVoiceChannel(uint8_t channel) {
    RDA1846_WriteRegister(RDA1846_TX_VOICE_CHANNEL_REG, channel);
}

void RDA1846_SetTxPaBias(uint16_t bias) {
    RDA1846_WriteRegister(RDA1846_PA_BIAX_REG, bias & 0x3F);
}

/**
 * @brief Výpočet CTCSS tónu a zápis do registru.
 * Krok je proveden v celočíselné aritmetice, aby se zamezilo ztrátě přesnosti.
 * @param freq_10th_hz Hodnota CTCSS tónu v desetinách Hz (např. 94.7 Hz => 947)
 */
void RDA1846_SetCTCSS_FREQ(RDA1846_ctcss *ctcss, uint16_t freq_10th_hz) {
    // Vzorec z dokumentace: Ctcss freq = ctcss_freq * 2^16 kHz
    // Převedeno na celočíselný výpočet bez floating point math
    uint32_t ctcss_reg = ((uint32_t)freq_10th_hz * 65536) / 10000;
    
    ctcss->tone = (uint16_t)ctcss_reg;
    RDA1846_SetCTCSS_TONE(ctcss);
}

void RDA1846_SetCTCSS_TONE(RDA1846_ctcss *ctcss) {
    RDA1846_WriteRegister(RDA1846_CTCSS_FREQ_REG, ctcss->tone);
}

uint16_t RDA1846_ReadRSSI(void) {
    return RDA1846_ReadRegister(0x0B);
}

uint16_t RDA1846_ReadVSSI(void) {
    return RDA1846_ReadRegister(0x0C);
}

uint16_t RDA1846_ReadFlags(void) {
    return RDA1846_ReadRegister(0x0D);
}

void RDA1846_SendFlagsUART_DMA(uint16_t flags) {
    int offset = 0;

    offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer),
            "RDA1846 Status Flags:\r\n");

    if (flags & FLAG_SQUELCH_OPEN) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Squelch is OPEN\r\n");
    } else {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Squelch is CLOSED\r\n");
    }

    if (flags & FLAG_CTCSS_DETECTED) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - CTCSS Tone Detected\r\n");
    }

    if (flags & FLAG_DCS_DETECTED) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - DCS Code Detected\r\n");
    }

    if (flags & FLAG_RX_ACTIVE) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Receiving Signal\r\n");
    }

    if (flags & FLAG_TX_ACTIVE) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Transmitting Signal\r\n");
    }

    if (flags & FLAG_LOW_VOLTAGE) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Low Voltage Warning!\r\n");
    }

    if (flags & FLAG_PLL_LOCK) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - PLL is Locked\r\n");
    }

    if (flags & FLAG_RSSI_HIGH) {
        offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - High RSSI Level Detected\r\n");
    }

    HAL_UART_Transmit_DMA(&huart2, (uint8_t*) uart_tx_buffer, offset);
}
