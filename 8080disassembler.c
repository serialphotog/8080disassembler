// Copyright 2018-2026, Adam Thompson <adam@hackeradam.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial 
// portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Forward declaration for the disassemble routine.
typedef struct
{
	const char *format;
	uint8_t length;
} Instruction;

static const Instruction instructions[256] = {
	[0x00] = { "NOP", 1 },
	[0x01] = { "LXI\tB, #$0x%02x%02x", 3 },
	[0x02] = { "STAX\tB", 1 },
	[0x03] = { "INX\tB", 1 },
	[0x04] = { "INR\tB", 1 },
	[0x05] = { "DCR\tB", 1 },
	[0x06] = { "MVI\tB, #$0x%02x", 2 },
	[0x07] = { "RLC", 1 },
	[0x08] = { "NOP", 1 },
	[0x09] = { "DAD\tB", 1 },
	[0x0a] = { "LDAX\tB", 1 },
	[0x0b] = { "DCX\tB", 1 },
	[0x0c] = { "INR\tC", 1 },
	[0x0d] = { "DCR\tC", 1 },
	[0x0e] = { "MVI\tC, #$0x%02x", 2 },
	[0x0f] = { "RRC", 1 },
	[0x10] = { "NOP", 1 },
	[0x11] = { "LXI\tD, #$0x%02x%02x", 3 },
	[0x12] = { "STAX\tD", 1 },
	[0x13] = { "INX\tD", 1 },
	[0x14] = { "INR\tD", 1 },
	[0x15] = { "DCR\tD", 1 },
	[0x16] = { "MVI\tD, #$0x%02x", 2 },
	[0x17] = { "RAL", 1 },
	[0x18] = { "NOP", 1 },
	[0x19] = { "DAD\tD", 1 },
	[0x1a] = { "LDAX\tD", 1 },
	[0x1b] = { "DCX\tD", 1 },
	[0x1c] = { "INR\tE", 1 },
	[0x1d] = { "DCR\tE", 1 },
	[0x1e] = { "MVI\tE, #$0x%02x", 2 },
	[0x1f] = { "RAR", 1 },
	[0x20] = { "NOP", 1 },
	[0x21] = { "LXI\tH, #$0x%02x%02x", 3 },
	[0x22] = { "SHLD\t0x%02x%02x", 3 },
	[0x23] = { "INX\tH", 1 },
	[0x24] = { "INR\tH", 1 },
	[0x25] = { "DCR\tH", 1 },
	[0x26] = { "MVI\tH, #$0x%02x", 2 },
	[0x27] = { "DAA", 1 },
	[0x28] = { "NOP", 1 },
	[0x29] = { "DAD\tH", 1 },
	[0x2a] = { "LHLD\t 0x%02x%02x", 3 },
	[0x2b] = { "DCX\tH", 1 },
	[0x2c] = { "INR\tL", 1 },
	[0x2d] = { "DCR\tL", 1 },
	[0x2e] = { "MVI\tL, #$0x%02x", 2 },
	[0x2f] = { "CMA", 1 },
	[0x30] = { "NOP", 1 },
	[0x31] = { "LXI\tSP, #$0x%02x%02x", 3 },
	[0x32] = { "STA\t0x%02x%02x", 3 },
	[0x33] = { "INX\tSP", 1 },
	[0x34] = { "INR\tM", 1 },
	[0x35] = { "DCR\tM", 1 },
	[0x36] = { "MVI\tM, #$0x%02x", 2 },
	[0x37] = { "STC", 1 },
	[0x38] = { "NOP", 1 },
	[0x39] = { "DAD\tSP", 1 },
	[0x3a] = { "LDA\t0x%02x%02X", 3 },
	[0x3b] = { "DCX\tSP", 1 },
	[0x3c] = { "INR\tA", 1 },
	[0x3d] = { "DCR\tA", 1 },
	[0x3e] = { "MVI\tA, #$0x%02x", 2 },
	[0x3f] = { "CMC", 1 },
	[0x40] = { "MOV\tB, B", 1 },
	[0x41] = { "MOV\tB, C", 1 },
	[0x42] = { "MOV\tB, D", 1 },
	[0x43] = { "MOV\tB, E", 1 },
	[0x44] = { "MOV\tB, H", 1 },
	[0x45] = { "MOV\tB, L", 1 },
	[0x46] = { "MOV\tB, M", 1 },
	[0x47] = { "MOV\tB, A", 1 },
	[0x48] = { "MOV\tC, B", 1 },
	[0x49] = { "MOV\tC, C", 1 },
	[0x4a] = { "MOV\tC, D", 1 },
	[0x4b] = { "MOV\tC, E", 1 },
	[0x4c] = { "MOV\tC, H", 1 },
	[0x4d] = { "MOV\tC, L", 1 },
	[0x4e] = { "MOV\tC, M", 1 },
	[0x4f] = { "MOV\tC, A", 1 },
	[0x50] = { "MOV\tD, B", 1 },
	[0x51] = { "MOV\tD, C", 1 },
	[0x52] = { "MOV\tD, D", 1 },
	[0x53] = { "MOV\tD, E", 1 },
	[0x54] = { "MOV\tD, H", 1 },
	[0x55] = { "MOV\tD, L", 1 },
	[0x56] = { "MOV\tD, M", 1 },
	[0x57] = { "MOV\tD, A", 1 },
	[0x58] = { "MOV\tE, B", 1 },
	[0x59] = { "MOV\tE, C", 1 },
	[0x5a] = { "MOV\tE, D", 1 },
	[0x5b] = { "MOV\tE, E", 1 },
	[0x5c] = { "MOV\tE, H", 1 },
	[0x5d] = { "MOV\tE, L", 1 },
	[0x5e] = { "MOV\tE, M", 1 },
	[0x5f] = { "MOV\tE, A", 1 },
	[0x60] = { "MOV\tH, B", 1 },
	[0x61] = { "MOV\tH, C", 1 },
	[0x62] = { "MOV\tH, D", 1 },
	[0x63] = { "MOV\tH, E", 1 },
	[0x64] = { "MOV\tH, H", 1 },
	[0x65] = { "MOV\tH, L", 1 },
	[0x66] = { "MOV\tH, M", 1 },
	[0x67] = { "MOV\tH, A", 1 },
	[0x68] = { "MOV\tL, B", 1 },
	[0x69] = { "MOV\tL, C", 1 },
	[0x6a] = { "MOV\tL, D", 1 },
	[0x6b] = { "MOV\tL, E", 1 },
	[0x6c] = { "MOV\tL, H", 1 },
	[0x6d] = { "MOV\tL, L", 1 },
	[0x6e] = { "MOV\tL, M", 1 },
	[0x6f] = { "MOV\tL, A", 1 },
	[0x70] = { "MOV\tM, B", 1 },
	[0x71] = { "MOV\tM, C", 1 },
	[0x72] = { "MOV\tM, D", 1 },
	[0x73] = { "MOV\tM, E", 1 },
	[0x74] = { "MOV\tM, H", 1 },
	[0x75] = { "MOV\tM, L", 1 },
	[0x76] = { "HLT", 1 },
	[0x77] = { "MOV\tM, A", 1 },
	[0x78] = { "MOV\tA, B", 1 },
	[0x79] = { "MOV\tA, C", 1 },
	[0x7a] = { "MOV\tA, D", 1 },
	[0x7b] = { "MOV\tA, E", 1 },
	[0x7c] = { "MOV\tA, H", 1 },
	[0x7d] = { "MOV\tA, L", 1 },
	[0x7e] = { "MOV\tA, M", 1 },
	[0x7f] = { "MOV\tA, A", 1 },
	[0x80] = { "ADD\tB", 1 },
	[0x81] = { "ADD\tC", 1 },
	[0x82] = { "ADD\tD", 1 },
	[0x83] = { "ADD\tE", 1 },
	[0x84] = { "ADD\tH", 1 },
	[0x85] = { "ADD\tL", 1 },
	[0x86] = { "ADD\tM", 1 },
	[0x87] = { "ADD\tA", 1 },
	[0x88] = { "ADC\tB", 1 },
	[0x89] = { "ADC\tC", 1 },
	[0x8a] = { "ADC\tD", 1 },
	[0x8b] = { "ADC\tE", 1 },
	[0x8c] = { "ADC\tH", 1 },
	[0x8d] = { "ADC\tL", 1 },
	[0x8e] = { "ADC\tM", 1 },
	[0x8f] = { "ADC\tA", 1 },
	[0x90] = { "SUB\tB", 1 },
	[0x91] = { "SUB\tC", 1 },
	[0x92] = { "SUB\tD", 1 },
	[0x93] = { "SUB\tE", 1 },
	[0x94] = { "SUB\tH", 1 },
	[0x95] = { "SUB\tL", 1 },
	[0x96] = { "SUB\tM", 1 },
	[0x97] = { "SUB\tA", 1 },
	[0x98] = { "SBB\tB", 1 },
	[0x99] = { "SBB\tC", 1 },
	[0x9a] = { "SBB\tD", 1 },
	[0x9b] = { "SBB\tE", 1 },
	[0x9c] = { "SBB\tH", 1 },
	[0x9d] = { "SBB\tL", 1 },
	[0x9e] = { "SBB\tM", 1 },
	[0x9f] = { "SBB\tA", 1 },
	[0xa0] = { "ANA\tB", 1 },
	[0xa1] = { "ANA\tC", 1 },
	[0xa2] = { "ANA\tD", 1 },
	[0xa3] = { "ANA\tE", 1 },
	[0xa4] = { "ANA\tH", 1 },
	[0xa5] = { "ANA\tL", 1 },
	[0xa6] = { "ANA\tM", 1 },
	[0xa7] = { "ANA\tA", 1 },
	[0xa8] = { "XRA\tB", 1 },
	[0xa9] = { "XRA\tC", 1 },
	[0xaa] = { "XRA\tD", 1 },
	[0xab] = { "XRA\tE", 1 },
	[0xac] = { "XRA\tH", 1 },
	[0xad] = { "XRA\tL", 1 },
	[0xae] = { "XRA\tM", 1 },
	[0xaf] = { "XRA\tA", 1 },
	[0xb0] = { "ORA\tB", 1 },
	[0xb1] = { "ORA\tC", 1 },
	[0xb2] = { "ORA\tD", 1 },
	[0xb3] = { "ORA\tE", 1 },
	[0xb4] = { "ORA\tH", 1 },
	[0xb5] = { "ORA\tL", 1 },
	[0xb6] = { "ORA\tM", 1 },
	[0xb7] = { "ORA\tA", 1 },
	[0xb8] = { "CMP\tB", 1 },
	[0xb9] = { "CMP\tC", 1 },
	[0xba] = { "CMP\tD", 1 },
	[0xbb] = { "CMP\tE", 1 },
	[0xbc] = { "CMP\tH", 1 },
	[0xbd] = { "CMP\tL", 1 },
	[0xbe] = { "CMP\tM", 1 },
	[0xbf] = { "CMP\tA", 1 },
	[0xc0] = { "RNZ", 1 },
	[0xc1] = { "POP\tB", 1 },
	[0xc2] = { "JNZ\t0x%02x%02X", 3 },
	[0xc3] = { "JMP\t0x%02x%02x", 3 },
	[0xc4] = { "CNZ\t0x%02x%02x", 3 },
	[0xc5] = { "PUSH\tB", 1 },
	[0xc6] = { "ADI\t#$0x%02x", 2 },
	[0xc7] = { "RST\t0", 1 },
	[0xc8] = { "RZ", 1 },
	[0xc9] = { "RET", 1 },
	[0xca] = { "JZ\t0x%02x%02x", 3 },
	[0xcb] = { "NOP", 1 },
	[0xcc] = { "CZ\t0x%02x%02x", 3 },
	[0xcd] = { "CALL\t0x%02x%02x", 3 },
	[0xce] = { "ACI\t#$0x%02x", 2 },
	[0xcf] = { "RST\t1", 1 },
	[0xd0] = { "RNC", 1 },
	[0xd1] = { "POP\tD", 1 },
	[0xd2] = { "JNC\t0x%02x%02x", 3 },
	[0xd3] = { "OUT\t#$0x%02x", 2 },
	[0xd4] = { "CNC\t0x%02x%02x", 3 },
	[0xd5] = { "PUSH\tD", 1 },
	[0xd6] = { "SUI\t#$0x%02x", 2 },
	[0xd7] = { "RST\t2", 1 },
	[0xd8] = { "RC", 1 },
	[0xd9] = { "NOP", 1 },
	[0xda] = { "JC\t0x%02x%02x", 3 },
	[0xdb] = { "IN\t#$0x%02x", 2 },
	[0xdc] = { "CC\t0x%02x%02x", 3 },
	[0xdd] = { "NOP", 1 },
	[0xde] = { "SBI\t#$0x%02x", 2 },
	[0xdf] = { "RST\t3", 1 },
	[0xe0] = { "RPO", 1 },
	[0xe1] = { "POP\tH", 1 },
	[0xe2] = { "JPO\t0x%02x%02x", 3 },
	[0xe3] = { "XTHL", 1 },
	[0xe4] = { "CPO\t0x%02x%02x", 3 },
	[0xe5] = { "PUSH\tH", 1 },
	[0xe6] = { "ANI\t#$0x%02x", 2 },
	[0xe7] = { "RST\t4", 1 },
	[0xe8] = { "RPE", 1 },
	[0xe9] = { "PCHL", 1 },
	[0xea] = { "JPE\t0x%02x%02x", 3 },
	[0xeb] = { "XCHG", 1 },
	[0xec] = { "CPE\t0x%02x%02x", 3 },
	[0xed] = { "NOP", 1 },
	[0xee] = { "XRI\t#$0x%02x", 2 },
	[0xef] = { "RST\t5", 1 },
	[0xf0] = { "RP", 1 },
	[0xf1] = { "POP\tPSW", 1 },
	[0xf2] = { "JP\t0x%02x%02x", 3 },
	[0xf3] = { "DI", 1 },
	[0xf4] = { "CP\t0x%02x%02x", 3 },
	[0xf5] = { "PUSH\tPSW", 1 },
	[0xf6] = { "ORI\t#$0x%02x", 2 },
	[0xf7] = { "RST\t6", 1 },
	[0xf8] = { "RM", 1 },
	[0xf9] = { "SPHL", 1 },
	[0xfa] = { "JM\t0x%02x%02x", 3 },
	[0xfb] = { "EI", 1 },
	[0xfc] = { "CM\t0x%02x%02x", 3 },
	[0xfd] = { "NOP", 1 },
	[0xfe] = { "CPI\t#$0x%02x", 2 },
	[0xff] = { "RST\t7", 1 },
};

