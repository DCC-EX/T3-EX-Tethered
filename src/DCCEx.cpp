#include <DCCEx.h>
#include <Regexp.h>

DCCEx::DCCEx(HardwareSerial *serial, uint16_t timeout)
    : _serial(serial) {
  _serial->begin(115200);
  _serial->setTimeout(timeout);
}

uint16_t DCCEx::readCSResponse(char *buffer, uint16_t length) {
  return _serial->readBytesUntil('\n', buffer, length);
}

void DCCEx::clearCSResponse() {
  _serial->read();
}

void DCCEx::powerOff(Track track) {
  if (track == Track::ALL) {
    _serial->println(F("<0>"));
  } else if (track == Track::MAIN) {
    _serial->println(F("<0 MAIN>"));
  } else if (track == Track::PROG) {
    _serial->println(F("<0 PROG>"));
  }
}

void DCCEx::powerOn(Track track) {
  if (track == Track::ALL) {
    _serial->println(F("<1>"));
  } else if (track == Track::MAIN) {
    _serial->println(F("<1 MAIN>"));
  } else if (track == Track::PROG) {
    _serial->println(F("<1 PROG>"));
  }
}

void DCCEx::powerJoin() {
  _serial->println(F("<1 JOIN>"));
}

void DCCEx::emergencyStopAll() {
  _serial->println(F("<!>"));
}

bool DCCEx::setThrottle(uint16_t address, int8_t speed, uint8_t direction) {
  char buf[18];
  sprintf_P(buf, PSTR("<t 1 %d %d %d>"), address, speed, direction);
  _serial->println(buf);

  memset(buf, 0, sizeof(buf));
  MatchState ms(buf, sizeof(buf));
  char pattern[18];
  strncpy_P(pattern, PSTR("<T 1 (%d+) (%d+)>"), sizeof(pattern));

  if (readCSResponse(buf, sizeof(buf) - 1) && ms.Match(pattern) == REGEXP_MATCHED &&
      (int8_t)strtol(ms.capture[0].init, (char **)NULL, 10) == speed &&
      (uint8_t)strtoul(ms.capture[1].init, (char **)NULL, 10) == direction) {
    return true;
  }
  return false;
}

void DCCEx::setFn(uint16_t address, uint16_t fn, bool state) {
  char buf[16];
  sprintf_P(buf, PSTR("<F %d %d %d>"), address, fn, state);
  _serial->println(buf);
}

void DCCEx::release(uint16_t address) {
  char buf[10];
  sprintf_P(buf, PSTR("<- %d>"), address);
  _serial->println(buf);
}

bool DCCEx::writeAddress(uint16_t address) {
  char buf[10];
  sprintf_P(buf, PSTR("<W %d>"), address);
  _serial->println(buf);

  memset(buf, 0, sizeof(buf));
  MatchState ms(buf, sizeof(buf));
  char pattern[10];
  strncpy_P(pattern, PSTR("<w (%d+)>"), sizeof(pattern));

  if (readCSResponse(buf, sizeof(buf) - 1) && ms.Match(pattern) == REGEXP_MATCHED &&
      (uint16_t)strtoul(ms.capture[0].init, (char **)NULL, 10) == address) {
    return true;
  }
  return false;
}

int16_t DCCEx::readAddress() {
  char buf[10] = { 0 };
  _serial->println(F("<R>"));

  MatchState ms(buf, sizeof(buf));
  char pattern[10];
  strncpy_P(pattern, PSTR("<r (%d+)>"), sizeof(pattern));

  if (readCSResponse(buf, sizeof(buf) - 1) && ms.Match(pattern) == REGEXP_MATCHED) {
    return (int16_t)strtol(ms.capture[0].init, (char **)NULL, 10);
  }
  
  return -1;
}

bool DCCEx::writeCVByte(uint16_t cv, uint8_t value) {
  char buf[24];
  sprintf_P(buf, PSTR("<W %d %d 12345 32767>"), cv, value);
  _serial->println(buf);

  memset(buf, 0, sizeof(buf));
  MatchState ms(buf, sizeof(buf));
  char pattern[27];
  strncpy_P(pattern, PSTR("<r12345|32767|(%d+) (%d+)>"), sizeof(pattern));

  if (readCSResponse(buf, sizeof(buf) - 1) && ms.Match(pattern) == REGEXP_MATCHED &&
      (uint16_t)strtoul(ms.capture[0].init, (char **)NULL, 10) == cv &&
      (uint8_t)strtoul(ms.capture[1].init, (char **)NULL, 10) == value) {
    return true;
  }
  return false;
}

int16_t DCCEx::readCVByte(uint16_t cv) {
  char buf[24];
  sprintf_P(buf, PSTR("<R %d 12345 32767>"), cv);
  _serial->println(buf);

  memset(buf, 0, sizeof(buf));
  MatchState ms(buf, sizeof(buf));
  char pattern[27];
  strncpy_P(pattern, PSTR("<r12345|32767|(%d+) (%d+)>"), sizeof(pattern));

  if (readCSResponse(buf, sizeof(buf) - 1) && ms.Match(pattern) == REGEXP_MATCHED &&
      (uint16_t)strtoul(ms.capture[0].init, (char **)NULL, 10) == cv) {
    return (int16_t)strtol(ms.capture[1].init, (char **)NULL, 10);
  }
  return -1;
}

bool DCCEx::writeCVBit(uint16_t cv, uint8_t bit, bool value) {
  char buf[25];
  sprintf_P(buf, PSTR("<B %d %d %d 12345 32767>"), cv, bit, value);
  _serial->println(buf);

  memset(buf, 0, sizeof(buf));
  MatchState ms(buf, sizeof(buf));
  char pattern[33];
  strncpy_P(pattern, PSTR("<r12345|32767|(%d+) (%d+) (%d+)>"), sizeof(pattern));

  if (readCSResponse(buf, sizeof(buf) - 1) && ms.Match(pattern) == REGEXP_MATCHED &&
      (uint16_t)strtoul(ms.capture[0].init, (char **)NULL, 10) == cv &&
      (uint8_t)strtoul(ms.capture[1].init, (char **)NULL, 10) == bit &&
      (uint8_t)strtoul(ms.capture[2].init, (char **)NULL, 10) == value) {
    return true;
  }
  return false;
}
