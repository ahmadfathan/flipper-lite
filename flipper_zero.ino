// Native Library
#include <Arduino.h>
#include "EEPROM.h"
#include <Wire.h>

#include <WiFi.h>

// https://github.com/olikraus/u8g2
#include <U8g2lib.h>

// https://github.com/crankyoldgit/IRremoteESP8266
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

#include <SPI.h>
#include <MFRC522.h>

#include "config.h"

IRrecv irrecv(IR_RECEIVER_PIN, IR_RECV_BUFFER_SIZE, IR_RECV_TIMEOUT, true);
IRsend irsend(IR_LED_PIN);  // Set the GPIO to be used to sending the message.

decode_results results;  // Somewhere to store the results

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  // All Boards without Reset of the Display

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance



struct Route {
  uint8_t pos;
  char* title;
  char* list;
  Route* children;
  uint8_t num_of_children;
};

Route signal_route[] = {
  Route{ 1, "Signal 1" },
  Route{ 2, "Signal 2" },
  Route{ 3, "Signal 3" },
  Route{ 4, "Back" },
};

Route ir_signal_route[] = {
  Route{ 1, "Add IR Signal", "Signal 1\n"
                             "Signal 2\n"
                             "Signal 3\n"
                             "Back",
         &signal_route[0], 4 },
  Route{ 2, "Emit IR Signal", "Signal 1\n"
                              "Signal 2\n"
                              "Signal 3\n"
                              "Back",
         &signal_route[0], 4 },
  Route{ 3, "Back" }
};

Route rfid_route[] = {
  Route{ 1, "Read Tag" },
  Route{ 2, "Write Tag" },
  Route{ 3, "Back" },
};

Route main_menu_route[] = {
  Route{ 1, "IR Signal", "Add IR Signal\n"
                         "Emit IR Signal\n"
                         "Back",
         &ir_signal_route[0], 3 },
  Route{ 2, "RFID", "Read Tag\n"
                    "Write Tag\n"
                    "Back",
         &rfid_route[0], 3 },
  Route{ 3, "Back" }
};

Route root = Route{ 1, "Main Menu", "IR Signal\n"
                                    "RFID",
                    &main_menu_route[0], 2 };

Route current_route = root;
Route back_stack[10];
uint8_t route_pos = 0;
uint8_t last_selection = 0;

void setup(void) {

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();  // Init MFRC522 card

  irrecv.setTolerance(IR_RECV_TOLERANCE_PERCENTAGE);  // Override the default tolerance.
  irsend.begin();

  EEPROM.begin(EEPROM_SIZE);

  u8g2.setBusClock(5000000);

  u8g2.begin(MENU_SELECT_PIN, MENU_NEXT_PIN, MENU_PREV_PIN, /* menu_up_pin= */ U8X8_PIN_NONE, /* menu_down_pin= */ U8X8_PIN_NONE, /* menu_home_pin= */ U8X8_PIN_NONE);

  u8g2.setFont(DISPLAY_FONT);

  back_stack[route_pos] = current_route;
}

void loop(void) {

  uint8_t current_selection = u8g2.userInterfaceSelectionList(
    current_route.title,
    (last_selection == 0) ? current_route.children[0].pos : last_selection,
    current_route.list);

  Route* next_route = find_next_route_by_pos(current_selection);

  if (next_route == NULL) return;

  last_selection = current_selection;

  if (next_route->title == "Back") {
    route_pos--;
    current_route = back_stack[route_pos];
    last_selection = 0;
    return;
  } else if (next_route->title == "Read Tag") {
    read_tag_uid();
    return;
  } else if (next_route->title == "Write Tag") {
    write_tag_uid();
    return;
  } else if (next_route->title == "Signal 1") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(1);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Waiting..");
      u8g2.sendBuffer();

      add_ir_signal(1);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "Signal 2") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(2);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Waiting..");
      u8g2.sendBuffer();

      add_ir_signal(2);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done");
      u8g2.sendBuffer();
    }
    return;
  } else if (next_route->title == "Signal 3") {
    if (current_route.title == "Emit IR Signal") {
      emit_ir_signal(3);
    } else if (current_route.title == "Add IR Signal") {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Recording..");
      u8g2.sendBuffer();

      add_ir_signal(3);

      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, "Done");
      u8g2.sendBuffer();
    }
    return;
  }

  route_pos++;
  current_route = *next_route;
  back_stack[route_pos] = current_route;
  last_selection = 0;
}

Route* find_next_route_by_pos(uint8_t pos) {
  for (uint8_t i = 0; i < current_route.num_of_children; i++) {
    if (pos == current_route.children[i].pos) {
      return &current_route.children[i];
    }
  }
  return NULL;
}

