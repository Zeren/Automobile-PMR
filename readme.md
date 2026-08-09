# PMR radio for R124

# Connection to R124

## Connection in R124 A-1
### Connection as R112. 

| Cont. |                | Cont. |                     | Cont. |                |
|-------|----------------|-------|---------------------|-------|----------------|
| a1    |                | b1    | +26V <br/> Black    | c1    |                |
| a2    |                | b2    | Speaker <br/> Green | c2    | GND <br/> Blue |
| a3    | PTT<br/> Brown | b3    | MIC- <br/>Orange    | c3    | MIC+ <br/> Red |
| a4    |                | b4    |                     | c4    |                |
| a5    |                | b5    |                     | c5    |                |
| a6    |                | b6    |                     | c6    |                |
| a7    |                | b7    |                     | c7    |                |
| a8    |                | b8    |                     | c8    |                |
| a9    |                | b9    |                     | c9    |                |
| a0    |                | b0    |                     | c0    |                |

### Connection as R123. 

| Cont. |                     | Cont. |                  | Cont. |                  |
|-------|---------------------|-------|------------------|-------|------------------|
| a1    |                     | b1    | +26V <br/> Black | c1    |                  |
| a2    |                     | b2    |                  | c2    | GND <br/> Blue   |
| a3    |                     | b3    |                  | c3    |                  |
| a4    |                     | b4    |                  | c4    |                  |
| a5    |                     | b5    |                  | c5    |                  |
| a6    |                     | b6    |                  | c6    |                  |
| a7    |                     | b7    |                  | c7    |                  |
| a8    |                     | b8    |                  | c8    |                  |
| a9    |                     | b9    |                  | c9    | PTT<br/> Brown   |
| a0    | Speaker <br/> Green | b0    | MIC+ <br/> Red   | c0    | MIC- <br/>Orange |

## M12 Connector

| M12 | Function | Wire color |
|-----|----------|------------|
| 1   | +26V     | Black      |
| 2   | PTT      | Brown      |
| 3   | Mic+     | Red        |
| 4   | Mic-     | Orange     |
| 5   | NC       | Yellow     |
| 6   | Speaker  | Green      |
| 7   | GND      | Blue       |
| 8   | NC       | Violet     |

![img.png](Doc/Pic/M12_radio_connector.png)

## Updates for v2

- Add resistors to feedback of speaker amplifier to add gain
- Lower values of resistors in feedback of mic amplifier to make it stable
- Lower gain of mic amplifier
- Add connection for BT audio input/output for hands free
  - Make it as module to 2.54mm  pin header
  - ESP32 and ES8388 look like good choice

# PMR 446 Technical Specification

- Transmit Power 500mW Estimated (REP)
- Bandwidth 12.5kHz 
- Antenna Fixed, non removable. Impossible to connect to external aerial.
- Frequency Modulation and digital modes.
- User Separation CTCSS, DCS, digital signalling.
- Frequencies (Simplex TX=RX)

## Channels
| PMR446 Channel | Frequency (MHz) |
|----------------|-----------------|
| 1              | 446,00625       | 
| 2              | 446,01875       |
| 3              | 446,03125       |
| 4              | 446,04375       |
| 5              | 446,05625       |
| 6              | 446,06875       |
| 7              | 446,08125       |
| 8              | 446,09375       |
| 9              | 446,10625       |
| 10             | 446,11875       |
| 11             | 446,13125       |
| 12             | 446,14375       |
| 13             | 446,15625       |
| 14             | 446,16875       |
| 15             | 446,18125       |
| 16             | 446,19375       |

## CTCSS codes

| Code | Tone Hz | Code | Tone Hz | Code | Tone Hz |
|------|---------|------|---------|------|---------|
| 0    | Off     | 13   | 103.5   | 26   | 162.2   |
| 1    | 67.0    | 14 	 | 107.2   | 27   | 167.9   |
| 2    | 71.9    | 15 	 | 110.9   | 28   | 173.8   |
| 3    | 74.4    | 16   | 114.8   | 29   | 179.9   |
| 4    | 77.0    | 17   | 118.8   | 30   | 186.2   |
| 5    | 79.7    | 18   | 123.0   | 31   | 192.8   |
| 6    | 82.5    | 19   | 127.3   | 32   | 203.5   |
| 7    | 85.4    | 20   | 131.8   | 33   | 210.7   |
| 8    | 88.5    | 21   | 136.5   | 34   | 218.1   |
| 9    | 91.5    | 22   | 141.3   | 35   | 225.7   |
| 10   | 94.8    | 23   | 146.2   | 36   | 233.6   |
| 11   | 97.4    | 24   | 151.4   | 37   | 241.8   |
| 12   | 100.0   | 25   | 156.7   | 38   | 250.3   |

