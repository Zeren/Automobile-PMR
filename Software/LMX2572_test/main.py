import time

import spidev

registers = [0x7D2288,
             0x7C0000,
             0x7B0000,
             0x7A0000,
             0x790000,
             0x780000,
             0x770000,
             0x760000,
             0x750000,
             0x740000,
             0x730000,
             0x727802,
             0x710000,
             0x700000,
             0x6F0000,
             0x6E0000,
             0x6D0000,
             0x6C0000,
             0x6B0000,
             0x6A0007,
             0x694440,
             0x682710,
             0x670000,
             0x660000,
             0x650000,
             0x642710,
             0x630000,
             0x620000,
             0x610000,
             0x600000,
             0x5F0000,
             0x5E0000,
             0x5D0000,
             0x5C0000,
             0x5B0000,
             0x5A0000,
             0x590000,
             0x580000,
             0x570000,
             0x560000,
             0x55D800,
             0x540001,
             0x530000,
             0x522800,
             0x510000,
             0x50CCCC,
             0x4F004C,
             0x4E0071,
             0x4D0000,
             0x4C000C,
             0x4B08C0,
             0x4A0000,
             0x49003F,
             0x480001,
             0x470081,
             0x46C350,
             0x450000,
             0x4403E8,
             0x430000,
             0x4201F4,
             0x410000,
             0x401388,
             0x3F0000,
             0x3E00AF,
             0x3D00A8,
             0x3C03E8,
             0x3B0001,
             0x3A9001,
             0x390020,
             0x380000,
             0x370000,
             0x360000,
             0x350000,
             0x340421,
             0x330080,
             0x320080,
             0x314180,
             0x3003E0,
             0x2F0300,
             0x2E07F0,
             0x2DC61F,
             0x2C1D23,
             0x2B0001,
             0x2A3520,
             0x290000,
             0x280000,
             0x270001,
             0x264E20,
             0x250205,
             0x240023,
             0x230004,
             0x220010,
             0x211E01,
             0x2005BF,
             0x1FC3E6,
             0x1E18A6,
             0x1D0000,
             0x1C0488,
             0x1B0002,
             0x1A0808,
             0x190624,
             0x18071A,
             0x17007C,
             0x160001,
             0x150409,
             0x144848,
             0x1327B7,
             0x120064,
             0x110089,
             0x100080,
             0x0F060E,
             0x0E1820,
             0x0D4000,
             0x0C5001,
             0x0BB018,
             0x0A1278,
             0x090004,
             0x082000,
             0x0700B2,
             0x06C802,
             0x0528C8,
             0x040A43,
             0x030782,
             0x020500,
             0x010808,
             0x00211C]


def reset(spi) -> None:
    setup_data = [0x00, 0b00100011, 0b00011110]
    spi.writebytes(setup_data)
    time.sleep(0.01)


def testovaci_stusteni(spi) -> None:
    # VCO kalibrace
    data = [78, 0x02, 0b00110011]
    spi.writebytes(data)

    # Vystupni divider CHDIV = 5 => outDIB = 16 => Fout = 3500/16=218MHz
    data = [75, 0x01, 0b01000000]
    spi.writebytes(data)

    # Musi se to po resetu preprogramovat
    data = [71, 0x00, 0x81]
    spi.writebytes(data)

    # Musi se to po resetu preprogramovat
    data = [57, 0x00, 0x20]
    spi.writebytes(data)

    # Musi se to po resetu preprogramovat
    data = [52, 0x04, 0x21]
    spi.writebytes(data)

    # PLL_N nastavit na VCO 3.5GHz => N =  35
    data = [36, 0x00, 35]
    spi.writebytes(data)
    data = [34, 0x00, 0x10]
    spi.writebytes(data)

    # Musi se to po resetu preprogramovat
    data = [30, 0x0c, 0xa6]
    spi.writebytes(data)

    # Musi se to po resetu preprogramovat
    data = [29, 0, 0]
    spi.writebytes(data)

    # PLL_R = 1
    data = [11, 0b10110000, 0b00001000]
    spi.writebytes(data)

    # MULT = 4
    data = [10, 16 + 4, 0x78]
    spi.writebytes(data)

    # diff vystup, zakonceny 100 ohm
    data = [5, 0b00101000, 0xc8]
    spi.writebytes(data)

    # Reg 0
    data = [0, 0b00100001, 0b00011100]
    spi.writebytes(data)


def main() -> None:
    spi = spidev.SpiDev()
    spi.open(0, 0)
    spi.max_speed_hz = 10000000
    spi.mode = 0  # TOHLE JE DOLEZITE
    reset(spi)

    for zapis in registers:
        data = [zapis >> 16 & 255, zapis >> 8 & 255, zapis & 255]
        spi.writebytes(data)
        print(data)


if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
