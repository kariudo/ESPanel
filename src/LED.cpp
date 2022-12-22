#include "LED.h"

namespace ESPanel
{
inline namespace LED
{
void blink(int rate)
{
    digitalWrite(D4, LOW); //LED ON
    delay(rate);
    digitalWrite(D4, HIGH); //LED OFF
    delay(rate);
}
} // namespace LED
} // namespace ESPanel