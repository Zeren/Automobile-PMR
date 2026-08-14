# SDR PMR Transceiver – Design Description (HW rev 10)

SDR transceiver for PMR446, shared channels per VO-R/16/05.2020-6 and the 70 cm amateur band. STM32H725RGV6 processes the I/Q stream from the SX1255 and generates the remodulated signal for transmit. Primary modulation is FM; SSB is a planned extension. Prepared expansion port for a CB (26.965–27.405 MHz) module on J16.

* * *

## 1. System Architecture

```
Antenna (J4 MCX) ── LPF (TX harmonics / RX pre-filter)
                  │
                  U7  (SKY13330, FILTER_TUNE, PC5) ── tune port J3
                  │
                  U10 (SKY13330, TX/RX, PC1)
                  │      RX:
                  U19 (LNA-in SW) ── U20 PGA-103+ (LNA) ── U21 (LNA-out SW)
                  │      └─────────── bypass path ───────────┘
                  U8/U9 (BPF select, RX_BAND PC4 + inverter Q4)
                  │      ├─ external BPF PMR/shared  (J12/J13, RX only)
                  │      └─ external BPF 70 cm       (J10/J11, RX only)
                  │
SX1255 (U6) ◄── I2S (Mode B2, I/Q) + SPI6 (register map) ──► STM32H725 (U15)
TAC5142 (U11 codec) ◄── I2S2 (32 bit, target/slave) ──► STM32H725 (U15)
CB module (J16) ◄── I2S3 (32 bit, master) + I2C1 + CB_nIRQ + CB_nRESET
```

TX path: STM32 (I2S1) → SX1255 (I/Q mod, synthesizer, TX front-end) → balun U5 (NCS1-521+) → π attenuator (82R/91R/91R) → PA U4 (GRF5504) → output LPF → U7/U10 switches → J4.

## 2. RF Front-End (Semtech SX1255, U6)

*   Zero-IF I/Q transceiver, internal 3 V rails VR_PA / VR_ANA1 / VR_ANA2 / VR_DIG with 1 µF decoupling each.
    
*   TX: differential RF_ON/RF_OP through balun U5 into the PA drive network. Max +7 dBm saturated.
    
*   RX: single-ended RF_IN via switch matrix and external BPF. NF typ. 4.5 dB at max LNA gain; IIP3 −23 dBm (max gain) … +20 dBm (min gain); max RF input 0 dBm operating.
    
*   Consumption: RX 18–25 mA, TX 60–90 mA from +3V3_RF.
    

### 2.1 Clock Domain 32 MHz

*   **TCXO1 = ATX-H13-F-32.000MHz-F25-T** (CMOS output, ±2 ppm tolerance, ±2.5 ppm over −40…+85 °C, aging ±1 ppm/yr). Worst-case error ≈ ±4.9 ppm → ±2.2 kHz @ 446 MHz. Pin 1 (EN) via R137 0R → +3V3_CLOCK (fitted); R138 0R → GND (DNP) allows Hi-Z mode for external reference operation. Powered from dedicated LDO U22 (+3V3_CLOCK).
    
*   Fanout buffer U23 (74AC04MTCX), each output with its own 30R series resistor:
    *   1Y → R130 → **J16-10** (32 MHz for the CB module),
        
    *   2Y → R132 → **PH0-OSC_IN** (MCU HSE bypass),
        
    *   3Y → R134 → **PC9** (I2S_CKIN, kernel clock of I2S1/I2S2/SPI123),
        
    *   4Y → R135 → R50/R51 1k/1k divider → **XTB** of SX1255 (~1.65 Vpp, limit 1.8 Vpp).
        
*   **External reference option (DNP group):** J17 (MMCX) → R133 50R termination → C106 → clamp D16 (BAT64) → U24 (TP74AUP1G14C5 Schmitt, DNP) → R131 → U23 inputs. For external reference mode: remove R137 (TCXO1 EN → Hi-Z) and populate the DNP group.
    
