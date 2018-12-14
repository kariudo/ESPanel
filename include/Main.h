#ifndef MAIN_H_
#define MAIN_H_

namespace ESPanel
{
inline namespace Setup
{
#ifdef DEBUG_OUTPUT
void startSerial();
#endif // DEBUG_OUTPUT
void setPins();
void readAllPins();
} // namespace Setup
} // namespace ESPanel

#endif // !MAIN_H_