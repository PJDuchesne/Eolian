/*
__/\\\\\\\\\\\\\_____/\\\\\\\\\\\__/\\\\\\\\\\\\____        
 _\/\\\/////////\\\__\/////\\\///__\/\\\////////\\\__       
  _\/\\\_______\/\\\______\/\\\_____\/\\\______\//\\\_      
   _\/\\\\\\\\\\\\\/_______\/\\\_____\/\\\_______\/\\\_     
    _\/\\\/////////_________\/\\\_____\/\\\_______\/\\\_    
     _\/\\\__________________\/\\\_____\/\\\_______\/\\\_   
      _\/\\\___________/\\\___\/\\\_____\/\\\_______/\\\__  
       _\/\\\__________\//\\\\\\\\\______\/\\\\\\\\\\\\/___
        _\///____________\/////////_______\////////////_____

-> Name:  library.cpp
-> Brief: Implementation for the library.cpp, a helper library
-> Date: June 6, 2017   (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <iomanip>

#include "Include/library.h"

// Returns true if a S9 is found with a start address, false if no S9 is found

// Loads the file currently stored in fin (a global variable)
void load_file()
{
	std::string current_record = "";
	bool end = false;

	int s1_pos = 0; // Position of memory location in
	unsigned char cnt = 0;
	unsigned short address = 0;
	unsigned char s1_chksum = 0;
	unsigned char calc_chksum = 0;
	unsigned char temp_byte = 0;

	int i = 0; // S1 number

	while(1)
	{
		std::getline(fin, current_record);
		if(fin.eof()) break; // This is the only break
		if(current_record.substr(0, 2) == "S9") break;

		// Get CNT and address, use stoi to convert from hex string to numeric value
		cnt = std::stoi(current_record.substr(2, 2), nullptr, 16);
		address = std::stoi(current_record.substr(4, 4), nullptr, 16);
		s1_pos = 0;

		std::cout << "LINE #" << std::dec << i++ << ": >>" << current_record << "<<" << std::endl << std::hex;
		std::cout << std::hex << "\tCNT: " << (short)cnt << "\t | ADDR: " << address << std::endl;

		calc_chksum  = cnt; 			// This also resets the calc_chksum
		calc_chksum += address & 0x00ff; 	// Least significant 8 bits
		calc_chksum += (address & 0xff00)>>8; 	// Most significant 8 bits

		while(s1_pos < cnt-3) // Count includes the address and count itself
		{
			// Grab data at this point

			std::cout << "INPUTTING >>" << current_record.substr((8+s1_pos*2), 2) << "<<" << std::endl;

			temp_byte = std::stoi(current_record.substr((8+s1_pos*2), 2), nullptr, 16);

			// Emit to memory and increment s1_pos
			mem_array[address+s1_pos++] = temp_byte;
			
			// Update to checksum

			calc_chksum += temp_byte;
		}

		s1_chksum = std::stoi(current_record.substr((8+s1_pos*2), 2), nullptr, 16);

		// Check checksum!
		if((s1_chksum + calc_chksum) != 0xff)
		{
			std::cout << "ERROR ON CHECKSUM >>" << std::hex << (short)calc_chksum << "<< (< CALC) VS (GIVEN >) >>" << (short)s1_chksum << "<<" << std::endl;
			getchar();
			return;
		}
		else std::cout << "CHECKSUM CORRECT" << std::endl;
	}
}

// Dumps contents of memory into the output memory for diagnostics
void dump_mem()
{
	// Set options for output stream
	outfile << std::right << std::setfill('0') << std::hex;
	// Output to file
	for(int i = 0; i < MAX_MEM_SIZE; i++) outfile << std::setw(4) << i << "\t" << std::setw(2) << (short)mem_array[i] << std::endl;
}

// BUS_CTRL

void bus(int &mar, int &mdr, BUS_CTRL ctrl)
{
	switch(ctrl)
	{
		case READ_W:
			std::cout << "[BUS] Read Word!\n" << std::endl;


			break;
		case READ_B:
			std::cout << "[BUS] Read Byte!\n" << std::endl;


			break;
		case WRITE_W:
			std::cout << "[BUS] Write Word!\n" << std::endl;


			break;
		case WRITE_B:
			std::cout << "[BUS] Write Byte!\n" << std::endl;


			break;
		default:
			std::cout << "[BUS] INVALID BUS INPUT - STOPPING PROGRAM" << std::endl;
			getchar();			
			exit;
			break;
	}
}