*   PLL1: 32 MHz /3 ×51 → **544 MHz** SYSCLK; HCLK 272 MHz, APB 136 MHz.
    
*   USB 48 MHz from PLL3Q: 32 MHz /6 ×90 /10 – no HSI48/CRS needed.
    

### 2.2 I2S1: SX1255 ↔ STM32 (Mode B2)

| Block | Parameter | Value | Note |
| --- | --- | --- | --- |
| SX1255 | I/Q mode | **Mode B2** (`IISM=10`) | I/Q multiplexed on one data line; chip is master of CLK and WS |
|  | CLK_OUT divider (`iism_clk_div`) | **48** | 32 MHz / 48 = **666.67 kHz** = BCLK |
|  | Decimation R | **1536** (MANT=8, m=1, n=6) | fs = 32 MHz / 1536 = **20.8333 kHz** |
|  | Sample width | **16 bit / channel** | frame = 32 BCLK (16 I + 16 Q) |
|  | WS output | DIO2 (pin 23) | WS=0 → I, WS=1 → Q (Philips convention) |
| STM32 I2S1 | Role | **Slave, full-duplex** | SX1255 is clock master |
|  | Standard / format | I2S Philips, **16 bit** | main.c: `I2S_MODE_SLAVE_FULLDUPLEX`, `I2S_DATAFORMAT_16B`, 20833 Hz |
|  | Kernel clock | `i2s_ckin` (PC9, 32 MHz) | RCC mux `SPI123CLKSOURCE_PIN` |

Wiring: `I_OUT` (pin 15, RX data out) → R43/R89 → **PA6** (I2S1_SDI); MCU **PA7** (I2S1_SDO) → pin 13 `I_IN` (IQ_IN in B2); `CLK_OUT` (pin 10) → R42 → **PA5** (I2S1_CK); DIO2 (WS) → R47 → **PA4** (I2S1_WS). Frequency step: FSTEP = 32 MHz / 2^20 = **30.52 Hz** (FRF registers must be recomputed for 32 MHz reference).

### 2.3 SPI6 – SX1255 Register Interface

| CubeMX parameter | Setting | Note |
| --- | --- | --- |
| Mode | Full-Duplex Master | SCK PB12… (see GPIO table), MISO PB4, MOSI PB5, SCK PC12 |
| NSS | Soft (GPIO) | **PB3** `SPI6_nCS`, held low across whole BURST transaction |
| Data / First bit | 8 bit, MSB first | per SX1255 DS |
| CPOL / CPHA | Low / 1 Edge | Mode 0 |
| Baud | **8.5 MHz** | 136 MHz APB /16; SX1255 limit ≤10 MHz |

Frame format: first byte = `wnr` (bit7, 1=write) + address A[6:0]. On read, MISO is High-Z during the address byte; on write, MISO returns the previous register content.

## 3. Baseband & DSP (STM32H725RGV6)

*   I/Q from SX1255 via I2S1 slave; **CORDIC** for phase/amplitude demodulation (Q1.15 pairs in one 32-bit WDATA word); **FMAC** for channel FIR/IIR filtering.
    
*   Core supply: internal Direct SMPS (L14 2R2 + VCAP bank), `PWR_DIRECT_SMPS_SUPPLY`, VOS0.
    
*   Firmware: C stack (HAL + STM32 USB Device Library).
    

## 4. Audio Chain (TAC5142IRGER, U11)

Codec on I2S2 as **target/slave** (MCU master), synchronised with the radio sampling domain. Configuration is hardware-latched on MD0–MD5 at power-up (all six pins via 0R to GND; the IOVDD side is DNP).
| Pin | Setting | Meaning |
| --- | --- | --- |
| MD0 = 0 | **Target I2S mode** | slave; BCLK/FSYNC from MCU |
| MD1/MD2 = 0/0 | AVDD 3.3 V, **32 bit**, linear-phase filters |  |
| MD3 = 0 | HPF ≈ 1 Hz, quick charge off |  |
| MD4/MD5 = 0/0 | differential AC-coupled input, differential line-out |  |