# R124 Notes

## Connectors

### R124 A-1

| Cont. |                                              | Cont. |                      | Cont. |               |
|-------|----------------------------------------------|-------|----------------------|-------|---------------|
| a1    | NC                                           | b1    | +26V                 | c1    | NC            |
| a2    | NC                                           | b2    | R112 Speaker         | c2    | GND           |
| a3    | R112 PTT                                     | b3    | R112 MIC-            | c3    | MIC+          |
| a4    | A-2 R112 Speaker                             | b4    | A-2 R123 MIC+        | c4    | A-2 R112 MIC+ |
| a5    | A-2 R123 Speaker                             | b5    | A-2 R123 MIC-        | c5    | A-2 R112 MIC- |
| a6    | Tangenta Speaker                             | b6    | Tangenta Intercom PB | c6    | Tangenta GND  |
| a7    | Tangenta MIC+                                | b7    | Tangenta MIC-        | c7    | Tangenta PTT  |
| a8    | Intercom Speaker for <br/>A-2, A-3, A-4, A-5 | b8    | Intercom PB          | c8    | GND           |
| a9    | +22V for <br/>A-2, A-3, A-4, A-5             | b9    | Intercom MIC to PA   | c9    | R123 PTT      |
| a0    | R123 Speaker                                 | b0    | R123 MIC+            | c0    | R123 MIC-     |

Intercom MIC to PA - Amplified microphone

### R124 A-2

| Cont. |                                  | Cont. |                  | 
|-------|----------------------------------|-------|------------------|
| a1    | A-1 R112 MIC+                    | b1    | A-1 R123 MIC+    |
| a2    | A-1 R112 MIC-                    | b2    | A-1 R123 MIC-    |
| a3    | Tangenta PTT                     | b3    | Tangenta MIC+    |
| a4    | Intercom MIC to PA               | b4    | Tangenta MIC-    |
| a5    | GND                              | b5    | A-1 R112 Speaker |
| a6    | +22V for <br/>A-2, A-3, A-4, A-5 | b6    | A-1 R112 PTT     |
| a7    | Tangenta Speaker                 | b7    | A-1 R123 Speaker |
| a8    | Intercom Speaker                 | b8    | A-1 R123 PTT     |

# Architektura SDR Transceiveru

## 1. Hodinová synchronizační doména (Clock Tree)

* **Hlavní oscilátor:** Společné 36MHz TCXO pro celou desku.
* **Distribuce signálu (Hardware Split):**
* Větev pro mikrokontrolér vedena přímo do vstupu `OSC_IN` s plnou logickou úrovní.
* Větev pro RF front-end vedena do vstupu `XTB` přes tlumicí člen (např. odporový dělič s AC vazbou), aby nebyla překročena maximální povolená špičková úroveň 1.8 Vpp. Vstup `XTA` zůstává nezapojený.




* **Zpracování beze skluzu:** Všechny vzorkovací frekvence v systému (I/Q data i audio) jsou odvozeny čistým celočíselným dělením z 36 MHz. Tím je zcela eliminována nutnost použití asynchronního převzorkování (ASRC) nebo zlomkových PLL.

## 2. RF Front-End (Semtech SX1255)

* **Architektura:** Zero-IF transceiver.


* **Digitální rozhraní:** Využit integrovaný Digital Bridge v režimu **Mode B2** (I2S kompatibilní datový tok, multiplexované I a Q signály na jednom pinu).


* **Decimace na čipu:** Nastavena na hodnotu **1536** pomocí 1. sady parametrů ($\text{MANT}=8$).


* **Vzorkovací frekvence (I/Q):** Odvozena z krystalu na $23.4375\text{ kHz}$.


