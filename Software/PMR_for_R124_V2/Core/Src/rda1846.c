#include "rda1846.h"
#include <stdio.h>
#include <string.h>

// Buffer for I2C transmission
uint8_t rda1846_tx_buffer[3];
uint8_t rda1846_rx_buffer[32];
// Buffer for UART DMA transmission
char uart_tx_buffer[128];

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

const uint16_t CTCSS_Tones[39] = { 0, 4390, 4712, 4875, 5046, 5223, 5406, 5596,
        5799, 5996, 6212, 6383, 6553, 6782, 7025, 7267, 7523, 7785, 8060, 8342,
        8637, 8945, 9260, 9581, 9922, 10269, 10629, 11003, 11390, 11789, 12202,
        12635, 13336, 13808, 14293, 14791, 15309, 15846, 16403 };

HAL_StatusTypeDef RDA1846_WriteRegister(uint16_t reg, uint16_t value) {
    HAL_StatusTypeDef status;

    if (reg >= RDA1846_ADDR_LIMIT) {
        reg = reg - RDA1846_ADDR_LIMIT;
        
        rda1846_tx_buffer[0] = RDA1846_PAGE_REG;
        rda1846_tx_buffer[1] = (RDA1846_PAGE_UP >> 8) & 0xFF;
        rda1846_tx_buffer[2] = RDA1846_PAGE_UP & 0xFF;
        status = HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
        if (status != HAL_OK) return status;

        rda1846_tx_buffer[0] = reg;
        rda1846_tx_buffer[1] = (value >> 8) & 0xFF;
        rda1846_tx_buffer[2] = value & 0xFF;
        status = HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
        if (status != HAL_OK) return status;

        rda1846_tx_buffer[0] = RDA1846_PAGE_REG;
        rda1846_tx_buffer[1] = (RDA1846_PAGE_DOWN >> 8) & 0xFF;
        rda1846_tx_buffer[2] = RDA1846_PAGE_DOWN & 0xFF;
        return HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
    } else {
        rda1846_tx_buffer[0] = reg;
        rda1846_tx_buffer[1] = (value >> 8) & 0xFF;
        rda1846_tx_buffer[2] = value & 0xFF;
        return HAL_I2C_Master_Transmit(&hi2c1, RDA1846_I2C_ADDR << 1, rda1846_tx_buffer, 3, HAL_MAX_DELAY);
    }
}

uint16_t RDA1846_ReadRegister(uint8_t reg) {
    uint8_t rxData[2] = { 0, 0 };

    // HAL_I2C_Mem_Read automaticky použije Repeated Start místo STOP condition
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, RDA1846_I2C_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, rxData, 2, HAL_MAX_DELAY);

    if (status != HAL_OK) {
        // V případě selhání sběrnice vrátíme 0, aby se nevyhodnotily falešné příznaky (0xFFFF)
        return 0x0000;
    }

    return ((uint16_t)rxData[0] << 8) | rxData[1];
}

void RDA1846_Init(void) {
    // Přesná tovární inicializační sekvence zachycená z reálného zařízení
    const struct {
        uint8_t reg;
        uint16_t data;
    } init_seq[] = {
        { 0x30, 0x0001 }, // Soft reset
        { 0x30, 0x0004 }, // Power up (pdn_reg = 1)
        { 0x04, 0x0FD0 },
        { 0x0B, 0x1A10 },
        { 0x2B, 0x32C8 }, // Výchozí XTAL_FREQ
        { 0x2C, 0x1964 }, // Výchozí ADC_CLK
        { 0x31, 0x3FC0 }, // [KRITICKÉ] Konfigurace Charge Pump / PLL
        { 0x32, 0x627E }, // AGC parametry
        { 0x33, 0x0AF2 },
        { 0x47, 0x3BEC },
        { 0x4F, 0x1040 }, // [KRITICKÉ] Nedokumentovaný PLL/VCO parametr
        { 0x4E, 0x293A },
        { 0x56, 0x0652 },
        { 0x6E, 0x062D },
        { 0x70, 0x181B },
        { 0x71, 0x6C1E },
        { 0x85, 0x001F }, // Zápis 0x001F do registru 0x05 na stránce 1 (nad 0x7F)
        { 0x3C, 0x0A78 },
        { 0x3D, 0x200B }, // [KRITICKÉ] Nedokumentovaný parametr
        { 0x1F, 0x1001 },
        { 0x0A, 0x0340 },
        { 0x02, 0x0698 }, // [KRITICKÉ] Nedokumentovaný parametr
        { 0x54, 0x1D40 },
        { 0x0F, 0x3D24 }, // Band Select konfigurace
        { 0x30, 0x3006 }  // Finální aktivace: chip_cal_en=1, pdn_reg=1, sq_on=1
    };

    for (int i = 0; i < sizeof(init_seq) / sizeof(init_seq[0]); i++) {
        RDA1846_WriteRegister(init_seq[i].reg, init_seq[i].data);
        if (i == 0) {
            HAL_Delay(10); // Krátká stabilizační prodleva po soft resetu
        }
    }
}

