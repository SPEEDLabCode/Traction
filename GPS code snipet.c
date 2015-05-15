	unsigned char GPS_RMC_ON[] = "$PSRF103,04,00,01,01     ";		//RMC on once a second
	unsigned char GPS_RMC_OFF[] = "$PSRF103,04,00,00,01     ";		//RMC off
	unsigned char GPS_RMC_QRY[] = "$PSRF103,04,01,00,01     ";		//RMC query
	unsigned char GPS_GGA_OFF[] = "$PSRF103,00,00,00,01     ";		//GGA off
	unsigned char GPS_GLL_OFF[] = "$PSRF103,01,00,00,01     ";		//GLL off
	unsigned char GPS_GSA_OFF[] = "$PSRF103,02,00,00,01     ";		//GSA off
	unsigned char GPS_GSV_OFF[] = "$PSRF103,03,00,00,01     ";		//GGA off
	unsigned char GPS_VTG_OFF[] = "$PSRF103,05,00,00,01     ";		//VTG off
	tickOFF = 10;
	LCD_BL = 1;
	tickBL = 11;
	button_flag = 0;
	while(1)
	{

  	_INT1IE = 0; /* disable external interrupt 1 */

/* these comands only get executed when powering on */
    if(!Initialized_Flag) InitializeSystem();
	if(!Powered_Flag)	power_up();
	if(!Mag_Flag)		MAG_enable();
	if(!GPS_Flag){		
		gps_enable();
		delay_ticks(500);
	
		gps_comand(GPS_GGA_OFF);
		gps_comand(GPS_GLL_OFF);
		gps_comand(GPS_GSA_OFF);
		gps_comand(GPS_GSV_OFF);
		gps_comand(GPS_VTG_OFF);
		gps_comand(GPS_RMC_OFF);
		}
///****************************************************/

	colat = 0;
	elong = 0;
	alt = 0;
	heading = 0;

	if ((valid_data == 0) || (tickGPS == 0)){
//		GREEN_LED = 1;
		gps_comand(GPS_RMC_QRY); //get the next RMC packet
		valid_data = 4;
		tickGPS = 5;
//		GREEN_LED = 0;
	}

	if (valid_data == 2) {
//		GREEN_LED = 1;
	Nop();
	Nop();
	Nop();
	Nop();
		i = 0;
		valid_data = 0;
		while ((dataG[i] != 44) && ( i <= sizeG)) ++i;	//check for first comma
		++i;
		while ((dataG[i] != 44) && ( i <= sizeG)) ++i;	//check for second comma
		++i;
		if ((dataG[i] == 65) && ( i <= sizeG)){			// if this is A then we have valid data
			valid_data = 1;
			++i;
			++i;
			j = i;
			while ((dataG[i] != 46) && ( i <= sizeG))++i;// go to decimal point
			Mx = ((dataG[i+1]-48) * 1000)+((dataG[i+2]-48) * 100)+((dataG[i+3]-48) * 10)+(dataG[i+4]-48);
					//That was the 4 decimal places of the minutes
			X = (double)Mx;
			colat = X/10000;
			if ((i-j) == 1) {
				Mx = (dataG[i-1]-48);	//only 1 unit of minutes
			}else if ((i-j) >= 2) {
				Mx = ((dataG[i-2]-48)*10)+(dataG[i-1]-48);
			}
			X = (double)Mx;
			Mx = 0;
			colat = colat + X;	//we now have the minutes
			colat = colat / 60.0;	//we now have decimal degrees
			if ((i-j) == 3) { 		
				Mx = (dataG[i-3]-48);
			}else if ((i-j) == 4) {			
				Mx = ((dataG[i-4]-48)*10)+(dataG[i-3]-48);
			}	//that is the two degree digits as lattitude can only go to 90 degrees.
			X = (double)Mx;
			colat = colat + X;	//that the decimal lattitude just need to know east or west now

			while ((dataG[i] != 44) && ( i <= sizeG)) ++i;	//go to next comma
			++i;
			if (dataG[i] == 83) colat = -1 * colat; // as we are south.
			++i;
			++i;
			j = i;
			while ((dataG[i] != 46) && ( i <= sizeG))++i;// go to decimal point
			Mx = ((dataG[i+1]-48) * 1000)+((dataG[i+2]-48) * 100)+((dataG[i+3]-48) * 10)+(dataG[i+4]-48);
					//That was the 4 decimal places of the minutes
			X = (double)Mx;
			elong = X/10000;
			if ((i-j) == 1) {
				Mx = (dataG[i-1]-48);	//only 1 unit of minutes
			}else if ((i-j) >= 2) {
				Mx = ((dataG[i-2]-48)*10)+(dataG[i-1]-48);
			}
			X = (double)Mx;
			Mx = 0;
			elong = elong + X;		//we now have the minutes
			elong = elong / 60.0; 	//we now have decimal degrees
			if ((i-j) == 3) { 		
				Mx = (dataG[i-3]-48);
			}else if ((i-j) == 4){			
				Mx = ((dataG[i-4]-48)*10)+(dataG[i-3]-48);
			}else if ((i-j) == 5){			
				Mx = ((dataG[i-5]-48)*100)+((dataG[i-4]-48)*10)+(dataG[i-3]-48);
			}
			X = (double)Mx;
			elong = elong + X; //thats the decimal longditude just need to go north or south now

			while ((dataG[i] != 44) && ( i <= sizeG)) ++i;	//go to next comma
			++i;
			if (dataG[i] == 87) elong = -1 * elong; // as we are west.

			++i;
			++i;
			while ((dataG[i] != 44) && ( i <= sizeG)) ++i;	// we skip over speed in knots
			++i;
			j = i;
			while ((dataG[i] != 46) && ( i <= sizeG)) ++i;	// go to decimal point
			Mx = ((dataG[i+1]-48) * 0x0A) + (dataG[i+2]-48);
			heading = (double)Mx;
			heading = heading/100.0;	//decimal part of heading.
			Mx = 0;
			if ((i-j) == 1){
				Mx = (dataG[i-1]-48);
			}else if((i-j) == 2){
				Mx = ((dataG[i-2]-48) * 0x0A) + (dataG[i-1]-48);
			}else if((i-j) == 3){
				Mx = ((dataG[i-3]-48) * 0x64) + ((dataG[i-2]-48) * 0x0A) + (dataG[i-1]-48);
			}
			X = (double)Mx;
			heading = heading + X;
			if (heading >= 360) heading = heading - 360;
			if (heading < 0) heading = heading + 360;	
	Nop();
	Nop();
	Nop();
	Nop();
			
			}//we have now disected the RMC packet phew!
	}