> MD pins latch at power-up; MD0 must be stable ≤2 ms after rails; changing ASI mode requires a power cycle.

### I2S2 parameters

*   Role: MCU **master** → codec target; Philips, **32 bit / 32-bit frame** (64 BCLK/frame).
    
*   fs = **20.8333 kHz**; BCLK = 32 MHz / 24 = **1.3333 MHz**; kernel = i2s_ckin.
    
*   Mapping: BCLK PB10 → U11/2; FSYNC PB12 → U11/3; MCU **PB15 (SDO) → DIN** (U11/5); codec **DOUT** (U11/4) → R57/R98 → **PB14 (SDI)**.
    

### Analog audio (single +30 V domain)

Audio chain U1/U12/U14 (SA5532ADR) and U13 (TPA6120A2) run on single +30 V with virtual grounds: HALF_BIAS (10k/10k divider) and HALF_BIAS_BUFFERED / HALF BIAS MIC buffered by the two halves of U14. Codec IN2 has AC coupling C57/C58 (100 nF) toward J15. Line outputs drive U12 stage → U13 (TPA6120A2, CFA, Rf = 1k, R73 = **49R9 1.5 W** series output) → C63 (**10 µF ALU 63V** DC block) → PTC F4 → J7/J8. Both TPA6120 channels powered (+LVCC/+RVCC on +30 V). Codec rails: +3V3_CODEC from U17 (LDK320).

## 5. PA Stage (GRF5504, U4)

*   Custom tune **#229 (400–500 MHz)**: gain ~40 dB, OP1dB ~33 dBm, OIP3 ~40–42 dBm, EVB NF ~5.5 dB, Iddq ~137 mA @ 5 V.
    
*   Bias resistors R37 = 3k3 (VEN1), R38 = 6k8 (VEN2) per the 5 V bias table; bias switching Q2/Q3 from PA_BIAS_ON (PC1) with external 100k pull-down.
    
*   Target operating point: **FM ≤ 33 dBm** at the connector. SSB: backoff + IMD3 measurement open (estimate −28…−30 dBc around 26 dBm/tone).
    

### Class-G control of the +30 V rail (TLV61047, U3)

TIM3_CH1 (PC6) PWM 25 kHz (PSC=3, ARR=2719 @ 272 MHz) is filtered (R30 91k + C29) and injected through R32 22k into the FB node. KCL at FB with VREF = 1.233 V: (VOUT − VREF)/430k + (VCTRL − VREF)/(91k + 22k) = VREF/22k → **VOUT ≈ 28.2 V @ 0 % duty … 20.4 V @ 100 % duty**, ~3 mV/LSB, linear inverse. **Firmware must clamp duty** (VREF tol. ±2 % and TPA6120/SA5532 30–33 V limits). Startup: PWM started only after rails settle.

## 6. Filters & Switch Matrix

### 6.1 Output LPF with shunt-inductor ESD protection

Series L8–L11 (19n/38n/38n/19n) + C53/C55/C56 (8p5/8p5/9p1) + **shunt inductors L12/L13 (20 nH air-core) to GND** near J4. The shunt L provides a DC/low-frequency bleed path for ESD charge instead of a diode clamp (no parasitic capacitance on the RF line; harmless at 432–450 MHz). Simulated S-parameters: **S11 < −20 dB over 432–450 MHz, IL ≤ ~0.15 dB, 2nd harmonic (860–900 MHz) −54…−58 dB**. Use air-core/wire-wound parts (no ferrite multilayer – saturation); re-tune C53 after PCB parasitics.

### 6.2 Switch truth table (SKY13330, VBATT = +3V3_RF)