void add_ir_signal(uint8_t signal_num) {

  if (signal_num > MAX_IR_SIGNAL_COUNT) return;  // abort if signal num is exceeded

  uint16_t addr_signal_len = MAX_IR_SIGNAL_LEN * (signal_num - 1);
  uint16_t addr_signal = addr_signal_len + 2;

  irrecv.enableIRIn();  // enable IR receiver

  uint64_t now = millis();
  while (true) {
    if (irrecv.decode(&results)) {

      uint16_t rawdata[1024];
      uint16_t j = 0;
      for (uint16_t i = 1; i < results.rawlen; i++, j++) {
        uint32_t usecs = results.rawbuf[i] * kRawTick;
        rawdata[j] = usecs;
      }

      EEPROM.writeUShort(addr_signal_len, j);

      for (uint16_t i = 0; i < j; i++) {
        EEPROM.writeUShort(addr_signal + (i * 2), rawdata[i]);
      }

      EEPROM.commit();

      break;
    }
    if (millis() - now > ADD_IR_SIGNAL_TIMEOUT) break;
  }

  irrecv.disableIRIn();  // disable IR receiver
}

void emit_ir_signal(uint8_t signal_num) {

  if (signal_num > MAX_IR_SIGNAL_COUNT) return;

  uint16_t addr_signal_len = MAX_IR_SIGNAL_LEN * (signal_num - 1);
  uint16_t signal_len = EEPROM.readUShort(addr_signal_len);

  if (signal_len == 0) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Signal Not Set");
    u8g2.sendBuffer();
    return;
  }

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Emiting..");
  u8g2.sendBuffer();

  uint16_t raw_data[signal_len];

  uint16_t addr_signal = addr_signal_len + 2;

  for (uint16_t i = 0; i < signal_len; i++) {
    raw_data[i] = EEPROM.readUShort(addr_signal + (i * 2));
  }

  irsend.sendRaw(raw_data, signal_len, CARRIER_SIGNAL_FREQ);
}

void write_tag_uid() {
  uint16_t addr_rfid_tag_uid_len = ADDR_RFID_TAG_UID;
  uint8_t uid_len = EEPROM.readByte(addr_rfid_tag_uid_len);

  uint16_t addr_rfid_tag_uid = addr_rfid_tag_uid_len + 1;

  String uid_str = "";

  uint8_t uid[uid_len];

  for (uint8_t i = 0; i < uid_len; i++, addr_rfid_tag_uid++) {
    uid[i] = EEPROM.readByte(addr_rfid_tag_uid);

    uid_str += String(uid[i], 16) + " ";
  }

  uid_str.toUpperCase();

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Writing TAG..");
  u8g2.drawStr(0, 50, &uid_str[0]);
  u8g2.sendBuffer();

  uint64_t now = millis();
  bool success = false;
  char* err_title = "Timeout";
  char* err_msg = "";
  while (true) {
    if (millis() - now > WRITE_TAG_UID_TIMEOUT) break;
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      continue;
    }

    // Write tag UID
    if (!mfrc522.MIFARE_SetUid(uid, uid_len, true)) {
      err_title = "Failed";
      err_msg = "Not supported";
      success = false;
      break;
    }

    success = true;
    break;
  }

  uint8_t selection = u8g2.userInterfaceMessage(
    success ? "Success" : err_title,
    "",
    success ? "" : err_msg,
    success ? " ok " : " ok \n retry ");

  if (selection == 2) {  // retry
    write_tag_uid();
  }
}

void read_tag_uid() {

  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "Reading TAG..");
  u8g2.sendBuffer();

  String uid_str = "";
  uint64_t now = millis();
  bool found = false;
  while (true) {
    if (millis() - now > READ_TAG_UID_TIMEOUT) break;
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(50);
      continue;
    }

    // RFID Tag found
    uint16_t addr_rfid_tag_uid_len = ADDR_RFID_TAG_UID;
    EEPROM.writeByte(addr_rfid_tag_uid_len, mfrc522.uid.size);

    uint16_t addr_rfid_tag_uid = addr_rfid_tag_uid_len + 1;

    // Dump UID
    for (uint8_t i = 0; i < mfrc522.uid.size; i++, addr_rfid_tag_uid++) {

      EEPROM.writeByte(addr_rfid_tag_uid, mfrc522.uid.uidByte[i]);

      uid_str += String(mfrc522.uid.uidByte[i], 16) + " ";
    }

    uid_str.toUpperCase();

    EEPROM.commit();

    found = true;
    break;
  }

  uint8_t selection = u8g2.userInterfaceMessage(
    found ? "Saved" : "Timeout",
    found ? &uid_str[0] : "",
    "",
    found ? " ok " : " ok \n retry ");

  if (selection == 2) {  // retry
    read_tag_uid();
  }
}
