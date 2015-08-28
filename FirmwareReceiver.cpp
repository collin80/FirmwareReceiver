#include "FirmwareReceiver.h"

CAN_FRAME out;

FirmwareReceiver::FirmwareReceiver(CANRaw *bus, uint32_t token, uint32_t base)
{
	canbus = bus;
	deviceToken = token;
	baseAddress = base;
	updatingFirmware = false;
	flashWritePosition = 0;

	//if we booted from FLASH1 then write to FLASH0 otherwise write to FLASH1
	if (dueFlashStorage.getGPNVMBootMode()) flashSection = 0;
	else flashSection = 1;

	//bus->setRXFilter(base, 0x7C0, false);
}

void FirmwareReceiver::gotFrame(CAN_FRAME *frame)
{	
	int location, bufferWritePtr;					

	if (frame->id == baseAddress)
	{
		if ((frame->data.byte[0] == 0xDE) && (frame->data.byte[1] == 0xAD))
		{
			if ((frame->data.byte[2] == 0xBE) && (frame->data.byte[3] == 0xEF))
			{
				if ((frame->data.byte[4] == (deviceToken & 0xFF)) && (frame->data.byte[5] == ((deviceToken >> 8) & 0xFF)))
				{
					if ((frame->data.byte[6] == ((deviceToken >> 16) & 0xFF)) && (frame->data.byte[7] == ((deviceToken >> 24) & 0xFF)))
					{
						SerialUSB.println("Starting firmware upload process");
						flashWritePosition = 0;
						updatingFirmware = true;		
						out.id = baseAddress + 0x10;
						out.extended = false;
						out.length = 8;
						out.data.byte[0] = 0xDE;
						out.data.byte[1] = 0xAF;
						out.data.byte[2] = 0xDE;
						out.data.byte[3] = 0xED;
						out.data.byte[4] = (deviceToken & 0xFF);
						out.data.byte[5] = ((deviceToken >> 8) & 0xFF);
						out.data.byte[6] = ((deviceToken >> 16) & 0xFF);
						out.data.byte[7] = ((deviceToken >> 24) & 0xFF);
						canbus->sendFrame(out);
					}
				}
			}
		}
	}

	if (frame->id == (baseAddress + 0x16))
	{
		//if (!updatingFirmware) return;
		location = frame->data.byte[0] + (256 * frame->data.byte[1]);
		bufferWritePtr = (location * 4) % IFLASH1_PAGE_SIZE;
		pageBuffer[bufferWritePtr++] = frame->data.byte[2];
		pageBuffer[bufferWritePtr++] = frame->data.byte[3];
		pageBuffer[bufferWritePtr++] = frame->data.byte[4];
		pageBuffer[bufferWritePtr++] = frame->data.byte[5];
		if (bufferWritePtr == (IFLASH1_PAGE_SIZE))
		{
			flashSection = 1;
			SerialUSB.print("Writing flash at section ");
			SerialUSB.print(flashSection);
			SerialUSB.print("     address ");
			SerialUSB.println(flashWritePosition);
			dueFlashStorage.write(flashWritePosition, pageBuffer, IFLASH1_PAGE_SIZE, flashSection);
			flashWritePosition += IFLASH1_PAGE_SIZE;
		}		
		out.id = baseAddress + 0x20;
		out.extended = false;
		out.length = 2;
		out.data.byte[0] = frame->data.byte[0];
		out.data.byte[1] = frame->data.byte[1];
		canbus->sendFrame(out);
	}

	if (frame->id == (baseAddress + 0x30))
	{
		if ((frame->data.byte[0] == 0xC0) && (frame->data.byte[1] == 0xDE))
		{
			if ((frame->data.byte[2] == 0xFA) && (frame->data.byte[3] == 0xDE))
			{
				//write out the any last little bits that were received but didn't add up to a full page
				SerialUSB.print("Writing flash at section ");
				SerialUSB.print(flashSection);
				SerialUSB.print("     address ");
				SerialUSB.println(flashWritePosition);
				dueFlashStorage.write(flashWritePosition, pageBuffer, bufferWritePtr, flashSection);

				//switch boot section
				if (flashSection = 0) dueFlashStorage.setGPNVMBootMode(false);
				else dueFlashStorage.setGPNVMBootMode(true);
				SerialUSB.println("Done sending firmware. You can reboot now.");
			}
		}
	}
}