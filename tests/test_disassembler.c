#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main disassembler_main
#include "../8080disassembler.c"
#undef main

static unsigned int tests_run;
static unsigned int tests_failed;

#define CHECK(condition) do { \
	++tests_run; \
	if (!(condition)) { \
		fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
		++tests_failed; \
	} \
} while (0)

static size_t decode(const uint8_t *bytes, size_t size, char *output, size_t capacity,
		char *error, size_t error_capacity)
{
	FILE *out = tmpfile();
	FILE *err = tmpfile();
	if (out == NULL || err == NULL)
	{
		fprintf(stderr, "Unable to create temporary test files\n");
		exit(EXIT_FAILURE);
	}

	size_t length = disassemble(out, err, bytes, size, 0);
	rewind(out);
	rewind(err);

	size_t output_length = fread(output, 1, capacity - 1, out);
	size_t error_length = fread(error, 1, error_capacity - 1, err);
	output[output_length] = '\0';
	error[error_length] = '\0';
	fclose(out);
	fclose(err);
	return length;
}

static void expect_decode(const uint8_t *bytes, size_t size, size_t expected_length,
		const char *expected_output)
{
	char output[128];
	char error[128];
	size_t length = decode(bytes, size, output, sizeof(output), error, sizeof(error));

	CHECK(length == expected_length);
	CHECK(strcmp(output, expected_output) == 0);
	CHECK(error[0] == '\0');
}

static void test_table_is_complete(void)
{
	for (size_t opcode = 0; opcode < 256; ++opcode)
	{
		CHECK(instructions[opcode].format != NULL);
		CHECK(instructions[opcode].format[0] != '\0');
		CHECK(instructions[opcode].length >= 1 && instructions[opcode].length <= 3);
	}
}

static void test_representative_instructions(void)
{
	const uint8_t nop[] = { 0x00 };
	const uint8_t mov[] = { 0x7f };
	const uint8_t immediate[] = { 0x3e, 0x42 };
	const uint8_t address[] = { 0xc3, 0x34, 0x12 };

	expect_decode(nop, sizeof(nop), 1, "0x0000  NOP\n");
	expect_decode(mov, sizeof(mov), 1, "0x0000  MOV\tA, A\n");
	expect_decode(immediate, sizeof(immediate), 2, "0x0000  MVI\tA, #$0x42\n");
	expect_decode(address, sizeof(address), 3, "0x0000  JMP\t0x1234\n");
}

static void test_corrected_opcodes(void)
{
	const uint8_t mvi_d[] = { 0x16, 0x42 };
	const uint8_t adc_d[] = { 0x8a };
	const uint8_t ori[] = { 0xf6, 0x55 };
	const uint8_t undocumented_nop_20[] = { 0x20 };
	const uint8_t undocumented_nop_30[] = { 0x30 };

	expect_decode(mvi_d, sizeof(mvi_d), 2, "0x0000  MVI\tD, #$0x42\n");
	expect_decode(adc_d, sizeof(adc_d), 1, "0x0000  ADC\tD\n");
	expect_decode(ori, sizeof(ori), 2, "0x0000  ORI\t#$0x55\n");
	expect_decode(undocumented_nop_20, sizeof(undocumented_nop_20), 1, "0x0000  NOP\n");
	expect_decode(undocumented_nop_30, sizeof(undocumented_nop_30), 1, "0x0000  NOP\n");
}

static void test_truncated_instruction(void)
{
	const uint8_t bytes[] = { 0x21, 0x34 };
	char output[128];
	char error[128];
	size_t length = decode(bytes, sizeof(bytes), output, sizeof(output), error, sizeof(error));

	CHECK(length == 0);
	CHECK(output[0] == '\0');
	CHECK(strstr(error, "truncated instruction at 0x0000") != NULL);
	CHECK(strstr(error, "need 3 bytes, have 2") != NULL);
}

int main(void)
{
	test_table_is_complete();
	test_representative_instructions();
	test_corrected_opcodes();
	test_truncated_instruction();

	if (tests_failed != 0)
	{
		fprintf(stderr, "%u of %u checks failed\n", tests_failed, tests_run);
		return EXIT_FAILURE;
	}

	printf("All %u checks passed\n", tests_run);
	return EXIT_SUCCESS;
}
