#ifndef CONFIG_H
#define CONFIG_H

/** IR Signal **/
#define MAX_IR_SIGNAL_LEN             1000        // maximum of IR signal length
#define MAX_IR_SIGNAL_COUNT           3           // total of IR signal saved

#define ADD_IR_SIGNAL_TIMEOUT         5000

#define IR_LED_PIN                    4           // GPIO 4, where IR LED attached
#define IR_RECEIVER_PIN               15          // GPIO 15,  where IR Recevier attached
#define IR_RECV_BUFFER_SIZE           1024        // multiply by 2-byte (size of uint16_t)
#define IR_RECV_TIMEOUT               15          // in ms
#define IR_RECV_TOLERANCE_PERCENTAGE  25          // tolerance percentage in %

#define CARRIER_SIGNAL_FREQ           38          // in kHz

/** Menu Button **/
#define MENU_SELECT_PIN               18          // GPIO 18
#define MENU_NEXT_PIN                 19          // GPIO 19
#define MENU_PREV_PIN                 5           // GPIO 5

/** Display **/
#define DISPLAY_FONT                  u8g2_font_9x15_tr // font of LED display, refer to https://github.com/olikraus/u8g2/wiki/fntlistall

/** EEPROM **/
#define EEPROM_SIZE                   4096        // in byte
#define ADDR_RFID_TAG_UID             3000

/** MFRC522 Module **/
#define RST_PIN                       26  
#define SS_PIN                        27   
#define MOSI_PIN                      13
#define MISO_PIN                      12
#define SCK_PIN                       14 

#define READ_TAG_UID_TIMEOUT          5000
#define WRITE_TAG_UID_TIMEOUT         5000

#endif