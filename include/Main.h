#ifndef MAIN_H_
#define MAIN_H_

namespace ESPanel
{
inline namespace Setup
{
#ifdef DEBUG
void startSerial();
#endif // DEBUG
void setPins();
void blink(int rate);
void readAllPins();
} // namespace Setup
} // namespace ESPanel

#endif // !MAIN_H_