#include "sound.h"
#include "radiomed.h"
//#include <keypadc.h>

void main(void) {
	os_ClrHome();
	os_PutStrFull("Initializing");
    if (!initSerialDefault()) {
        os_PutStrFull(" Failure");
        return;
    }
    os_PutStrFull(" Switching");
    if (!switchToPCM()) {
        os_PutStrFull(" Failure");
        return;
    }
    delay(1000);
	os_PutStrFull(" Writing");
    sendPCMAudio(radiomed_raw, radiomed_raw_len);
    return;
    //usb_Cleanup();
}
