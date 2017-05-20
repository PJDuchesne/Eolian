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

-> Name:  symtbl.cpp
-> Brief: Implements the symtbl with functions and such
-> Date: May 15, 2017   (Created)
-> Date: May 17, 2017   (Last Modified)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cstdlib>

#include "symtbl.h"

std::string types[] = {"REG", "KNO", "UNK"};

// Actual pointer to the symbol table
symtbl_entry* symtbl_master = NULL;

void init_symtbl()
{
	// Add r0-r15, R0-R15, plus aliases
	add_symbol("R0",  0,  REG);
	add_symbol("R1",  1,  REG);
	add_symbol("R2",  2,  REG);
	add_symbol("R3",  3,  REG);
	add_symbol("R4",  4,  REG);
	add_symbol("R5",  5,  REG);
	add_symbol("R6",  6,  REG);
	add_symbol("R7",  7,  REG);
	add_symbol("R8",  8,  REG);
	add_symbol("R9",  9,  REG);
	add_symbol("R10", 10, REG);
	add_symbol("R11", 11, REG);
	add_symbol("R12", 12, REG);
	add_symbol("R13", 13, REG);
	add_symbol("R14", 14, REG);
	add_symbol("R15", 15, REG);
	add_symbol("r0",  0,  REG);
	add_symbol("r1",  1,  REG);
	add_symbol("r2",  2,  REG);
	add_symbol("r3",  3,  REG);
	add_symbol("r4",  4,  REG);
	add_symbol("r5",  5,  REG);
	add_symbol("r6",  6,  REG);
	add_symbol("r7",  7,  REG);
	add_symbol("r8",  8,  REG);
	add_symbol("r9",  9,  REG);
	add_symbol("r10", 10, REG);
	add_symbol("r11", 11, REG);
	add_symbol("r12", 12, REG);
	add_symbol("r13", 13, REG);
	add_symbol("r14", 14, REG);
	add_symbol("r15", 15, REG);
	add_symbol("PC",  0,  REG);
	add_symbol("Pc",  0,  REG);
	add_symbol("pC",  0,  REG);
	add_symbol("pc",  0,  REG);
	add_symbol("SP",  1,  REG);
	add_symbol("Sp",  1,  REG);
	add_symbol("sP",  1,  REG);
	add_symbol("sp",  1,  REG);
	add_symbol("SR",  2,  REG);
	add_symbol("Sr",  2,  REG);
	add_symbol("sR",  2,  REG);
	add_symbol("sr",  2,  REG);
	add_symbol("CG1", 2,  REG);
	add_symbol("cG1", 2,  REG);
	add_symbol("Cg1", 2,  REG);
	add_symbol("cg1", 2,  REG);
	add_symbol("CG2", 3,  REG);
	add_symbol("Cg2", 3,  REG);
	add_symbol("cG2", 3,  REG);
	add_symbol("cg2", 3,  REG);
}

void add_symbol(std::string label, int value, symtbltype type)
{
	symtbl_entry* new_entry = new symtbl_entry();
	new_entry->label = label;
	new_entry->value = value;
	new_entry->type = type;
	new_entry->next = symtbl_master;	// Link new entry to next entry
	symtbl_master = new_entry;	// Set ptr to the new entry
}

void output_symtbl()
{
	int temp_cnt = 0;
        // Iterate through points by using the "next" pointer on each value
        symtbl_entry* temp = symtbl_master;
        while(temp->next != NULL)
        {
                std::cout << "Entry #"    << temp_cnt;
		std::cout << " | Label: " << temp->label;
		std::cout << " | Value: " << temp->value;
		std::cout << " | type: "  << temp->type << std::endl;
                temp = temp->next;
                temp_cnt++;
        }
}

symtbl_entry* get_symbol(std::string label)
{	
	symtbl_entry* temp = symtbl_master;
	
	while(temp->next != NULL)
	{
		if(temp->label == label) return temp;
		temp=temp->next;
	}
	return NULL;
}

bool valid_symbol(std::string token)
{
	// If token is A-Z or a-z or _ (Alphabetic)
	if(token.length() > 32) return false; // TOKEN IS TOO LONG
	else if(((token[0] >= 65) && (token[0] <= 90))||((token[0] >= 97) && (token[0] <= 122))||(token[0] == 95))
	{
		int temp_cnt = 1;
		while(temp_cnt < token.length())
		{
			// If token is A-Z or a-z or _ (Alphabetic)
			if(!((token[temp_cnt] >= 65 && token[temp_cnt] <= 90)||(token[temp_cnt] >= 97 && token[temp_cnt] <= 122)||(token[temp_cnt] == 95))) break;
			temp_cnt++;
		}
		while(1)
		{
			// Can only have 2 more charaters, MAX
			if(token.length() - temp_cnt > 2) return false; // too many characters after alphabetic section
			else if(token.length() - temp_cnt == 1) // If this is the only character left
			{
				// Number can be 0-9
				if(!((token[temp_cnt] >= 48)&&(token[temp_cnt] <= 57))) return false; // NUMBER TOO HIGH OR NOT NUMBER
			}
			else if(token.length() - temp_cnt == 2) // If this is the second last token
			{
				// Number can be 0-3 (SHOULD THIS 0 BE ALLOWED?)
				if(!((token[temp_cnt] >= 48)&&(token[temp_cnt] <= 51))) return false; // NUMBER TOO HIGH OR NOT NUMBER
			 	if(token[temp_cnt] == 51)  // If second last digit is 3, next one MUST be 0
				{
					temp_cnt++;
					return (token[temp_cnt] == 48) ? true : false; 
				}
				else
				{
					temp_cnt++;
					return ((token[temp_cnt] >= 48)&&(token[temp_cnt] <= 57)) ? true : false; // Return true if the character is from 0-9, false otherwise
				}
			}
			return true;
		}
	}
	else return false;

	std::cout << "THIS SHOULD NEVER HAPPEN IN VALID_SYMBOL CHECKER" << std::endl;
}