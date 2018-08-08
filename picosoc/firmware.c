#include <stdint.h>
#include <stdbool.h>

#define TF 1
#if TF
#include "tensorflow/contrib/lite/context.h"
#include "tensorflow/contrib/lite/interpreter.h"
#endif

// a pointer to this is a null pointer, but the compiler does not
// know that because "sram" is a linker symbol from sections.lds.
extern uint32_t sram;

#define reg_spictrl (*(volatile uint32_t*)0x02000000)
#define reg_uart_clkdiv (*(volatile uint32_t*)0x02000004)
#define reg_uart_data (*(volatile uint32_t*)0x02000008)
#define reg_leds (*(volatile uint32_t*)0x03000000)

// --------------------------------------------------------

int* __errno() {return 0;}

// --------------------------------------------------------

using namespace tflite;

void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	reg_uart_data = c;
}

void print(const char *p)
{
	while (*p)
		putchar(*(p++));
}

void print_hex(uint32_t v, int digits)
{
	for (int i = 7; i >= 0; i--) {
		char c = "0123456789abcdef"[(v >> (4*i)) & 15];
		if (c == '0' && i >= digits) continue;
		putchar(c);
		digits = i;
	}
}

void print_dec(int32_t v)
{
  if (v<0) {
    print("-");
    v = -v;
  }

	if (v >= 1000) {
		print(">=1000");
		return;
	}

	if      (v >= 900) { putchar('9'); v -= 900; }
	else if (v >= 800) { putchar('8'); v -= 800; }
	else if (v >= 700) { putchar('7'); v -= 700; }
	else if (v >= 600) { putchar('6'); v -= 600; }
	else if (v >= 500) { putchar('5'); v -= 500; }
	else if (v >= 400) { putchar('4'); v -= 400; }
	else if (v >= 300) { putchar('3'); v -= 300; }
	else if (v >= 200) { putchar('2'); v -= 200; }
	else if (v >= 100) { putchar('1'); v -= 100; }

        if      (v >= 90) { putchar('9'); v -= 90; }
	else if (v >= 80) { putchar('8'); v -= 80; }
	else if (v >= 70) { putchar('7'); v -= 70; }
	else if (v >= 60) { putchar('6'); v -= 60; }
	else if (v >= 50) { putchar('5'); v -= 50; }
	else if (v >= 40) { putchar('4'); v -= 40; }
	else if (v >= 30) { putchar('3'); v -= 30; }
	else if (v >= 20) { putchar('2'); v -= 20; }
	else if (v >= 10) { putchar('1'); v -= 10; }

	if      (v >= 9) { putchar('9'); v -= 9; }
	else if (v >= 8) { putchar('8'); v -= 8; }
	else if (v >= 7) { putchar('7'); v -= 7; }
	else if (v >= 6) { putchar('6'); v -= 6; }
	else if (v >= 5) { putchar('5'); v -= 5; }
	else if (v >= 4) { putchar('4'); v -= 4; }
	else if (v >= 3) { putchar('3'); v -= 3; }
	else if (v >= 2) { putchar('2'); v -= 2; }
	else if (v >= 1) { putchar('1'); v -= 1; }
	else putchar('0');
}

char getchar_prompt(char *prompt)
{
	int32_t c = -1;

	uint32_t cycles_begin, cycles_now, cycles;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));

	if (prompt)
		print(prompt);

	reg_leds = ~0;
	while (c == -1) {
		__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
		cycles = cycles_now - cycles_begin;
		if (cycles > 12000000) {
			if (prompt)
				print(prompt);
			cycles_begin = cycles_now;
			reg_leds = ~reg_leds;
		}
		c = reg_uart_data;
	}
	reg_leds = 0;
	return c;
}

char getchar2()
{
	return getchar_prompt(0);
}


#if TF

static char error_buf[128];
extern char *heap_ptr;

struct Reporter : public tflite::ErrorReporter {
 public:
  virtual ~Reporter(){}

   int Report(const char* format, va_list args) override {
     int len = 0;
     print("ERROR: ");
     print(format);
     //len = vsprintf(error_buf, format, args);
     //print(error_buf);
     print("\n");
     return len;
  }

};