* **Formát rámce:** Parametr `CLK_OUT/XTAL` nastaven na dělení 24. Výstupní hodiny $f_{CLK\_OUT}$ běží na $1.5\text{ MHz}$, což generuje standardní I2S rámec o délce 64 pulzů (32 bitů na složku). Do DMA mikrokontroléru se ukládá pouze 16 bitů (poskytujících dynamický rozsah cca $98\text{ dB}$).



## 3. Baseband a DSP (STM32H725RGV6)

* **Sběr rozhraní:** Vyčítání SX1255 probíhá přes standardní I2S (nebo SAI) periferii v režimu Master. MCU generuje BCLK a WS.
* **Matematická koprocese:**
* **CORDIC:** Využit pro hardwarově akcelerovanou fázovou a amplitudovou demodulaci. Vstupní 16bitová data (formát Q1.15) z DMA se prokládají do jediného 32bitového slova pro zápis do `WDATA` registru koprocesoru.


* **FMAC:** K dispozici jako Filter Mathematical Accelerator pro dodatečné rychlé kanálové FIR/IIR filtrování před samotnou demodulací.




* **Firmware:** Napsán v jazyce Rust nad asynchronním frameworkem Embassy (využívající `embassy-stm32` a `embassy-executor`), což umožňuje bezblokující řízení I2S DMA přeskoků.

## 4. Audio Front-End a koncový stupeň

* **Audio Kodek:** TAC5142 v režimu Slave. BCLK a WS generuje STM32H7. Vzorkovací frekvence audia je buď nativních $23.4375\text{ kHz}$, nebo po jedné decimaci/half-band filtru $11.71875\text{ kHz}$.
* **Mikrofonní vstup:** Sovětské uhlíkové vložky nahrazeny moderními elektretovými nebo dynamickými kapslemi. Předpětí (bias) je řešeno standardně přes vývod `MICBIAS` kodeku TAC5142 s vazbou do diferenciálního vstupu PGA.
* **Audio koncový stupeň:** Zesilovač TPA6120A2 řídící sluchátka v sovětské tankistické kukle.
* **Napájení:** Asymetrické (až 30 V) z dedikovaného step-up DC/DC měniče (vstup 5 V). Výstupní napětí z měniče bude možné dynamicky řídit (princip třídy G/H) pro maximalizaci účinnosti PA na základě reálného rozkmitu.
* **Virtuální střed:** Realizován ratiometrickým odporovým děličem z DC/DC napájení, který je posílen přes volný operační zesilovač v zapojení napěťového sledovače. Toto zajišťuje tvrdý střed s nízkou výstupní impedancí (odstranění přechodových subsonických rázů a přeslechů).
* **Specifika CFA topologie:** TPA6120A2 je zesilovač s proudovou zpětnou vazbou. Zpětnovazební rezistor $R_f$ má fixní hodnotu $1\text{ k}\Omega$ a absolutně se vynechává jakákoliv kapacita v paralelní zpětnovazební větvi. Na DPS je striktně odstraněn měděný polygon pod piny invertujících vstupů (IN-). Výstup je oddělen sériovým blokovacím DC kondenzátorem $220\ \mu\text{F}$.



## 5. RF Koncový stupeň (Transmitter PA)

* **Výkonový stupeň:** InGaP HBT tranzistor Guerrilla RF GRF5504 vybuzený na kompresní hranu, dodávající na konektor výkon $34.8\text{ dBm}$ (zhruba $3\text{ W}$), což je ideální pro sdílené kmitočty v ČR ($448\text{ MHz} / 449\text{ MHz}$).
* **Výstupní filtrace:** Modifikovaný dolní propust (LC Čebyšev), kde jsou nuly přenosu (LC pasti) úmyslně a přesně naladěny na eliminaci 2. a 3. (případně 4.) harmonické. Tím je dosaženo extrémní strmosti přesně v pásmech, kam by komprimující GRF5504 produkoval největší intermodulační rušení, při zachování minimálního vložného útlumu v propustném pásmu.

## 6. Uživatelské a ladicí rozhraní (HMI & Debug)

