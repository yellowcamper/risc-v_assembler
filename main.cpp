/**
 * \mainpage RISC-V Assembler
 * \details This is a C++ class that Assembles RISC-V Assembly into hex machine code for the use in simulators and programming projects.
 * \n \n
 * --- Github repository link: <a href="https://github.com/yellowcamper/risc-v_assembler">https://github.com/yellowcamper/risc-v_assembler</a>
 * \n
 * \author Kenneth Michael (Mikey) Neal
 * \date Initial Upload: 23 September 2021
 * \copyright Kenneth Michael (Mikey) Neal (c) 23 September 2021 under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * \remark This code does not currently support the whole RISC-V ISA, including psudo-instructions.
 * \remark This code is still prerelease, do please report any bugs or errors through the github issue tracker.
 */

/**
 * \file main.cpp
 * \brief This is the heart of the RISC-V Assembler.
 * \details --- Github repository link: <a href="https://github.com/yellowcamper/risc-v_assembler">https://github.com/yellowcamper/risc-v_assembler</a>
 * \author Kenneth Michael (Mikey) Neal
 * \date Initial Upload: 23 September 2021
 * \copyright Kenneth Michael (Mikey) Neal (c) 23 September 2021 under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * \remark This code is still prerelease, do please report any bugs or errors through the github issue tracker.
 */
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <sstream>
#include <string>
#include <map>

using namespace std;

/**
 * \brief \c risc_v_assembler is a class that allows for the assembly of RISC-V into a file.
 * \note If you would like a binary executable, edit the fprintf statement in the \c process() function.
 */
class risc_v_assembler {
	protected:
		/**
		 * \brief \c input_file holds the name of the input file.
		 */
		char * input_file = nullptr;
		/**
		 * \brief \c output_file holds the name of the output file.
		 */
		char * output_file = nullptr;
		/**
		 * \brief \c labels holds the locations of all of the labels in the file by the location of the next instruction.
		 */
		map <string, uint64_t> labels;
		
		uint32_t getRegister(string, uint8_t);
		uint32_t getOpcode(string, char&);
		void makeLabel(string, uint64_t);
		uint64_t findLabelPos(string);
		uint32_t processLine(string, uint64_t);
	public:
		/**
		 * \brief Default constructor.
		 */
		risc_v_assembler() {}
		/**
		 * \brief Constructor with file names.
		 * 
		 * \param [in] input_file_name is the name of the input file.
		 * \param [in] output_file_name is the name of the output file.
		 */
		risc_v_assembler(char * input_file_name, char * output_file_name) {
			input_file = input_file_name;
			output_file = output_file_name;
		}
		
		void process();
		char * getInputFile();
		char * getOutputFile();
		void setInputFile(char * );
		void setOutputFile(char * );
		
};

/**
 * \brief \c getRegister() is a function that interprets strings and gives the corresponding register out.
 * 
 * \param [in] input is a string to be interpreted as a register.
 * \param [in] offset is the logical shift left amount for the output. 
 * \return the register number 0-31
 */
uint32_t risc_v_assembler::getRegister(string input, uint8_t offset = 0) {
	uint32_t numeric_part = 0;
	string non_numeric_part = "";
	
	if (input.size() > 4) {
		cerr << "ERROR: invalid input in register name.\n";
		abort();
	}
	
	for (int i = 0; i < input.size(); i++) {
		if ((input.at(i) <= '9') && (input.at(i) >= '0')) {
			numeric_part += ((input.at(i) - '0') * (input.size() - i - 1) * 10) + ((input.at(i) - '0') * (i == input.size() - 1));
		} else {
			non_numeric_part += input.at(i);
		}
	}
	
	if ((non_numeric_part.compare("x") == 0) && (numeric_part < 32)){
		return numeric_part << offset;
	} else if ((non_numeric_part.compare("t") == 0) && (numeric_part < 7)){
		if (numeric_part < 3) {
			return (5 + numeric_part) << offset;
		} else {
			return (25 + numeric_part) << offset;
		}
	} else if ((non_numeric_part.compare("s") == 0) && (numeric_part < 12)){
		if (numeric_part < 2) {
			return (8 + numeric_part) << offset;
		} else {
			return (16 + numeric_part) << offset;
		}
	} else if ((non_numeric_part.compare("a") == 0) && (numeric_part < 8)){
		return (10 + numeric_part) << offset;
	} else if ((non_numeric_part.compare("ra") == 0) && (input.size() == 2)) {
		return (1) << offset;
	} else if ((non_numeric_part.compare("sp") == 0) && (input.size() == 2)) {
		return (2) << offset;
	} else if ((non_numeric_part.compare("gp") == 0) && (input.size() == 2)) {
		return (3) << offset;
	} else if ((non_numeric_part.compare("tp") == 0) && (input.size() == 2)) {
		return (4) << offset;
	} else if ((non_numeric_part.compare("fp") == 0) && (input.size() == 2)) {
		return (8) << offset;
	} else if ((non_numeric_part.compare("zero") == 0) && (input.size() == 4)) {
		return (0) << offset;
	}
	
	cerr << "ERROR: invalid input in register name \""<< input <<"\"\n";
	abort();
	
	return 0;
}