| Switch | Function | CTRL (pin 7) | CTRL=1 | CTRL=0 |
| --- | --- | --- | --- | --- |
| **U10** | TX/RX path | PA_BIAS_ON (PC1) | TX (PA path) | RX |
| **U8/U9** | external BPF select | RX_BAND (PC4), inverter Q4 | 70 cm (J10/J11) | **PMR/shared (J12/J13)** |
| **U7** | antenna vs tune | FILTER_TUNE (PC5) | tune port J3 | antenna J4 |
| **U19/U21** | **LNA in/out** | **LNA ON (PC7)** | **LNA (U20) in path** | **bypass** (default after POR) |

CTRL level via 10k/22k divider ≈ **2.27 V** (valid window 1.65–2.7 V; ≤ VDD). Isolated ports are reflective-short.

### 6.3 Switchable LNA (U20 = PGA-103+)

*   Located between U10 and the BPF matrix, bypassed by U19/U21 (default bypass after POR via R123 100k pull-down on PC7).
    
*   Supply: +3V3_RF → Q8 (NX3008PBKW load switch, gate via Q9/Q10) → L16 (LQW18ANR10) + R124 1k Q-damping → U20 pin 3. Operating point ≈ 3.0–3.1 V, ~60 mA: gain ≈ 21.5 dB @ 0.4 GHz, NF ≈ 0.5 dB, P1dB ≈ +18…19 dBm, OIP3 ≈ +34 dBm.
    
*   Low-frequency stabilisation on input: C98 (100p) + L17 (330 nH) → C101 (270 pF) → R125 (120R) trap (validated in AWR; PGA-103+ is conditionally stable below ~200 MHz).
    
*   Combined RX chain NF: ~7–8 dB without LNA → ~2 dB with LNA.
    
*   **Firmware: auto-bypass by RSSI mandatory** (LNA sits before the BPF; blockers amplified). Reduce SX1255 internal LNA gain (RxLnaGain G2/G3) when external LNA active. Two RSSI calibration tables (bypass / LNA-on, ~21 dB offset).
    

## 7. Power Supply

| Rail | Converter | Values | Note |
| --- | --- | --- | --- |
| +26V input | J6/J7/J8, **F1 0458004.DR (4 A)**, D7 SMDJ33A | standoff 33 V | 24 V vehicle systems |
| **+5V** | U2 SIC479ED buck | 52k3/10k → 4.98 V; fSW 39k; MODE 10k; ILIMIT→VDD (4 A valley); L1 FTKB1207-6R1 | FR1 ferrite at input |
| **+30V** | U3 TLV61047 boost | 430k/22k + PWM injection (§5); L2 FTC404020S100MGCA; D4 DFLS240L | fed directly from +5 V |
| **+3V3_MCU** | U16 TLV62130 buck | 750k/240k → 3.30 V; EN=PG_5V; PG→PG_3V3; FSW=VOUT; SS 3n3; L15 2R2 | input ferrite FR3 |
| **+3V3_CODEC** | U17 LDK320 | EN=PG_5V; COUT ≈ 11 µF | ferrite FR4; codec AVDD+IOVDD |
| **+3V3_RF** | U18 LDK320 | EN=PG_5V; COUT ≈ 18 µF | ferrite FR5; SX1255 + RF switches + LNA |
| **+3V3_CLOCK** | U22 LDK320 | EN=IN (always on); COUT ≈ 13 µF | ferrite FR6; TCXO + U23 (+U24 DNP) |
| MCU core | internal SMPS | Direct SMPS, L14 2R2 + VCAP bank | PWR_DIRECT_SMPS_SUPPLY |

*   All ferrites: 742792023 (120 Ω @100 MHz, 3 A). Local ceramics at each IC per its layout guide.
    
*   Sequencing: SIC479 PGOOD → enables U3/U16/U17/U18 (PG_5V); TLV62130 PG → PG_3V3 (LED).
    
*   J16 carries **+POWER_RAIL** (26 V behind F1/TVS/FR2) with C101 = 10 µF ALU at the header.
    

## 8. USB (OTG HS, FS embedded PHY)