* **Externí ovládací panel:** Volitelné příslušenství. Připojení je realizováno asynchronně přes sériovou linku **UART4**. Panel není nezbytný pro základní chod transceiveru (jádro SDR může běžet zcela autonomně).
* **Debug a programování:** Sdružený servisní konektor.
  * **Diagnostická konzole:** Linka **UART7** je vyhrazena výhradně pro výpis běhových logů (např. logování z asynchronních úloh v Embassy) a interní diagnostiku.
  * **Programovací rozhraní:** Využity standardní signály **SWDIO** a **SWCLK** pro flashování firmwaru a on-chip debugging.


## Parametry pro I2S propojení (SX1255 $\leftrightarrow$ STM32H725)

| Architektonický blok        | Parametr                         | Hodnota / Nastavení                                                | Popis a výpočet                                                                                                |
|-----------------------------|----------------------------------|--------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------|
| **Hodinová doména**         | Základní takt krystalu (XTAL)    | **36 MHz**                                                         | Společný fyzický krystal pro MCU i SX1255.                                                                     |
| **SX1255: Digital Bridge**  | Provozní režim I/Q               | **Mode B2**                                                        | Plně I2S kompatibilní. I a Q složky jsou multiplexovány na jedné datové lince.                                 |
|                             | Sada parametrů (MANT)            | **8** (1st set)                                                    | V registru `0x13` bit `int_dec_mantisse = 0`.                                                                  |
|                             | Parametr $m$                     | **1**                                                              | V registru `0x13` bit `int_dec_m_parameter = 1`.                                                               |
|                             | Parametr $n$                     | **6**                                                              | V registru `0x13` bity `int_dec_n_parameter = 6`.                                                              |
|                             | **Celková decimace** ($R$)       | **1536**                                                           | Vzorec: $R = 8 \times 3^1 \times 2^6 = 1536$.                                                                  |
|                             | Dělička výstupních hodin         | **48**                                                             | V registru `0x12` bity `iism_clk_div = 1000`. Dělí XTAL pro získání bitových hodin (BCLK).                     |
| **SX1255: I2S Výstup**      | Vzorkovací frekvence ($f_s$)     | **23.4375 kHz**                                                    | $36\text{ MHz} / 1536 = 23.4375\text{ kHz}$.                                                                   |
|                             | Frekvence hodin ($f_{CLK\_OUT}$) | **750 kHz**                                                        | $36\text{ MHz} / 48 = 750\text{ kHz}$. Toto je sériový Bit Clock (BCLK).                                       |
|                             | Rozlišení vzorku (Data Size)     | **16 bitů**                                                        | Pro děličku 48 a decimaci 1536 generuje čip nativně 16 bitů na kanál.                                          |
|                             | Počet pulzů na rámec (Frame)     | **32 bitů**                                                        | 16 bitů (I) + 16 bitů (Q). Zkouška: $750\text{ kHz} / 32 = 23.4375\text{ kHz}$.                                |
| **STM32CubeMX (I2S / SAI)** | Role na sběrnici                 | **Slave**                                                          | **Klíčové:** V módu B je SX1255 vždy Master a generuje hodiny i Word Select.                                   |
|                             | Standard protokolu               | **I2S Philips**                                                    | Word Select (WS) o 1 takt předchází datům, což přesně odpovídá logice Mode B v SX1255.                         |
|                             | Data Format / Data Size          | **16 Bits Data on 16 Bits Frame**                                  | Pro každý z obou kanálů (I i Q). Tím MCU očekává přesně 32 hodinových cyklů na jeden WS rámec.                 |
|                             | Audio Frequency                  | **23.4375 kHz**                                                    | Odpovídá našemu výpočtu (v CubeMX se v režimu Slave většinou dopočítá z BCLK, ale je dobré to mít na zřeteli). |
|                             | Mapování pinů (Příjem z RX)      | **BCLK:** `CLK_OUT`<br><br>**WS:** `DIO2`<br><br>**SD_RX:** `I_IN` | Fyzické propojení signálů pro příjem. (Pro vysílání se z MCU použije `SD_TX` do `I_OUT`).                      |

### Důležité poznámky pro realizaci a CubeMX:

