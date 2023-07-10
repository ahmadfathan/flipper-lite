
### FLIPPER ZERO

This project tries to duplicate Flipper with minimum functionality, IR cloner and RFID UID reader/writer

#### MENU ROUTE

Main Menu - IR Signal - Add Signal - Signal 1
                                      Signal 2
                                      Signal 3
                      - Emit Signal - Signal 1
                                    - Signal 2
                                    - Signal 3
          - RFID - Read Tag
                  - Write Tag

#### EEPROM Address Mapping

##### 1. IR Signal

0     - 999   -> IR Signal 1
1000  - 1999  -> IR Signal 2
2000  - 2999  -> IR Signal 3

##### 2. RFID

3000  - 30009 -> UID 1 (currently only 1 RFID Tag) 

#### Dependency

1. Display, https://github.com/olikraus/u8g2
2. IR send/receive, https://github.com/crankyoldgit/IRremoteESP8266
3. MFRC522, https://github.com/miguelbalboa/rfid