/**
 * \brief \c getOpcode() compares a string with an instruction list to determine what type it is and the base opcode. 
 * 
 * \param [in] input is the instruction to be compared.
 * \param [out] instruction_type is the RISC-V instruction type.
 * \returns The base opcode for an instruction.
 * 
 * \details This function will error out if an unknown opcode is entered.
 * \note This is the function that needs to be edited to add more instructions.
 */
uint32_t risc_v_assembler::getOpcode(string input, char &instruction_type) {
	instruction_type = 0;
	if (input.compare("lb") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000000000000000011;
	} else if (input.compare("lh") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000001000000000011;
	} else if (input.compare("lw") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000010000000000011;
	} else if (input.compare("ld") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000011000000000011;
	} else if (input.compare("lbu") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000100000000000011;
	} else if (input.compare("lhu") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000101000000000011;
	} else if (input.compare("lwu") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000110000000000011;
	} else if (input.compare("addi") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000000000000010011;
	} else if (input.compare("slli") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000001000000010011;
	} else if (input.compare("slti") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000010000000010011;
	} else if (input.compare("sltiu") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000011000000010011;
	} else if (input.compare("xori") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000100000000010011;
	} else if (input.compare("srli") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000101000000010011;
	} else if (input.compare("srai") == 0) {
		instruction_type = 'I';
		return 0b01000000000000000101000000010011;
	} else if (input.compare("ori") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000110000000010011;
	} else if (input.compare("andi") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000111000000010011;
	} else if (input.compare("auipc") == 0) {
		instruction_type = 'U';
		return 0b00000000000000000000000000010111;
	} else if (input.compare("addiw") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000000000000011011;
	} else if (input.compare("slliw") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000001000000011011;
	} else if (input.compare("srliw") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000101000000011011;
	} else if (input.compare("sraiw") == 0) {
		instruction_type = 'I';
		return 0b01000000000000000101000000011011;
	} else if (input.compare("sb") == 0) {
		instruction_type = 'S';
		return 0b00000000000000000000000000100011;
	} else if (input.compare("sh") == 0) {
		instruction_type = 'S';
		return 0b00000000000000000001000000100011;
	} else if (input.compare("sw") == 0) {
		instruction_type = 'S';
		return 0b00000000000000000010000000100011;
	} else if (input.compare("sd") == 0) {
		instruction_type = 'S';
		return 0b00000000000000000011000000100011;
	} else if (input.compare("add") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000000000000110011;
	} else if (input.compare("sub") == 0) {
		instruction_type = 'R';
		return 0b01000000000000000000000000110011;
	} else if (input.compare("sll") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000001000000110011;
	} else if (input.compare("slt") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000010000000110011;
	} else if (input.compare("sltu") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000011000000110011;
	} else if (input.compare("xor") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000100000000110011;
	} else if (input.compare("srl") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000101000000110011;
	} else if (input.compare("sra") == 0) {
		instruction_type = 'R';
		return 0b01000000000000000101000000110011;
	} else if (input.compare("or") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000110000000110011;
	} else if (input.compare("and") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000111000000110011;
	} else if (input.compare("mul") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000000000000110011;
	} else if (input.compare("mulh") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000010000000110011;
	} else if (input.compare("mulhsu") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000010000000110011;
	} else if (input.compare("mulhu") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000011000000110011;
	} else if (input.compare("div") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000100000000110011;
	} else if (input.compare("divu") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000101000000110011;
	} else if (input.compare("rem") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000110000000110011;
	} else if (input.compare("remu") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000111000000110011;
	} else if (input.compare("lui") == 0) {
		instruction_type = 'U';
		return 0b00000000000000000000000000110111;
	} else if (input.compare("addw") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000000000000111011;
	} else if (input.compare("subw") == 0) {
		instruction_type = 'R';
		return 0b01000000000000000000000000111011;
	} else if (input.compare("sllw") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000001000000111011;
	} else if (input.compare("srlw") == 0) {
		instruction_type = 'R';
		return 0b00000000000000000101000000111011;
	} else if (input.compare("sraw") == 0) {
		instruction_type = 'R';
		return 0b01000000000000000101000000111011;
	} else if (input.compare("mulw") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000000000000111011;
	} else if (input.compare("divw") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000100000000111011;
	} else if (input.compare("divuw") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000101000000111011;
	} else if (input.compare("remw") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000110000000111011;
	} else if (input.compare("remuw") == 0) {
		instruction_type = 'R';
		return 0b00000010000000000111000000111011;
	} else if (input.compare("beq") == 0) {
		instruction_type = 'B';
		return 0b00000000000000000000000001100011;
	} else if (input.compare("bne") == 0) {
		instruction_type = 'B';
		return 0b00000000000000000001000001100011;
	} else if (input.compare("blt") == 0) {
		instruction_type = 'B';
		return 0b00000000000000000100000001100011;
	} else if (input.compare("bge") == 0) {
		instruction_type = 'B';
		return 0b00000000000000000101000001100011;
	} else if (input.compare("bltu") == 0) {
		instruction_type = 'B';
		return 0b00000000000000000110000001100011;
	} else if (input.compare("bgeu") == 0) {
		instruction_type = 'B';
		return 0b00000000000000000111000001100011;
	} else if (input.compare("jalr") == 0) {
		instruction_type = 'I';
		return 0b00000000000000000000000001100111;
	} else if (input.compare("jal") == 0) {
		instruction_type = 'J';
		return 0b00000000000000000000000001101111;
	} 
	
	cerr << "ERROR: unrecognized command \"" << input << "\"\n";
	abort();
	
	return 0;
}