1. **STM32 jako Slave:** Přestože mikrokontrolér STM32H7 bude řídit celou logiku, z pohledu této konkrétní datové sběrnice (přenos I/Q dat) musí být jeho I2S/SAI nastaveno jako **Slave**. SX1255 si sám časuje vyhazování dat pomocí svého interního děliče hodin.


2. **Polarita Word Select (WS):** Protokol SX1255 (Mode B2) určuje, že když je WS na logické "0", přenáší se I (In-phase) složka, a při logické "1" se přenáší Q (Quadrature) složka. Standardní I2S Philips v STM32 funguje přesně takto (levý kanál na nule, pravý na jedničce), takže se ti I a Q složky namapují naprosto korektně a stabilně do levého a pravého kanálu v DMA bufferu.



## Konfigurace I2S a vnitřních bloků (STM32H725 $\leftrightarrow$ TAC5142)

| Architektonický blok          | Parametr                     | Hodnota / Nastavení                                                                      | Popis a výpočet                                                                                                                               |
|-------------------------------|------------------------------|------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------|
| **TAC5142: Hardware Control** | `MD0` (Režim a Rozhraní)     | **0 (GND)**                                                                              | **Target I2S Mode**. TAC5142 je Slave, očekává BCLK a FSYNC z mikrokontroléru.                                                                |
|                               | `MD1` a `MD2` (Audio formát) | **0 (GND) / 0 (GND)**                                                                    | AVDD = 3.3 V, délka slova = **32 bitů**, Linear-phase filtr.                                                                                  |
|                               | `MD3` (HPF a Quick Charge)   | **0 (GND)**                                                                              | **HPF Cut-off = 1 Hz** (@ 48 kHz). Input Cap Quick Charge = Disabled (50 ms).                                                                 |
|                               | `MD4` a `MD5` (Analog I/O)   | **0 (GND) / 0 (GND)**                                                                    | **Vstup:** Diferenciální, AC-vazba.<br><br>**Výstup:** Diferenciální, Line-out.                                                               |
| **Hodinová doména (Z MCU)**   | Vzorkovací frekvence ($f_s$) | **23.4375 kHz**                                                                          | Určena signálem FSYNC (Word Select) generovaným z MCU, fázově svázáno s SX1255.                                                               |
|                               | Frekvence Bit Clock (BCLK)   | **1.5 MHz**                                                                              | Pro 32bitový I2S rámec: $23.4375\text{ kHz} \times (32\text{ bitů} \times 2\text{ kanály}) = 1.5\text{ MHz}$. MCU musí přesně toto generovat. |
|                               | Poměr BCLK / FSYNC           | **64**                                                                                   | Očekáváno v Target I2S Mode pro 32bitové slovo na levý i pravý kanál ($32 + 32 = 64$).                                                        |
| **STM32CubeMX (I2S / SAI)**   | Role na sběrnici             | **Master**                                                                               | MCU musí generovat BCLK a WS (FSYNC) pro TAC5142.                                                                                             |
|                               | Standard protokolu           | **I2S Philips**                                                                          | Odpovídá Target I2S Mode na TAC5142.                                                                                                          |
|                               | Data Format / Data Size      | **32 Bits Data on 32 Bits Frame**                                                        | Zajišťuje vygenerování 64 BCLK pulzů na jeden FSYNC rámec.                                                                                    |
|                               | Audio Frequency              | **23.4375 kHz**                                                                          | Cílová vzorkovací frekvence, ze které MCU přes interní děličku odvodí BCLK.                                                                   |
|                               | Mapování pinů                | **BCLK:** `BCLK`<br><br>**WS:** `FSYNC`<br><br>**SD_RX:** `DOUT`<br><br>**SD_TX:** `DIN` | Fyzické propojení signálů pro obousměrný přenos audia.                                                                                        |

Pro správnou komunikaci s RF front-endem SX1255 je nutné SPI3 nakonfigurovat podle specifikací v katalogovém listu. Digitální rozhraní SX1255 je plně synchronní a podporuje jak izolovaný přístup (SINGLE), tak blokový přenos (BURST) s automatickou inkrementací adresy.

Zde je kompletní přehled nastavení pro STM32CubeMX:

## Konfigurace SPI3 pro SX1255

