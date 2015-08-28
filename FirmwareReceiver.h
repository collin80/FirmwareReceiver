/*
FirmwareReceiver - The receive side for a canbus connected firmware updater. This side will write new firmware into
flash memory on this chip. Upon success it will switch which flash bank is used to boot and so the sketch will
then be running the new version.

Expects that you've already started the canbus and set it up.
*/

#include <due_can.h>
#include <DueFlashStorage.h>

class FirmwareReceiver
{
public:
	FirmwareReceiver(CANRaw *bus, uint32_t token, uint32_t base);
	void gotFrame(CAN_FRAME *frame);	

private:
	CANRaw *canbus;
	DueFlashStorage dueFlashStorage;
	uint32_t deviceToken; //set a unique value here for each device type
	uint32_t baseAddress;
	bool updatingFirmware;
	int flashSection;
	uint8_t pageBuffer[IFLASH1_PAGE_SIZE];
	uint32_t flashWritePosition;
};