void tf() {

  print("1\n");
  Reporter* pReporter = new Reporter();
  print("1a\n");

  tflite::Interpreter interpreter(pReporter);

  print("1b\n");
  int base = 0;
  interpreter.AddTensors(4, &base);
  print("base: ");print_dec(base);print("\n");
  interpreter.AddTensors(1, &base);
  print("base: ");print_dec(base);print("\n");

  print("2\n");
  TfLiteRegistration reg_add = {nullptr, nullptr, nullptr, nullptr};
  reg_add.prepare = [](TfLiteContext* context, TfLiteNode* node) {
      TfLiteTensor* tensorIn0 = &context->tensors[node->inputs->data[0]];
      // TODO(aselle): Check if tensorIn1 is the same size as tensorOut
      // and that tensorIn0 and tensorIn1 and tensorOut are all float32 type.
      TfLiteTensor* tensorOut = &context->tensors[node->outputs->data[0]];
      print("PicoSoC TFLite Demo1!\n");
      TfLiteIntArray* newSize = TfLiteIntArrayCopy(tensorIn0->dims);
      TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, tensorOut, newSize));
      print("PicoSoC TFLite Demo2!\n");
      return kTfLiteOk;
    };
    reg_add.invoke = [](TfLiteContext* context, TfLiteNode* node) {
                       print("PicoSoC TFLite Demo!!\n");
      TfLiteTensor* a0 = &context->tensors[node->inputs->data[0]];
      TfLiteTensor* a1 = &context->tensors[node->inputs->data[1]];
      TfLiteTensor* a2 = &context->tensors[node->outputs->data[0]];
      int count = a0->bytes / sizeof(float);
      float* a = a0->data.f;
      float* b = a1->data.f;
      float* c = a2->data.f;
      float* c_end = c + count;
      for(; c != c_end; c++, a++, b++)
        *c = *a + *b;
      return kTfLiteOk;
    };
  print("heap: ");print_hex(heap_ptr, 8);print("\n");

  interpreter.AddNodeWithParameters({0, 1}, {2}, nullptr, 0, nullptr,
                                      &reg_add);
  print("heap: ");print_hex(heap_ptr, 8);print("\n");
  print("2a\n");
  interpreter.AddNodeWithParameters({2, 2}, {3}, nullptr, 0, nullptr,
                                     &reg_add);

  print("2b\n");
  TfLiteQuantizationParams quantized;
  interpreter.SetTensorParametersReadWrite(0, kTfLiteFloat32, "", {3},
                                           quantized);
  interpreter.SetTensorParametersReadWrite(1, kTfLiteFloat32, "", {3},
                                           quantized);
  interpreter.SetTensorParametersReadWrite(2, kTfLiteFloat32, "", {3},
                                           quantized);
  interpreter.SetTensorParametersReadWrite(3, kTfLiteFloat32, "", {3},
                                           quantized);
  print("3\n");
  interpreter.SetInputs({0, 1});
  interpreter.SetOutputs({3});
  print("4\n");

  TfLiteStatus allocateStatus = interpreter.AllocateTensors();
  print("4a\n");
  auto tt0 = interpreter.tensor(0);
  print("tt0: ");print_hex(tt0, 8);print("\n");

  float* aIn = interpreter.typed_tensor<float>(0);
  float* bIn = interpreter.typed_tensor<float>(1);

  aIn[0] = 1.f;
  aIn[1] = 2.f;
  aIn[2] = 3.f;

  bIn[0] = -4.f;
  bIn[1] = -2.f;
  bIn[2] = 11.f;

  print("aIn: ");
  print_hex(aIn, 8);
  print("\n");
  for (int ii=0; ii<3; ii++) {
    print_dec(aIn[ii]);
    print("\n");
  }
  print("bIn: ");
  print_hex(bIn, 8);
  print("\n");
  for (int ii=0; ii<3; ii++) {
    print_dec(bIn[ii]);
     print("\n");
  }


  // printf("tflite input: 2*([%2.2f %2.2f %2.2f] + [%2.2f %2.2f %2.2f])\n",
  //        aIn[0], aIn[1], aIn[2],
  //        bIn[0], bIn[1], bIn[2]
    //       );
  print("5\n");
  interpreter.Invoke();
  print("6\n");
  float* cOut = interpreter.typed_tensor<float>(3);

  print("cOut: ");
  print_hex(cOut, 8);
  print("\n");
  //    printf( "tflite output: %f %f %f\n", cOut[0], cOut[1], cOut[2]);
  for (int ii=0; ii<3; ii++) {
    print_dec(cOut[ii]);
    print("\n");
  }

  print("tf\n");
  print("heap: ");print_hex(heap_ptr, 8);print("\n");

}
#endif

// --------------------------------------------------------

// --------------------------------------------------------

// --------------------------------------------------------


void main()
{
	reg_uart_clkdiv = 104;

#if TF
        tf();
#endif

	while (getchar_prompt("Press ENTER to continue..\n") != '\r') { /* wait */ }

	print("\n");
	print("  ____  _          ____         ____\n");
	print(" |  _ \\(_) ___ ___/ ___|  ___  / ___|\n");
	print(" | |_) | |/ __/ _ \\___ \\ / _ \\| |\n");
	print(" |  __/| | (_| (_) |__) | (_) | |___\n");
	print(" |_|   |_|\\___\\___/____/ \\___/ \\____|\n");


	while (1)
	{
		print("\n");
		print("\n");
		print("SPI State:\n");

		print("  LATENCY ");
		print_dec((reg_spictrl >> 16) & 15);
		print("\n");

		print("  DDR ");
		if ((reg_spictrl & (1 << 22)) != 0)
			print("ON\n");
		else
			print("OFF\n");

		print("  QSPI ");
		if ((reg_spictrl & (1 << 21)) != 0)
			print("ON\n");
		else
			print("OFF\n");

		print("  CRM ");
		if ((reg_spictrl & (1 << 20)) != 0)
			print("ON\n");
		else
			print("OFF\n");

		print("\n");
		print("Select an action:\n");
		print("\n");
		print("   [1] Read SPI Flash ID\n");
		print("   [2] Read SPI Config Regs\n");
		print("   [3] Switch to default mode\n");
		print("   [4] Switch to Dual I/O mode\n");
		print("   [5] Switch to Quad I/O mode\n");
		print("   [6] Switch to Quad DDR mode\n");
		print("   [7] Toggle continuous read mode\n");
		print("   [9] Run simplistic benchmark\n");
		print("   [0] Benchmark all configs\n");
		print("\n");

		for (int rep = 10; rep > 0; rep--)
		{
			print("Command> ");
			char cmd = getchar2();
			if (cmd > 32 && cmd < 127)
				putchar(cmd);
			print("\n");

			switch (cmd)
			{
			case '1':
				break;
			case '2':
				break;
			case '3':
				reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00000000;
				break;
			case '4':
				reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00400000;
				break;
			case '5':
				reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00200000;
				break;
			case '6':
				reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00600000;
				break;
			case '7':
				reg_spictrl = reg_spictrl ^ 0x00100000;
				break;
			case '9':
				break;
			case '0':
				break;
			default:
				continue;
			}

			break;
		}
	}
}