| Parametr v STM32CubeMX    | Nastavení / Hodnota                      | Popis a hardwarová specifikace SX1255                                                                                                                                                                                                                                                                            |
|---------------------------|------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Mode**                  | **Full-Duplex Master**                   | Klasická čtyřvodičová topologie (MOSI, MISO, SCK a CS/NSS).                                                                                                                                                                                                                                                      |
| **Hardware NSS Signal**   | **Disable** *(doporučeno)*               | Přestože lze použít hardwarový NSS, pro BURST přístup do registrů je nezbytné řídit NSS (Chip Select) softwarově přes standardní GPIO (Output Push-Pull). V BURST režimu totiž musí NSS zůstat v logické `0` nepřetržitě mezi jednotlivými datovými bajty a do logické `1` přejít až po ukončení celé transakce. |
| **Frame Format**          | **Motorola**                             | Standardní topologie specifikovaná výrobcem.                                                                                                                                                                                                                                                                     |
| **Data Size**             | **8 Bits**                               | Adresování i datové toky jsou striktně 8bitové.                                                                                                                                                                                                                                                                  |
| **First Bit**             | **MSB First**                            | Nejdůležitější bit (Most Significant Bit) je vždy přenášen jako první.                                                                                                                                                                                                                                           |
| **Clock Polarity (CPOL)** | **Low**                                  | V klidovém stavu (idle) je hodinový signál SCK na úrovni logické `0`.                                                                                                                                                                                                                                            |
| **Clock Phase (CPHA)**    | **1 Edge**                               | Vstupní data (MOSI u Slave, MISO u Master) jsou vzorkována na první, tedy náběžné (rising) hraně SCK. Data na výstupu se mění s týlovou (falling) hranou.                                                                                                                                                        |
| **Baud Rate Prescaler**   | Zvolit tak, aby $f_{SCK} \le$ **10 MHz** | Absolutní maximální frekvence hodin SCK pro sběrnici SX1255 je 10 MHz. Pro jádro STM32H7 běžící na vysokých frekvencích budeš muset nastavit poměrně vysoký dělící poměr na příslušné APB sběrnici.                                                                                                              |

### Skladba přenosového rámce (Frame Structure)

Během implementace SPI ovladačů do firmwaru je třeba počítat se specifickým formátem prvního (adresního) bajtu, který mikrokontrolér posílá směrem do SX1255:

* **Bit 7 (`wnr`):** Určuje směr toku dat. Logická `1` pro zápis (Write), logická `0` pro čtení (Read).


* **Bity 6 až 0 (`A[6:0]`):** Fyzická adresa registru.



Při čtecí operaci (Read) vysílá SX1255 během odesílání adresního bajtu na lince MISO vysokou impedanci (High-Z) a samotná data začne posílat až s prvním bitem druhého bajtu (taktovaným od SCK). Při zápisu (Write) kopíruje SX1255 na MISO linku předchozí obsah zapisovaného registru (stav před samotným přepsáním).

### Kritické časovací parametry pro firmware

Pokud bys pro obsluhu SPI nevyužíval standardní blokovací HAL funkce a přistupoval bys k registrům velmi těsně napřímo (např. bit-banging NSS pinu bezprostředně kolem startu DMA), dej si pozor na tyto katalgové limity:

* **$t_{nsetup}$ (NSS set-up time):** Minimálně **30 ns** od pádu NSS do první náběžné hrany SCK.
* **$t_{nhold}$ (NSS hold time):** Minimálně **50 ns** od poslední týlové hrany SCK do zvednutí NSS zpět do logické `1`.
* **$t_{nhigh}$ (NSS high time):** Minimálně **20 ns** odpočinek (High stav NSS) mezi dvěma nezávislými SPI transakcemi.

---

# Architektura USB rozhraní (Composite UAC 1.0 + CDC)

## 1. Topologie a hardwarová vrstva

* **Periferie:** `USB_OTG_HS` s integrovaným Full-Speed PHY (rychlost 12 Mbit/s).
* **Napájení:** Self-Powered (zařízení napájeno vlastním zdrojem).
* **Detekce připojení:** VBUS sensing povolen na vyhrazeném pinu (bude nutný odporový dělič z 5 V linky USB kabelu na bezpečné 3.3 V pro MCU).
* **Přenos dat:** Vyhrazené interní DMA pro `USB_OTG_HS` (zcela odbřemení CPU jádra Cortex-M7 od přesunu dat do/z USB FIFO paměti).
* **Start of Frame (SOF):** Povoleno softwarové přerušení na SOF (1 ms interval) pro kritickou synchronizaci audio bufferů.

