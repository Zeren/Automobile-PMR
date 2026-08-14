# How to get HW specification

Proveď destilaci celého našeho návrhu hardwaru a vygeneruj strojově čitelný "Hardware Specification Manifest" ve formátu YAML. Udělej ho jako raw, nebo jako přílohu (attachment).
Tento dokument bude sloužit jako jediný vstup pro vývoj embedded firmwaru. Zcela ignoruj proces návrhu, BOM, pasivní součástky (pokud neovlivňují logiku) a fyzický layout. Soustřeď se výhradně na to, co musí znát programátor (C, STM HAL). Ještě můžeš doporučit nastavení DMA kanálů (především I2S, SPI, UART). Potom je ještě zvlášť napiš jaké mám kde nastavit v SMT32cubeMX.
Při generování dodržuj tuto strukturu:
1. mcu_config: Přesný typ MCU, frekvence jádra, zdroj hodin (HSE/HSI).
2. pinmap: Seznam všech využitých pinů. Formát: [Pin, Název signálu, Směr/Typ, Pull-up/down, Alternate Function (pokud jde např. o STM32 AF), Aktivní úroveň (High/Low)].
3. peripherals: Konfigurace periferií (např. I2C adresy slave zařízení na sběrnici, rychlost SPI, baudrate UARTu, přiřazení DMA kanálů).
4. power_and_reset: Sekvence zapínání periferií (pokud existují povolovací EN piny), časy potřebné pro ustálení napětí, konfigurace externího watchdogu.
5. interrupts: Požadovaná hardwarová přerušení a jejich priority.
6. quirks: Hardwarová specifika (např. "LED svítí při log. 0", "Senzor XY vyžaduje po startu 10 ms delay", "CS pin pro SPI flash má externí pull-up").
Vrať POUZE čistý kód ve formátu YAML. Nepřidávej žádný úvodní ani závěrečný text, žádné vysvětlování. Výstup bude anglicky.