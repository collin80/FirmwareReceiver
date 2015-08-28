// Required libraries
#include "variant.h"
#include <due_can.h>
#include <DueFlashStorage.h>
#include <FirmwareReceiver.h>

#define Serial SerialUSB

FirmwareReceiver *fwReceiver;

void gotFrame(CAN_FRAME *frame) 
{
	SerialUSB.println("got frame!");
	fwReceiver->gotFrame(frame);
}

void setup()
{
	delay(10000);

  Serial.begin(115200);
  
  // Initialize CAN0, Set the proper baud rates here
  Can0.init(CAN_BPS_250K);
	 
  Can0.setRXFilter(0, 0, 0, false); //catch all mailbox
    
  Can0.setGeneralCallback(gotFrame);  
	
 fwReceiver = new FirmwareReceiver(&Can0, 0x1FDA4C36, 0x100);
	
SerialUSB.println("Init successful");
}

void loop(){	
}


