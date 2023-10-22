# PMR radio for R124

# Connection to R124

## Connection in 124 A-1
Connection as R112. 

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