Composite device: Audio Control, AS OUT (playback, async + feedback), AS IN (record / I/Q), CDC (CAT). VBUS sensing PA9 via 10k/39k divider. USB clock: PLL3Q 48 MHz.
**Asynchronous UAC at 20.833 kHz:** declared 24 kHz nominal rate would be wrong — device works with **mostly 21 samples, sometimes 20 samples per 1 ms SOF** (average 20.833) on the IN endpoint; host-side async resampling handles the rest (same mechanism as before, recalculated for 32 MHz).

## 9. MCU Configuration (CubeMX rev)

*   PLL: HSE_BYPASS 32 MHz → M=3, N=51, P=1 → 544 MHz; HCLK 272, APB 136 MHz.
    
*   I2S1 slave FD 16 bit 20833 Hz; I2S2 master FD 32 bit; I2S3 master FD 32 bit (CB, J16).
    
*   SPI6 /16 → 8.5 MHz; kernel SPI123 = i2s_ckin (RCC mux `RCC_SPI123CLKSOURCE_PIN`).
    
*   I2C1 timing 0xA0303048 (~100 kHz @136 MHz) – external buses J15/J16.
    
*   TIM3 CH1 PC6 PWM 25 kHz for class-G; UART4 (panel), UART5 (debug: TX PB13, RX PD2).
    
*   USB: PLL3Q 48 MHz; FS embedded PHY, VBUS sensing.
    
*   Debug: **Serial Wire only** (JTAG pins reused).
    
*   Verify in generated MSP: `HAL_RCCEx_PeriphCLKConfig()` with PLL3 = 32/6/90/Q=10 for USB and SPI123 kernel = pin clock.
    

### GPIO map

| Signal | Pin | Type | Function |
| --- | --- | --- | --- |
| RADIO_RESET | PC0 | In/Out | SX1255 reset (OD emulation: PP high 100 µs → Hi-Z, 5 ms wait) |
| **PA_BIAS_ON** | PC1 | Out PP | 1 = TX (PA bias + U10 switch); 100k ext pull-down |
| **RX_BAND** | PC4 | Out PP | 0 = PMR/shared BPF (J12/J13), 1 = 70 cm BPF (J10/J11) |
| **FILTER_TUNE** | PC5 | Out PP | 1 = tune port J3 instead of antenna |
| **LNA ON** | PC7 | Out PP | 1 = LNA in path; 0 = bypass (POR default) |
| **MCU 30V CTRL** | PC6 | AF (TIM3_CH1) | class-G PWM 25 kHz |
| PTT_TO_MCU | PA2 | In (pull-up) | 0 = TX; debounce in FW |
| PTT_LED | PA3 | Out PP | red TX LED |
| **SPI6_nCS** | PB3 | Out PP | SX1255 NSS (manual, across BURST) |
| **CB_nIRQ** | PB8 | In EXTI falling, pull-up | CB module interrupt |
| **CB_nRESET** | PB9 | Out PP, init HIGH | CB module reset |
| I2S_CKIN | PC9 | AF input | 32 MHz kernel clock |
| SWDIO/SWCLK | PA13/PA14 | AF | programming via J5 |
| UART4 TX/RX | PA0/PA1 | AF | control panel |
| UART5 TX/RX | PB13/PD2 | AF | debug console (J5) |

## 10. Connectors

**J5 – service (53047-1010):** 1 SWDIO, 2 SWCLK, 3 NRST, 4 UART_DEBUG_TX, 5 UART_DEBUG_RX, 6 I2C_SCL, 7 I2C_SDA, 8 +5V, 9 +3V3, 10 GND.
**J16 – CB module (61301621121):** 1/2/11/12 GND, 3 I2S3_CLK, 4 I2C_SCL, 5 I2S3_DIN, 6 I2C_SDA, 7 I2S3_DOUT, 8 +5V, 9 I2S3_WS, **10 = 32 MHz clock out**, 13 CB_nIRQ, 14/16 +POWER_RAIL (26 V), 15 CB_nRESET.
**RF:** J4 antenna MCX; J3 tune port MMCX; J10/J11 external BPF 70 cm; J12/J13 external BPF PMR/shared; J17 external 32 MHz reference input (DNP option); J2 UFL (test).