## 2. Kompozitní konfigurace (USB Descriptors)

Zařízení se po připojení k PC enumeruje s jedním USB Configuration Descriptorem, který bude obsahovat následující rozhraní (Interfaces):

1. **Interface 0 (Audio Control - AC):** Společné ovládací rozhraní definující audio topologii (vstupy, výstupy, Feature Unit pro Volume/Mute).
2. **Interface 1 (Audio Streaming - AS OUT):** *Volitelné (Playback).* Pro poslech audia z PC do sluchátkové sady v kukle.
   * Endpoint: Isochronous OUT, Asynchronous.
   * Endpoint: Isochronous IN (Feedback). Nutný pro sdělení PC, jak rychle má data reálně posílat.

3. **Interface 2 (Audio Streaming - AS IN):** *Záznam (Record).* Pro odesílání demodulovaného audia nebo surových I/Q dat do PC.
   * Endpoint: Isochronous IN, Asynchronous.

4. **Interface 3 (CDC Control) & Interface 4 (CDC Data):** Virtuální sériový port (COM) pro CAT povely (přeladění, PTT, diagnostika).
   * Endpointy: Interrupt IN (Control), Bulk IN, Bulk OUT.

## 3. Strategie Asynchronního Audia (24 000 Hz vs 23 437.5 Hz)

Standardní operační systémy nepodporují naši hardwarově čistou frekvenci $23.4375\text{ kHz}$. Zařízení proto bude do OS deklarovat standardní frekvenci **24 000 Hz**.

**Mechanismus toku dat do PC (Záznam - IN):**

* Při 24 kHz (Full-Speed USB, 1 ms SOF) by PC očekávalo přesně 24 vzorků v každém paketu.
* Náš hardware z `TAC5142` a `SX1255` ale generuje vzorky tempem $23.4375\text{ kHz}$. To znamená $23.4375$ vzorků na 1 ms.
* Díky deklaraci asynchronního endpointu (podle specifikace UAC 1.0) nemusíme posílat přesně 24 vzorků. V obsluze SOF přerušení budeme dynamicky kontrolovat počet nasbíraných vzorků z I2S DMA.
* Většinu 1ms paketů odešleme s **23 vzorky**, a zhruba ve 43.75 % případů odešleme paket s **24 vzorky**.
* **Výsledek:** PC dostane data přesně hardwarovou rychlostí. Asynchronní ovladač v OS si data na své straně plynule pře-vzorkuje (resampluje) na uživatelem požadovaných 24 kHz (či 48 kHz), čímž získáme naprosto plynulý stream bez podtékání bufferů nebo lupanců, a to bez potřeby ASRC v hardwaru.

## 4. Architektura firmwaru v C (Software Stack)

Generátor ST USB_DEVICE v CubeMX neumí kompozitní zařízení sestavit automaticky. Přistoupíme k tomu následovně:

* Využijeme knihovnu **STM32 USB Device Library** jako základní "Core" vrstvu.
* Vytvoříme vlastní soubor třídy (např. `usbd_composite.c`), který ručně sestaví kompozitní Configuration Descriptor obsahující jak sekci pro `usbd_audio`, tak pro `usbd_cdc`.
* Obsluha endpointů v `usbd_composite.c` bude rozhazovat příchozí USB přerušení (Data IN/OUT, Setup Request) mezi příslušné handlery Audio a CDC tříd podle čísla endpointu a rozhraní.
* Pro I/Q data můžeme dynamicky využít 32bitový stereo formát (I složka vlevo, Q složka vpravo), který se vejde do šířky pásma Full-Speed USB (cca 1.5 Mbit/s je hluboko pod teoretickým limitem 12 Mbit/s).

# STM32H7 GPIO specifikace


