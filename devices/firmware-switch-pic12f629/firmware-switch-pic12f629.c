

extern uint8_t  cmd;
extern wirebus_packet  packet;

void setup()
{


}

void loop()
{
	if( cmd == WIREBUS_CMD_TOGGLE )
	{
		//Toggle switch pin

	}else if(cmd == WIREBUS_DATA_SWITCH)
	{	
		//Switch pin according to data byte
		switch( packet.data == 1 )
		{
			case 1:
				//Switch full
				dimmer = 128;
				
				break;
		
		
		}

	}else if( cmd == WIREBUS_DATA_SET )
	{
		//Data format : CXXXXXXX
		//C = 1 XXXXXXX = Divider (min/sec)
		//C = 2 Dimmer 0-128
		//C = 3 Timer  0-128 Note: timer delay in min or sec according to divider 
	}

}