### M12 connector

| M12 | Function | Wire color |
| --- | --- | --- |
| 1 | +26V | Black |
| 2 | PTT | Brown |
| 3 | Mic+ | Red |
| 4 | Mic- | Orange |
| 5 | NC | Yellow |
| 6 | Speaker | Green |
| 7 | GND | Blue |
| 8 | NC | Violet |

### R123

| Cont. |  | Cont. |  | Cont. |  |
| --- | --- | --- | --- | --- | --- |
| a1 |  | b1 | +26V (Black) | c1 |  |
| a2 |  | b2 |  | c2 | GND (Blue) |
| a3–a8 |  | b3–b9 |  | c3–c8 |  |
| a9 |  | b9 |  | c9 | PTT (Brown) |
| a0 | Speaker (Green) | b0 | MIC+ (Red) | c0 | MIC- (Orange) |

### R124 A-2

| Cont. |  | Cont. |  |
| --- | --- | --- | --- |
| a1 | A-1 R112 MIC+ | b1 | A-1 R123 MIC+ |
| a2 | A-1 R112 MIC- | b2 | A-1 R123 MIC- |
| a3 | Tangenta PTT | b3 | Tangenta MIC+ |
| a4 | Intercom MIC to PA | b4 | Tangenta MIC- |
| a5 | GND | b5 | A-1 R112 Speaker |
| a6 | +22V for A-2..A-5 | b6 | A-1 R112 PTT |
| a7 | Tangenta Speaker | b7 | A-1 R123 Speaker |
| a8 | Intercom Speaker | b8 | A-1 R123 PTT |

## PMR 446 Technical Specification

*   Transmit Power 500 mW ERP
    
*   Bandwidth 12.5 kHz
    
*   Antenna fixed, non removable
    
*   Frequency Modulation and digital modes
    
*   User Separation CTCSS, DCS, digital signalling
    
*   Frequencies (Simplex TX=RX)
    

## 11. Open Items / Bench Checklist

1.  Verify amplitudes after assembly: ~3.3 Vpp CMOS at U23 inputs, ~1.65 Vpp at XTB (U6-8).
    
2.  External reference mode (if used): input at U24 must provide ≥ ~1.6 Vpp to clear the TP74AUP1G14C5 thresholds; rework = remove R137, populate U24 + J17/R133/D16 group.
    
3.  SSB: measure GRF5504 IMD3 on the devboard, set operating backoff.
    
4.  PMR446 "fixed antenna" requirement vs J4 connector – documentation decision.
    
5.  MICBIAS (U11-14) unused with dynamic microphone – leave with local 1 µF only.
    
6.  AWR re-check of LPF with real air-coil models (DCR/SRF) and sweep to ≥1.4 GHz (3rd harmonic).
    
7.  LNA stability/NF verification with the complete RX cascade on the prototype.
    

## Layout Checklist (for the frozen schematic)

*   GRF5504 power pad: multiple 8-mil vias per eval board guidance; TPA6120 PowerPAD soldered with vias; TAC5142 thermal pad grounded; SKY13330 exposed pads low-impedance GND.
    
*   SIC479 input/PGND loops, BOOT cap placement, AGND single-point, TLV61047 SW node minimal, TLV62130 VOS tapped at C_out.
    
*   LDO input/output caps at pins (COUT ≤ 22 µF per rail), ferrites directly in input paths.
    
*   Clock domain (TCXO1 → R136 → U23 → fanout) short, away from USB D±, audio and J16/J17; no 32 MHz trace over GND plane splits.
    
*   +26V/fuse/ESD at board edge near J6–J8; J16 +26V branch traces sized ≥3 A.