| Název signálu   | Pin MCU       | Typ I/O        | Popis a logika použití                                                                                                                                                                                                                                                                                                                                                       |
|-----------------|---------------|----------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **RADIO_RESET** | `PC0`         | Vstup / Výstup | **Hardwarový reset pro SX1255.**<br><br>Využívá se softwarová simulace Open-Drain:<br><br>1. Přepnout pin na `Output Push-Pull` a zapsat **log. 1**.<br><br>2. Čekat **100 μs**.<br><br>3. Přepnout pin zpět na `Input Floating` (High-Z). Tím se sběrnice uvolní a interní pull-down v SX1255 stáhne signál k zemi.<br><br>4. Čekat **5 ms** před zahájením SPI komunikace. |
| **PA_BIAS_ON**  | `PC1`         | Výstup (PP)    | **Řízení předpětí a aktivace RF koncového stupně.**<br><br>• **Log. 0:** Příjem (RX) – PA je zakázán.<br><br>• **Log. 1:** Vysílání (TX) – Cesta je přepnuta a PA je aktivní.<br><br>*(Poznámka pro HW: Doporučuje se osadit na DPS externí pull-down rezistor např. 100 kΩ, aby byl PA bezpečně vypnutý během bootování MCU, kdy jsou piny ve stavu High-Z).*               |
| **RX_BAND**     | `PC4`         | Výstup (PP)    | **Přepínání přijímacích pásmových propustí (BPF).**<br><br>• **Log. 0:** Aktivní filtr pro sdílené kmitočty a PMR (442 MHz – 450 MHz).<br><br>• **Log. 1:** Aktivní filtr pro radioamatérské pásmo (70 cm).                                                                                                                                                                  |
| **FILTER_TUNE** | `PC5`         | Výstup (PP)    | **Diagnostický a ladicí režim.**<br><br>Softwarově odpojí běžnou TX/RX cestu od výstupní dolní propusti a připojí vstup na dedikovaný měřicí MMCX konektor. Slouží ke kalibraci a měření přenosu in-circuit.                                                                                                                                                                 |
| **PTT_TO_MCU**  | `PA2`         | Vstup          | **Vstup pro vnější klíčování (Push-To-Talk).**<br><br>• **Log. 0:** Vysílání (TX).<br><br>• **Log. 1:** Příjem (RX).<br><br>*(Poznámka pro SW: Pro spolehlivou funkci je nutné v konfiguraci pinu aktivovat interní Pull-Up rezistor a ve firmwaru implementovat debouncing).*                                                                                               |
| **PTT_LED**     | `PA3`         | Výstup (PP)    | **Indikační LED dioda vysílání.**<br><br>• **Log. 0:** Vypnuto.<br><br>• **Log. 1:** Svítí červeně (indikace aktivní TX relace).                                                                                                                                                                                                                                             |
| **SPI_NCS**     | *N/A*         | Výstup (PP)    | **Softwarový Chip Select (NSS) pro SPI3.**<br><br>Slouží ke komunikaci s SX1255. V klidu je výstup držen v **log. 1**. Ovládán ručně kolem blokových (BURST) SPI operací.                                                                                                                                                                                                    |
| **I2S_CKIN**    | `PC9`         | AF Vstup       | **Vstup hlavní hodinové synchronizace.**<br><br>Přivádí čistých 36 MHz z palubního TCXO přímo do I2S periferií. Bypassuje interní PLL MCU, čímž zajišťuje absolutní fázovou čistotu bez jitteru pro Zero-IF a audio.                                                                                                                                                         |
| **USB_VBUS**    | `PA9`         | Vstup          | **Detekce napětí 5 V z USB (VBUS Sensing).**<br><br>Kritické pro USB specifikaci, protože celé zařízení je "Self-Powered". MCU přes tento pin zjistí fyzické připojení USB kabelu k PC.<br><br>*(Poznámka pro HW: Musí být realizováno přes odporový dělič z 5 V na 3.3 V).*                                                                                                 |
| **USB_D- / D+** | `PA11 / PA12` | AF I/O         | **Datové linky Full-Speed USB (OTG_HS).**<br><br>Integrovaný fyzický PHY. Připojeno přes ESD ochranu přímo na konektor. Slouží pro USB Audio (UAC) a sériový port (CDC).                                                                                                                                                                                                     |