void RDA1846_SetFrequency(uint32_t freq) {
    uint16_t reg0f = RDA1846_ReadRegister(RDA1846_BAND_REG);
    reg0f &= ~(0x03 << 6);

    if((freq >= 134000000) && (freq <= 174000000))
        reg0f |= (RDA1846_BAND_134_174MHZ << 6);
    else if((freq >= 200000000) && (freq <= 260000000))
        reg0f |= (RDA1846_BAND_200_260MHZ << 6);
    else if((freq >= 400000000) && (freq <= 520000000))
        reg0f |= (RDA1846_BAND_400_520MHZ << 6);

    RDA1846_WriteRegister(RDA1846_BAND_REG, reg0f);
    
    uint32_t freq_reg = (freq * 8) / 1000;
    uint16_t freq_reg_high = (uint16_t) ((freq_reg >> 16) & 0x3FFF);
    uint16_t freq_reg_low = (uint16_t) (freq_reg & 0xFFFF);
    
    RDA1846_WriteRegister(RDA1846_FREQ_HIGH_VALUE, freq_reg_high);
    RDA1846_WriteRegister(RDA1846_FREQ_LOW_VALUE, freq_reg_low);
}

void RDA1846_SetReferenceClock(uint16_t clock_frequency) {
    uint16_t reg04 = RDA1846_ReadRegister(RDA1846_CLOCK_MODE_REG);
    reg04 &= ~0x0001;

    if ((clock_frequency >= 12000) && (clock_frequency <= 14000)) {
        reg04 |= RDA1846_CLOCK_MODE_12_14MHZ;
        RDA1846_WriteRegister(RDA1846_CLOCK_MODE_REG, reg04);
        RDA1846_WriteRegister(RDA1846_CLOCK_XTAL_REG, clock_frequency);
        RDA1846_WriteRegister(RDA1846_CLOCK_ADCLK_REG, clock_frequency / 2);
    }
    else if ((clock_frequency >= 24000) && (clock_frequency <= 28000)) {
        reg04 |= RDA1846_CLOCK_MODE_24_28MHZ;
        RDA1846_WriteRegister(RDA1846_CLOCK_MODE_REG, reg04);
        RDA1846_WriteRegister(RDA1846_CLOCK_XTAL_REG, clock_frequency / 2);
        RDA1846_WriteRegister(RDA1846_CLOCK_ADCLK_REG, clock_frequency / 4);
    }
}

void RDA1846_SetTxMode(RDA1846_Reg30_t *settings, RDA1846_ctcss *ctcss) {
    settings->bits.tx_on = 1;
    settings->bits.rx_on = 0;
    RDA1846_WriteRegister(0x30, settings->value);
    
    if(ctcss != NULL && ctcss->tx_CTCSS != 0){
        uint16_t reg45 = RDA1846_ReadRegister(RDA1846_CTCSS_CDCSS_MODE_REG);
        reg45 &= ~0x0007;
        reg45 |= (ctcss->tx_CTCSS & 0x0007);
        RDA1846_WriteRegister(RDA1846_CTCSS_CDCSS_MODE_REG, reg45);
        RDA1846_SetCTCSS_TONE(ctcss);
    }
}

void RDA1846_SetRxMode(RDA1846_Reg30_t *settings, RDA1846_ctcss *ctcss) {
    settings->bits.tx_on = 0;
    settings->bits.rx_on = 1;
    RDA1846_WriteRegister(0x30, settings->value);
    
    if(ctcss != NULL && ctcss->rx_CTCSS != 0){
        uint16_t reg45 = RDA1846_ReadRegister(RDA1846_CTCSS_CDCSS_MODE_REG);
        reg45 &= ~0x0007;
        reg45 |= (ctcss->rx_CTCSS & 0x0007);
        RDA1846_WriteRegister(RDA1846_CTCSS_CDCSS_MODE_REG, reg45);
        RDA1846_SetCTCSS_TONE(ctcss);
    }
}

