#include "DataFormatsFOCAL/DataStructs.h"

namespace o2::focal
{

bool isTriggerLine(const GBTLine& line) {
  return line.words[0] == 0xBBBBBBBB;
}

bool isIdleLine(const GBTLine& line) {
  constexpr uint32_t patterns[] = {
    0xACCCCCCC,
    0x9CCCCCCC
  };

  auto isIdleWord = [&](uint32_t word) {
    for (const uint32_t& p : patterns) {
      if (word == p) {
        return true;
      }
    }

    return false;
  };

  return isIdleWord(line.words[2]) &&
         isIdleWord(line.words[3]) &&
         isIdleWord(line.words[4]) &&
         isIdleWord(line.words[5]);
}

bool isDAQHLine(const GBTLine& line) { 
  constexpr uint32_t patterns[] = {
    0xF0000005,
    0xF0000002
  };

  auto isDAQHWord = [&](uint32_t word) {
    for (const uint32_t& p : patterns) {
      if ((word & p) == p) {
        return true;
      }
    }

    return false;
  };

  return isDAQHWord(line.words[2]) &&
         isDAQHWord(line.words[3]) &&
         isDAQHWord(line.words[4]) &&
         isDAQHWord(line.words[5]);
}

bool isPaddingLine(const GBTLine& line) {
  return (line.words[0] | line.words[1] |
          line.words[2] | line.words[3] |
          line.words[4] | line.words[5] |
          line.words[6] | line.words[7] ) == 0;
}

GBTLineType classify(const GBTLine& line) {
  if (isTriggerLine(line)) {
    return GBTLineType::Trigger;
  }

  if (isIdleLine(line)) {
    return GBTLineType::Idle;
  }

  if (isDAQHLine(line)) {
    return GBTLineType::DAQHeader;
  }

  if (isPaddingLine(line)) {
    return GBTLineType::Padding;
  }

 return GBTLineType::Generic; 
}

} // namespace o2::focal
