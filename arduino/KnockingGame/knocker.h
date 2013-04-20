#ifndef KNOCKER_H_
#define KNOCKER_H_

#include <stdio.h>

class Knocker
{
public:
  Knocker(uint8_t input_pin, uint8_t output_pin)
    : input_pin(input_pin), output_pin(output_pin), 
      recording_length(0), last_knock_time(0), is_recording(false) {}

  void playPattern(unsigned int *pattern, uint8_t length);
  uint8_t detectPattern(unsigned int *pattern, uint8_t maxlength, unsigned int timeout = 2000);

private:
  const uint8_t input_pin;
  const uint8_t output_pin;
  uint8_t recording_length;
  unsigned long last_knock_time;
  bool is_recording;
};

#endif