/**
 * \brief \c makeLabel() adds a label to branch/jump to. 
 * 
 * \param [in] name is the name of the branch.
 * \param [in] pos is the position.
 */
void risc_v_assembler::makeLabel(string name, uint64_t pos) {
	labels[name] = pos;
}



/**
 * \brief \c findLabelPos() gets the location of the label that was branched/jumped to. 
 * 
 * \param [in] name is the name of the branch.
 * \returns The location of the label.
 * 
 * \details This function will error out if an unknown label is entered.
 */
uint64_t risc_v_assembler::findLabelPos(string name) {
	map<string, uint64_t>::iterator it = labels.find(name);
	if (it == labels.end()) {
		cerr << "ERROR: undefined label \"" << name << "\"\n";
		abort();
	}
	return labels[name];
}

/**
 * \brief \c processLine() assembles the machine code for one line. 
 * 
 * \param [in] input is the line from the file.
 * \param [in] pos is the instruction number.
 * \returns The instruction in HEX.
 * 
 * \details This function will error out if there are any issues.
 * \note This is the function that needs to be edited to add more instruction types.
 */
uint32_t risc_v_assembler::processLine(string input, uint64_t pos) {
	stringstream ss_input(input);
	string temp;
	ss_input >> temp;

	char instruction_type;
	
	if ((temp.size() == 0 ) || (temp.at(0) == '#')) {
		return 0;
	}
	
	if (temp.at(temp.size() - 1) == ':') {
		ss_input >> temp;
	}
	
	if (!ss_input) {
		return 0;
	}
	
	if (temp.at(0) == '#') {
		return 0;
	}
	
	uint32_t instruction = getOpcode(temp, instruction_type);
	
	ss_input >> temp;
	
	if (!(ss_input && (temp.at(0) != '#'))) {
		cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
		abort();
	}
	
	switch (instruction_type) {
		case 'I':
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 7);
			
			ss_input >> temp;
			
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 15);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			if ((temp.size() >= 2) && (temp.at(0) == '0') && (temp.at(1) == 'x')) {
				instruction |= ((stoi(temp, nullptr, 16)) << 20);
			} else if ((temp.at(0) <= '9') && (temp.at(0) >= '0')) {
				instruction |= ((stoi(temp, nullptr)) << 20);
			} else {
				instruction |= (((findLabelPos(temp) - pos)) << 20);
			}
		break;
		case 'U':
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 7);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			if ((temp.size() >= 2) && (temp.at(0) == '0') && (temp.at(1) == 'x')) {
				instruction |= ((stoi(temp, nullptr, 16)) << 12);
			} else if ((temp.at(0) <= '9') && (temp.at(0) >= '0')) {
				instruction |= ((stoi(temp, nullptr)) << 12);
			} else {
				instruction |= (((findLabelPos(temp) - pos)) << 12);
			}
		break;
		case 'R':
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 7);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 15);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			instruction |= getRegister(temp, 20);
		break;
		case 'S':
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 15);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 20);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			if ((temp.size() >= 2) && (temp.at(0) == '0') && (temp.at(1) == 'x')) {
				instruction |= ((stoi(temp, nullptr, 16) &  0b11111) << 7 ) | 
							   ((stoi(temp, nullptr, 16) & ~0b11111) << 20);
			} else if ((temp.at(0) <= '9') && (temp.at(0) >= '0')) {
				instruction |= ((stoi(temp, nullptr) &  0b11111) << 7 ) | 
							   ((stoi(temp, nullptr) & ~0b11111) << 20);
			} else {
				instruction |= (((findLabelPos(temp) - pos) &  0b11111) << 7 ) | 
							   (((findLabelPos(temp) - pos) & ~0b11111) << 20);
			}
		break;
		case 'J':
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 15);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			if ((temp.size() >= 2) && (temp.at(0) == '0') && (temp.at(1) == 'x')) {
				instruction |= (((stoi(temp, nullptr, 16) >> 20) & 0x1  ) << 31) | 
							   (((stoi(temp, nullptr, 16) >> 1 ) & 0x3ff) << 21) | 
							   (((stoi(temp, nullptr, 16) >> 11) & 0x1  ) << 20) | 
							   (((stoi(temp, nullptr, 16) >> 12) & 0xff ) << 12);
			} else if ((input.at(0) <= '9') && (input.at(0) >= '0')) {
				instruction |= (((stoi(temp, nullptr) >> 20) & 0x1  ) << 31) | 
							   (((stoi(temp, nullptr) >> 1 ) & 0x3ff) << 21) | 
							   (((stoi(temp, nullptr) >> 11) & 0x1  ) << 20) | 
							   (((stoi(temp, nullptr) >> 12) & 0xff ) << 12);
			} else {
				instruction |= ((((findLabelPos(temp) - pos) >> 20) & 0x1  ) << 31) | 
							   ((((findLabelPos(temp) - pos) >> 1 ) & 0x3ff) << 21) | 
							   ((((findLabelPos(temp) - pos) >> 11) & 0x1  ) << 20) | 
							   ((((findLabelPos(temp) - pos) >> 12) & 0xff ) << 12);
			}
		break;
		case 'B':
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 15);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			instruction |= getRegister(temp.substr(0, (temp.size() - 1)), 20);
			
			ss_input >> temp;
			if (!(ss_input && (temp.at(0) != '#'))) {
				cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
				abort();
			}
			if ((temp.size() >= 2) && (temp.at(0) == '0') && (temp.at(1) == 'x')) {
				instruction |= (((stoi(temp, nullptr, 16) >> 11) & 0x1 ) << 7 ) | 
							   (((stoi(temp, nullptr, 16) >> 1 ) & 0xf ) << 8 ) | 
							   (((stoi(temp, nullptr, 16) >> 5 ) & 0x3f) << 25) | 
							   (((stoi(temp, nullptr, 16) >> 12) & 0x1 ) << 31);
			} else if ((input.at(0) <= '9') && (input.at(0) >= '0')) {
				instruction |= (((stoi(temp, nullptr) >> 11) & 0x1 ) << 7 ) | 
							   (((stoi(temp, nullptr) >> 1 ) & 0xf ) << 8 ) | 
							   (((stoi(temp, nullptr) >> 5 ) & 0x3f) << 25) | 
							   (((stoi(temp, nullptr) >> 12) & 0x1 ) << 31);
			} else {
				instruction |= ((((findLabelPos(temp) - pos) >> 11) & 0x1 ) << 7 ) | 
							   ((((findLabelPos(temp) - pos) >> 1 ) & 0xf ) << 8 ) | 
							   ((((findLabelPos(temp) - pos) >> 5 ) & 0x3f) << 25) | 
							   ((((findLabelPos(temp) - pos) >> 12) & 0x1 ) << 31);
			}
		break;
		default:
			cerr << "ERROR: unknown type \'" << instruction_type << "\'\n";
			abort();
	}
	
	ss_input >> temp;
	if (ss_input && (temp.at(0) != '#')) {
		cerr << "ERROR: incorrect args at line \"" << pos << "\"\n";
		abort();
	}
	
	return instruction;
}