static size_t disassemble(FILE *out, FILE *err, const uint8_t *buffer, size_t size, size_t pc);

// Entry point
// First argument is treated as the path to the file to disassemble
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s /path/to/binary.bin\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
	{
		fprintf(stderr, "Error: couldn't open %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	// Read the file into a memory buffer
	if (fseek(f, 0L, SEEK_END) != 0)
	{
		fprintf(stderr, "Error: couldn't seek in %s\n", argv[1]);
		fclose(f);
		return EXIT_FAILURE;
	}
	long file_length = ftell(f);
	if (file_length < 0 || fseek(f, 0L, SEEK_SET) != 0)
	{
		fprintf(stderr, "Error: couldn't determine the size of %s\n", argv[1]);
		fclose(f);
		return EXIT_FAILURE;
	}

	size_t fsize = (size_t)file_length;
	uint8_t *buffer = NULL;
	if (fsize > 0)
	{
		buffer = malloc(fsize);
		if (buffer == NULL)
		{
			fprintf(stderr, "Error: couldn't allocate memory for %s\n", argv[1]);
			fclose(f);
			return EXIT_FAILURE;
		}
		if (fread(buffer, 1, fsize, f) != fsize)
		{
			fprintf(stderr, "Error: couldn't read %s\n", argv[1]);
			free(buffer);
			fclose(f);
			return EXIT_FAILURE;
		}
	}
	if (fclose(f) != 0)
	{
		fprintf(stderr, "Error: couldn't close %s\n", argv[1]);
		free(buffer);
		return EXIT_FAILURE;
	}

	// Perform disassembly
	size_t pc = 0;
	while (pc < fsize)
	{
		size_t instruction_length = disassemble(stdout, stderr, buffer, fsize, pc);
		if (instruction_length == 0)
		{
			free(buffer);
			return EXIT_FAILURE;
		}
		pc += instruction_length;
	}

	free(buffer);

	return EXIT_SUCCESS;
}

// Performs the actual disassembly of an instruction
//
// Params:
//		FILE *out - The stream to receive disassembled instructions
//		FILE *err - The stream to receive decoding errors
//		const uint8_t *buffer - The buffer containing the machine code
//		size_t size - The size of the machine-code buffer
//		size_t pc - The program counter. Used to track our place in the buffer
//
// Returns:
//		size_t - The instruction length, or zero if the instruction is truncated or invalid
static size_t disassemble(FILE *out, FILE *err, const uint8_t *buffer, size_t size, size_t pc)
{
	const Instruction *instruction = &instructions[buffer[pc]];

	if (instruction->length > size - pc)
	{
		fprintf(err, "Error: truncated instruction at 0x%04zx (need %u bytes, have %zu)\n",
				pc, (unsigned int)instruction->length, size - pc);
		return 0;
	}

	fprintf(out, "0x%04zx  ", pc);
	switch (instruction->length)
	{
		case 1:
			fprintf(out, "%s", instruction->format);
			break;
		case 2:
			fprintf(out, instruction->format, (unsigned int)buffer[pc + 1]);
			break;
		case 3:
			fprintf(out, instruction->format, (unsigned int)buffer[pc + 2],
					(unsigned int)buffer[pc + 1]);
			break;
		default:
			fprintf(err, "Error: invalid instruction length for opcode 0x%02x\n",
					(unsigned int)buffer[pc]);
			return 0;
	}
	fprintf(out, "\n");

	return instruction->length;
}
