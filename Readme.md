
### FLIPPER LITE

This project tries to duplicate [Flipper Zero](https://flipperzero.one/) with minimum functionality, IR cloner and RFID UID reader/writer

#### MENU ROUTE
|  |  |  |  |
|-------------|-------------|--------------|------------|
| > Main Menu | > IR Signal | > Add Signal | > Signal 1 |
|             |             |              | > Signal 2 |
|             |             |              | > Signal 3 |
|             |             | > IR Signal  | > Signal 1 |
|             |             |              | > Signal 2 |
|             |             |              | > Signal 3 |
|             | > RFID      | > Read Tag   |            |
|             |             | > Write Tag  |            |

#### EEPROM Address Mapping

##### 1. IR Signal
|Address From|Address To|Remark|
|-|-|-|
|0|999|IR Signal 1|
|1000|1999|IR Signal 2|
|2000|2999|IR Signal 3|

##### 2. RFID

|Address From|Address To|Remark|
|-|-|-|
|3000|30009|UID 1 (currently only 1 RFID Tag)|

#### Dependency

1. Display, https://github.com/olikraus/u8g2
2. IR send/receive, https://github.com/crankyoldgit/IRremoteESP8266
3. MFRC522, https://github.com/miguelbalboa/rfid

#### Showcase

<div style="text-align:center">
  <img src="https://github.com/ahmadfathan/flipper-zero/blob/9742cb3e889354f63acfe1f884749bedc68e0789/images/image1.jpeg" width="400">
  <img src="https://github.com/ahmadfathan/flipper-zero/blob/9742cb3e889354f63acfe1f884749bedc68e0789/images/image2.jpeg" width="400">
</div>