/**
 * \brief \c process() assembles the machine code line by line and exports to a file in hex NOT Executable. 
 * 
 * \details This function will error out if there are any issues.
 * \note If you would like a binary executable, edit the fprintf statement.
 */
void risc_v_assembler::process() {
	fstream fin(input_file, fstream::in);
	
	if (!fin.is_open()) {
		cerr << "ERROR: invalid input file.\n";
		abort();
	}
	
	FILE * fout;
	fout = fopen(output_file, "w");
	
	if (fout == nullptr) {
		cerr << "ERROR: invalid output file.\n";
		abort();
	}
	
	uint32_t instruction;
	
	string input;
	getline(fin, input);
	stringstream ss_input(input);
	string temp;
	
	for (int i = 1; fin && ss_input; i++) {
		ss_input >> temp;
		
		if ((temp.size() != 0) && (temp.at(temp.size() - 1) == ':')) {
			makeLabel(temp.substr(0, (temp.size() - 1)), i);
			ss_input >> temp;
			if (!((temp.size() != 0) && ss_input) || ((temp.size() != 0) && (temp.at(0) == '#'))) {
				i -= 1;
			}
		}
		
		getline(fin, input);
		ss_input.str(input);
	}
	
	fin.clear();
	fin.seekg(0, fin.beg);
	
	getline(fin, input);
	ss_input.str(input);
	
	for (int i = 1; fin; i++) {
		instruction = processLine(input, i);
		
		if (instruction == 0) {
			i -= 1;
		} else {
			fprintf(fout, "%.8X\n", instruction);
		}
		getline(fin, input);
	}
	fin.close();
	fclose(fout);
}

/**
 * \brief \c getInputFile() returns the input file name. 
 * 
 * \returns \c input_file
 */
char * risc_v_assembler::getInputFile() {
	return input_file;
}

/**
 * \brief \c getOutputFile() returns the output file name. 
 * 
 * \returns \c output_file
 */
char * risc_v_assembler::getOutputFile() {
	return output_file;
}

/**
 * \brief \c setInputFile() sets the input file name. 
 * 
 * \param [in] input_file_name sets input_file.
 */
void risc_v_assembler::setInputFile(char * input_file_name) {
	input_file = input_file_name;
}

/**
 * \brief \c setOutputFile() sets the output file name. 
 * 
 * \param [in] output_file_name sets output_file.
 */
void risc_v_assembler::setOutputFile(char * output_file_name) {
	output_file = output_file_name;
}


int main(int argc, char * argv[]) {
	risc_v_assembler r1(argv[1], argv[2]);
	r1.process();
	
	return 0;
}