#ifndef MAIN_H_
#define MAIN_H_

namespace ESPanel
{
#ifdef DEBUG
void startSerial();
#endif // DEBUG
void setPins();
void blink(int rate);
} // namespace ESPanel

#endif // !MAIN_H_