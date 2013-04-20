#include "knocker.h"
#include "SerialCommand.h"

#define FLEX_PIN 0
#define SOLENOID_PIN 4
#define LED1_PIN 5
#define LED2_PIN 6
#define LED3_PIN 7
#define PIEZO_PIN 1

const int MAX_PATTERN_LENGTH = 50;
unsigned int knock_pattern[MAX_PATTERN_LENGTH];
unsigned int knock_pattern_count = 0;

unsigned int recorded_pattern[MAX_PATTERN_LENGTH];

Knocker knocker(PIEZO_PIN, SOLENOID_PIN);
SerialCommand sCmd;

void send_login(const char *name)
{
  Serial.print("LOGIN ");
  Serial.println(name);
}

void unrecognized(const char *cmd)
{
  Serial.print("unrecognized: ");
  Serial.println(cmd);
}

/*!
  KNOCK <knock-array>

  Examples:
  No initial delay: knock-knock-knock--knock
  KNOCK 0, 500, 500, 1000
  
  Initial delay: -knock-knock--knock
  KNOCK 500, 500, 1000
  
  Correct shave&haircut answer:
  KNOCK 1000, 500

 */
void knock_action()
{
  knock_pattern_count = 0;
  char *arg;
  while (arg = sCmd.next()) {
    knock_pattern[knock_pattern_count++] = atoi(arg);
  }

  Serial.print("Knock received: ");
  for (uint8_t i=0;i<knock_pattern_count;i++) {
    Serial.print(knock_pattern[i]);
    Serial.print(" ");
  }
  Serial.println();
  knocker.playPattern(knock_pattern, knock_pattern_count);
}

void cmd_action()
{
  char *arg = sCmd.next();

  Serial.print("Cmd received: ");
  Serial.println(arg);
}

void send_pattern(unsigned int *pattern, uint8_t length)
{
  Serial.print("KNOCK");
  for (uint8_t i=0;i<length;i++) {
    Serial.print(" ");
    Serial.print(pattern[i]);
  }
  Serial.println();
}

void setup ()
{ 
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  
  digitalWrite(LED1_PIN, 0);
  digitalWrite(LED2_PIN, 0);
  digitalWrite(LED3_PIN, 0);

  Serial.begin(9600);
  sCmd.addCommand("KNOCK", knock_action);
  sCmd.addCommand("CMD", cmd_action);
  sCmd.setDefaultHandler(unrecognized);

  send_login("Chambord");
}

void loop()
{
  sCmd.readSerial();

  int len = knocker.detectPattern(recorded_pattern, MAX_PATTERN_LENGTH, 2000);
  if (len > 0) { // Detected full pattern
    send_pattern(recorded_pattern, len);
  }
}
