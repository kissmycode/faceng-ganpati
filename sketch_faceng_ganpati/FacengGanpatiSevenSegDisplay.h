#ifndef _seven_seg_display_h_
#define _seven_seg_display_h_
/*****************************************************************************************/
//
// Global for 7-segment display
//
/*****************************************************************************************/
constexpr uint8_t numSegmentsForDigits = 7;
// Pin output for 7-seg display. Order of segments: a,b,c,d,e,f,g
constexpr int pinConfig[11][numSegmentsForDigits] = {{1,1,1,1,1,1,0 },    // 0
                                                    { 0,1,1,0,0,0,0 },    // 1
                                                    { 1,1,0,1,1,0,1 },    // 2
                                                    { 1,1,1,1,0,0,1 },    // 3
                                                    { 0,1,1,0,0,1,1 },    // 4
                                                    { 1,0,1,1,0,1,1 },    // 5
                                                    { 1,0,1,1,1,1,1 },    // 6
                                                    { 1,1,1,0,0,0,0 },    // 7
                                                    { 1,1,1,1,1,1,1 },    // 8
                                                    { 1,1,1,0,0,1,1 },    // 9
                                                    { 1,1,1,0,1,1,1 }};   // A

// Arduino pins for 7-seg display                             a.  b.  c.  d.  e.  f.  g
constexpr uint8_t arduinoPinsFor7segDisp[numSegmentsForDigits] = {49, 48, 47, 46, 45, 44, 43};

inline void writeNumberTo7SegDisplay(uint8_t num) {
  if (num > 10)
    return;
  for (int i = 0; i < numSegmentsForDigits; i++) {
    digitalWrite(arduinoPinsFor7segDisp[i], pinConfig[num][i]);
  }
}

#endif // _seven_seg_display_h_