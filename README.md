# RFID Copier

<img src="https://img.shields.io/badge/build-passing-brightgreen"><img src ="https://img.shields.io/badge/Based%20on-Arduino-3186a0">

Modifies/Copies ids of RFID cards.

## Requirements
1. Arduino board(Uno testing pass)
2. MFRC522

## Wiring
```
Typical pin layout used:
-----------------------------------------------------------------------------------------
           MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
           Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
Signal      Pin          Pin           Pin       Pin        Pin              Pin
-----------------------------------------------------------------------------------------
RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
SPI SS      SDA(SS)      10            53        D10        10               10
SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
```

## Usage
1. Scan through the original card. The beeper will alarm if success. 
2. Within 2 seconds, scan the modifiable RFID card.
3. Note that the targeting card needs to be modifiable.

## Datasheet 
MFRC522: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
