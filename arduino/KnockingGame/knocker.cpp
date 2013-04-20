#include "knocker.h"
#include <Arduino.h>

const int piezo_threshold = 570;
const int piezo_min_period = 100; // in ms

void Knocker::playPattern(unsigned int *pattern, uint8_t length)
{
  digitalWrite(this->output_pin, 1);
  delay(40);
  digitalWrite(this->output_pin, 0);
  for (uint8_t i=0;i<length;i++) {
    delay(pattern[i] > 40 ? pattern[i]-40 : 0);
    digitalWrite(this->output_pin, 1);
    delay(40);
    digitalWrite(this->output_pin, 0);
  }
}

/*!
  Will record a knock pattern and store it in the given array.

  If no knocks have been received within the given timeout, the
  pattern is considered complete and we return the pattern length.
  
  Until a pattern is completed, we will return 0.
*/
uint8_t Knocker::detectPattern(unsigned int *pattern, uint8_t maxlength, unsigned int timeout)
{
  unsigned long currtime = millis();
  if (this->is_recording && currtime - this->last_knock_time > timeout) {
    // Timeout: Reset knock pattern or detect end of pattern
    // Serial.println("Timeout");
    // for (int i=0;i<this->recording_length;i++) {
    //   Serial.print(pattern[i]);
    //   Serial.print(" ");
    // }
    // Serial.println();
    this->is_recording = false;
    uint8_t len = this->recording_length;
    this->recording_length = 0;
    return len;
  }

  if (currtime > this->last_knock_time + piezo_min_period) {
    int piezoval = analogRead(this->input_pin);
    //    Serial.println(piezoval);
    if (piezoval > piezo_threshold) {
      if (this->is_recording && this->recording_length < maxlength) {
        unsigned int delta = currtime - this->last_knock_time;
        pattern[this->recording_length++] = delta;
      }
      this->is_recording = true;
      this->last_knock_time = currtime;
    }
  }
  return 0;
}
