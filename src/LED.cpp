#include "LED.h"

namespace ESPanel
{
inline namespace LED
{
void blink(int rate)
{
    digitalWrite(2, LOW); //LED ON
    delay(rate);
    digitalWrite(2, HIGH); //LED OFF
    delay(rate);
}
} // namespace LED
} // namespace ESPanel