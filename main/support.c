#include <time.h>
#include "esp_system.h"

#include "include/support.h"

uint32_t millis()
{
    return (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC);
}