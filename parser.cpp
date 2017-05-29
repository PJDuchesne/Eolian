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

-> Name:  parser.h
-> Brief: Header file for parser.h.cpp
-> Date: May 18, 2017
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>

#include "Include/parser.h"
#include "Include/symtbl.h"
#include "Include/library.h"

// NOTE: Double check that I check the max number of characters of an operand

ADDR_MODE parse(std::string op, int& value0, int& value1)
{
	std::string operand = op;
	std::string temp_indexed;

	bool hex_flag = false;

	int temp_cnt = 0;

	value0 = -1;  // The general return value
	value1 = -1;  // The return value of the REGISTER in INDEXED mode
	symtbl_entry* se_ptr = NULL;

	bool auto_flag = false;

	switch (operand[0]) // Switch 
	{
		case '&': 	// (38) Absolute Mode or BUST    // 38

			std::cout << "ABSOLUTELY" << std::endl;

			operand.erase(0, 1);
			if(operand == "") return WRONG; // This means the input was just "&"
			
			se_ptr = get_symbol(operand);

			std::cout << "ABSOLUTELY" << std::endl;
		 	// If symbol not in table, and the operand is a valid label, add to symbol table
			if(se_ptr==NULL && valid_symbol(operand))
			{ // Valid symbol that isnt already on symbol table, FORWARD REFERNCE
				std::cout << "ABSOLUTELY" << std::endl;
				add_symbol(operand, -1, UNKNOWN);
				value0 = -1;
				if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
				else return ABSOLUTE; 
			}
			else if(se_ptr != NULL)
			{ // Valid symbol from symbol table, need to check that it isn't a REG
				if(se_ptr->type != REG)
				{
					value0 = se_ptr->value;
					if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
					else return ABSOLUTE;
				}
				else return WRONG; // Cannot be REG
			}
			else return WRONG; // Therefore not in symbol table, but also not valid symbol
			break;
		case '@':	// (64) Indirect or Indirect auto-increment (OR BUST)
			operand.erase(0, 1);

			if(operand == "") return WRONG; // This means the input was "@" alone

			// If auto indirect, it will have a + after the symbol. Remove it and trigger flag if so
			if(operand.find_last_of("+") == operand.length()-1)
			{
				operand.erase(operand.length()-1, operand.length()); // This needs testing
				if(operand == "") return WRONG; // This means the input was "@+" alone
				auto_flag = true;
			}

			// Check symbol table for operand, it must be a REG
			se_ptr = get_symbol(operand);
			if (se_ptr == NULL) return WRONG; // INVALID SYMBOL FROM INDIRECT AUTO INCREMENT
			else if (se_ptr->type !=REG) return WRONG; // INVALID SYMTBOL TYPE FROM INDIRECT AUTO INCREMENT
			else value0 = se_ptr->value;	// Register value, will be between 0 and 15
			
			if(auto_flag) return INDIRECT_AI;
			else return INDIRECT;
			break;

		case '#':	// (35) Immediate (OR BUST)
			operand.erase(0, 1);

			if(operand == "") return WRONG; // This means the input was "#" alone
			
			se_ptr = get_symbol(operand);

			if(se_ptr == NULL && valid_symbol(operand))
			{ // Forward reference of label within immediate // This is intentionally never done if this is called for directives
				add_symbol(operand, -1, UNKNOWN);
				value0 = -1;
				return IMMEDIATE;
			}
			if(se_ptr != NULL && se_ptr->type != REG)
			{ // Constant is the value from the label
				value0 = se_ptr->value;
				if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
				else return IMMEDIATE;
			}
			else
			{ // Value is a HEX number or DECIMAL number (Or bust!)
				std::cout << "\t\tOPERAND >>" << operand << "<<" << std::endl;
	 			if(operand[0] == '$')
				{
					operand.erase(0, 1); 
					if(operand == "") return WRONG; // Means operand was #$
					hex_flag = true;
				}
				else if(operand[0] == '-' && operand.length() == 1) return WRONG; // #- is an invalid immediate
				while(operand[0] == '0' && operand.length() > 1) operand.erase(0, 1); // Delete preceding 0s

				if(operand.length() > 8 && hex_flag) return WRONG; 					  // TOO LONG FOR STOL (Hex)
				if(operand.length() > 10 && !hex_flag) return WRONG; 				  // TOO LONG FOR STOL (Decimal)
				// Check that all remaining characters are numeric

				if(hex_flag && operand.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos) return WRONG;
				else if(!hex_flag && operand.find_first_not_of("-0123456789") != std::string::npos) return WRONG;

				// THEREFORE: "Operand" is numeric and contains a number

				value0 = std::stol(operand, nullptr, hex_flag ? 16 : 10);

				if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
				else return IMMEDIATE;
			}
			break;
		default:	// Reg, Indexed, Relative	
			if(operand.find_first_of("(") != -1 && operand.find_first_of(")") != -1) // If either is not found, 'find_first_of' returns n_pos, which is equal to -1
			{	// If true, this is Indexed (OR BUST)

				if(operand.find_first_of("(") + 1 == operand.find_first_of(")")) return WRONG; // Invalid INDEXED OPERAND (Closing bracket appears before opening bracket?)
				if(operand.find_first_of("(") > operand.find_first_of(")")) return WRONG; // Invalid INDEXED OPERAND (Closing bracket appears before opening bracket?)

				while(operand[0] != '(')
				{
					temp_indexed += operand[0]; // temp_indexed is the x in x(Rn) (the label, not REG)
					operand.erase(0,1);
				}

				operand.erase(0,1); // Erases the "("

				if(operand.find_first_of(")") != operand.length()-1) return WRONG; // Invalid closing bracket position (Not last character)
				operand.pop_back(); // Removes last character of the string, which is ")" in this case

				// temp_indexed is the x in x(Rn)
				// operand is the Rn in x(Rn)
				// Now check validity of both

				// Check validity of X in X(Rn)
				se_ptr = get_symbol(temp_indexed);
				if(get_symbol(temp_indexed) == NULL && valid_symbol(temp_indexed))
				{
					add_symbol(temp_indexed, -1, UNKNOWN);
					value0 = -1;
				}
				else if(se_ptr->type == REG) return WRONG; // X in x(Rn) cannot be a register
				else // X is KNOWN or UNKNOWN
				{
					value0 = se_ptr->value;
				}
				// Else KNOWN or UNKNOWN, therefore set value0 to se_ptr->value

				// Check validity of Rn in x(Rn)
				se_ptr = get_symbol(operand);
				if(se_ptr != NULL)
				{
					if(se_ptr->type == REG) value1 = se_ptr->value;
					else return WRONG; // Rn in x(Rn) must be REG type
				}
				else return WRONG; // Must be REG

				if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
				else if(value1 < 0 || value1 > 15) return WRONG; // Value1 cannot be outside of REG values (0, 15)
				else return INDEXED;		
			}
			else if(get_symbol(operand) == NULL && valid_symbol(operand))
			{
				add_symbol(operand, -1, UNKNOWN);
				value0 = -1;
				if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
				else return RELATIVE;
			}
			else if(get_symbol(operand) != NULL)
			{ // Reg or relative	
				se_ptr = get_symbol(operand);
				value0 = se_ptr->value;
				if(se_ptr->type == REG) return REG_DIRECT;
				else if(value0 < -32768 || value0 > 65535) return WRONG; // Value0 cannot be larger than a word
				else return RELATIVE;
			}
			break;
	}		
	return WRONG;
}

