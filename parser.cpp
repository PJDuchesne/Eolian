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

#include "parser.h"
#include "symtbl.h"
#include "library.h"

addr_mode parse(std::string op, int& value0, int& value1)
{
	std::string operand = op;
	std::string temp_indexed;

	bool hex_flag = false;

	int temp_cnt = 0;

	value0 = -1;  // The general return value
	value1 = -1;  // The return value of the REGISTER in INDEXED mode
	operand = "@R10+";

	symtbl_entry* se_ptr = NULL;

	bool auto_flag = false;

	switch (operand[0]) // Switch 
	{
		case '&': 	// (38) Absolute Mode or BUST    // 38
			operand.erase(0, 1);
			
			se_ptr = get_symbol(operand);

		 	// If symbol not in table, and the operand is a valid label, add to symbol table
			if(se_ptr==NULL && valid_symbol(operand)) add_symbol(operand, -1, UNKNOWN);
			else 
			{
				if(se_ptr->type != REG)
				{
					value0 = se_ptr->value;
					return ABSOLUTE;
				}
			}
			break;
		case '@':	// (64) Indirect or Indirect auto-increment (OR BUST)
			operand.erase(0, 1);

			// If auto indirect, it will have a + after the symbol. Remove it and trigger flag if so
			if(operand.find_last_of("+") == operand.length()-1)
			{
				operand.erase(operand.length()-1, operand.length()); // This needs testing
				auto_flag = true;
			}

			// Check symbol table for operand, it must be a REG
			se_ptr = get_symbol(operand);
			if (se_ptr == NULL) return WRONG; // INVALID SYMBOL FROM INDIRECT AUTO INCREMENT
			else if (se_ptr->type !=REG) return WRONG; // INVALID SYMTBOL TYPE FROM INDIRECT AUTO INCREMENT
			else
			{
 				value0 = se_ptr->value;
			}
			se_ptr->value += (auto_flag) ? 1 : 0;
			return INDIRECT;
			break;

		case '#':	// (35) Immediate (OR BUST)
			operand.erase(0, 1);
			
			se_ptr = get_symbol(operand);

			if(se_ptr != NULL && se_ptr->type != REG)
			{ // Constant is the value from the label
				value0 = se_ptr->value;
				return IMMEDIATE;
			}
			else
			{ // Value is a HEX number or DECIMAL number (Or bust!)
	 			if(operand[0] == '$') operand.erase(0, 1); 
				while(operand[0] == '0') operand.erase(0, 1); // Delete preceding 0s
				if(operand.length() > 8 && hex_flag) return WRONG; // TOO LONG FOR STOL (Hex)
				if(operand.length() > 10 && !hex_flag) return WRONG; // TOO LONG FOR STOL (Decimal)
				// Check that all remaining characters are numeric
				while(temp_cnt < operand.length()-1)
				{
					if(!((operand[temp_cnt] >= 48)&&(operand[temp_cnt] <= 57))) return WRONG;
					temp_cnt++;
				}
				
				// THEREFORE: "Operand" is numeric and contains a number

				value0 = std::stol(operand, nullptr, hex_flag ? 16 : 10);

				return IMMEDIATE;
			}
			break;
		default:	// Reg, Indexed, Relative	
			if(operand.find_first_of("(") != -1 && operand.find_first_of(")") != -1) // If either is not found, 'find_first_of' returns n_pos, which is equal to -1
			{	// If true, this is Indexed (OR BUST)
				if(operand.find_first_of("(") > operand.find_first_of(")")) return WRONG; // Invalid INDEXED OPERAND (Closing bracket appears before opening bracket?)
				while(operand[0] != '(')
				{
					temp_indexed += operand[temp_cnt]; // temp_indexed is the x in x(Rn) (the label, not REG)
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
				if(se_ptr->type == REG) return WRONG; // X in x(Rn) cannot be a register
				else if(se_ptr == NULL)
				{
					if(valid_symbol(temp_indexed)) add_symbol(temp_indexed, -1, UNKNOWN);
				}
				// Else KNOWN or UNKNOWN, therefore set value0 to se_ptr->value
				value0 = se_ptr->value;

				// Check validity of Rn in x(Rn)
				se_ptr = get_symbol(operand);
				if(se_ptr->type == REG) value1 = se_ptr->value;
				else return WRONG; // Rn in x(Rn) must be REG type

				return INDEXED;		
			}
			else if(get_symbol(operand) != NULL)
			{ // Reg or relative	
				se_ptr = get_symbol(operand);
				value0 = se_ptr->value;
				if(se_ptr->type == REG) return REG_DIRECT;
				else return RELATIVE;
			}
			else
			{	// The label is not in the symbol table, check if it is a valid label and add to symtbl if so
				if(valid_symbol(operand)) add_symbol(operand, -1, UNKNOWN);
				else return WRONG; // INVALID OPERAND
			}
			break;
	}		


	return WRONG;
}