void RDA1846_Set_Reg_0x30(RDA1846_Reg30_t *settings) {
    RDA1846_WriteRegister(0x30, settings->value);
}

void RDA1846_SetTxVoiceChannel(uint8_t channel) {
    uint16_t reg3c = RDA1846_ReadRegister(RDA1846_TX_VOICE_CHANNEL_REG);
    reg3c &= ~0xC000;
    reg3c |= (channel & 0xC000);
    RDA1846_WriteRegister(RDA1846_TX_VOICE_CHANNEL_REG, reg3c);
}

void RDA1846_SetTxPaBias(uint16_t bias) {
    uint16_t reg0a = RDA1846_ReadRegister(RDA1846_PA_BIAX_REG);
    reg0a &= ~0x003F;
    reg0a |= (bias & 0x3F);
    RDA1846_WriteRegister(RDA1846_PA_BIAX_REG, reg0a);
}

void RDA1846_SetCTCSS_FREQ(RDA1846_ctcss *ctcss, uint16_t freq_10th_hz) {
    uint32_t ctcss_reg = ((uint32_t)freq_10th_hz * 65536) / 10000;
    ctcss->tone = (uint16_t)ctcss_reg;
    RDA1846_SetCTCSS_TONE(ctcss);
}

void RDA1846_SetCTCSS_TONE(RDA1846_ctcss *ctcss) {
    RDA1846_WriteRegister(RDA1846_CTCSS_FREQ_REG, ctcss->tone);
}

uint16_t RDA1846_ReadRSSI(void) {
    uint16_t val = RDA1846_ReadRegister(RDA1846_RSSI_REG);
    return (val & 0x03FF); // RSSI je pouze ve spodních 10 bitech
}

uint16_t RDA1846_ReadVSSI(void) {
    uint16_t val = RDA1846_ReadRegister(RDA1846_VSSI_REG);
    return (val & 0x7FFF); // VSSI je ve spodních 15 bitech
}

uint16_t RDA1846_ReadFlags(void) {
    return RDA1846_ReadRegister(RDA1846_FLAG_REG);
}
void RDA1846_SendFlagsUART_DMA(uint16_t flags) {
    int offset = 0;
    offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer), "RDA1846 Status Flags:\r\n");

    if (flags & FLAG_SQUELCH_OPEN) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Squelch is OPEN\r\n");
    else offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Squelch is CLOSED\r\n");

    if (flags & FLAG_CTCSS_DETECTED) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - CTCSS Tone Detected\r\n");
    if (flags & FLAG_DCS_DETECTED) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - DCS Code Detected\r\n");
    if (flags & FLAG_RX_ACTIVE) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Receiving Signal\r\n");
    if (flags & FLAG_TX_ACTIVE) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Transmitting Signal\r\n");
    if (flags & FLAG_LOW_VOLTAGE) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - Low Voltage Warning!\r\n");
    if (flags & FLAG_PLL_LOCK) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - PLL is Locked\r\n");
    if (flags & FLAG_RSSI_HIGH) offset += snprintf(uart_tx_buffer + offset, sizeof(uart_tx_buffer) - offset, " - High RSSI Level Detected\r\n");

    HAL_UART_Transmit_DMA(&huart2, (uint8_t*) uart_tx_buffer, offset);
}

/**
 * @brief Nastaví prahové úrovně pro otevření a zavření šumové brány.
 *
 * @param open_dbm  Úroveň v dBm pro OTEVŘENÍ Squelche (např. -113)
 * @param shut_dbm  Úroveň v dBm pro ZAVŘENÍ Squelche (např. -116)
 */
void RDA1846_SetSquelchThresholds(int8_t open_dbm, int8_t shut_dbm) {
    // Ochrana proti podtečení (datasheet definuje ofset 135)
    if (open_dbm < -134) open_dbm = -134;
    if (shut_dbm < -134) shut_dbm = -134;

    // Převod na surové hodnoty zapsatelné do registrů
    uint16_t open_reg = (open_dbm + 135) * 8;
    uint16_t shut_reg = (shut_dbm + 135) * 8;

    RDA1846_WriteRegister(0x48, open_reg);
    RDA1846_WriteRegister(0x49, shut_reg);
}
