#include "t1s_lwip_hal.h"

void T1S_EIC_CB_Register(void (* callback)(uintptr_t context) , uintptr_t context)
{
    EIC_CallbackRegister(EIC_PIN_7, callback, context);
}

