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

char *ppp1 = "hello world";
char ppp0[] = "hello world";
extern void *_end; /* any type would do, only its address is important */
extern void *_data;
extern void *_edata;
extern void *flashio_worker_begin;

// --------------------------------------------------------

using namespace tflite;

#if 0
#include <stdio.h>
#include <malloc.h>

static void *(*old_malloc_hook)(size_t, const void *);
static void* moffset = 0x20;

static void *my_malloc_hook(size_t size, const void *caller)
{
  void *result;

  result = moffset;
  moffset += size;

  return result;
}

static void my_init_hook(void)
{
  old_malloc_hook = __malloc_hook;
  __malloc_hook = my_malloc_hook;
}

void (*__malloc_initialize_hook)(void) = my_init_hook;
#endif

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

void print_dec(uint32_t v)
{
	if (v >= 100) {
		print(">=100");
		return;
	}

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
/*
static char error_buf[1024];

class Reporter:public tflite::ErrorReporter {
 public:
  virtual ~Reporter(){}
  int Report(const char* format, va_list args) {
    vprintf(format, args);
    int len = vsprintf(error_buf, format, args);
    ptr += len;
  }

  char* ptr = error_buf;
};

Reporter reporter;
*/

struct Reporter:public tflite::ErrorReporter {
 public:
  virtual ~Reporter(){}

   int Report(const char* format, va_list args) override {
    //int len = vsprintf(error_buf, format, args);
    //ptr += len;
    return 0;
  }

  //char* ptr = error_buf;
};

Reporter reporter;


void tf() {
    // TfLiteRegistration reg_add = {nullptr, nullptr, nullptr, nullptr};
    // reg_add.prepare = [](TfLiteContext* context, TfLiteNode* node) {
    //   TfLiteTensor* tensorIn0 = &context->tensors[node->inputs->data[0]];
    //   // TODO(aselle): Check if tensorIn1 is the same size as tensorOut
    //   // and that tensorIn0 and tensorIn1 and tensorOut are all float32 type.
    //   TfLiteTensor* tensorOut = &context->tensors[node->outputs->data[0]];
    //   TfLiteIntArray* newSize = TfLiteIntArrayCopy(tensorIn0->dims);
    //   TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, tensorOut, newSize));
    //   return kTfLiteOk;
    // };
    // reg_add.invoke = [](TfLiteContext* context, TfLiteNode* node) {
    //   TfLiteTensor* a0 = &context->tensors[node->inputs->data[0]];
    //   TfLiteTensor* a1 = &context->tensors[node->inputs->data[1]];
    //   TfLiteTensor* a2 = &context->tensors[node->outputs->data[0]];
    //   int count = a0->bytes / sizeof(float);
    //   float* a = a0->data.f;
    //   float* b = a1->data.f;
    //   float* c = a2->data.f;
    //   float* c_end = c + count;
    //   for(; c != c_end; c++, a++, b++)
    //     *c = *a + *b;
    //   return kTfLiteOk;
    // };
  void* tm;
  putchar('.');
  putchar('\n');
  //tm =  malloc(1);



  char* blah = "blah blah";
  print_hex(0x13, 8); print("\n");
  print_hex(blah, 8); print("\n");

  print_hex(&errno, 8); print("\n");
  print("_end: ");
  print_hex(&_end, 8); print("\n");
  print_hex(&flashio_worker_begin, 8); print("\n");
  print("ppp\n");
  print_hex(ppp0, 8); print("\n");
  print_hex(ppp1, 8); print("\n");
  print_hex(&ppp0, 8); print("\n");
  print_hex(&ppp1, 8); print("\n");
  print("done\n");

  extern uint32_t __malloc_av_[];
  print("av: ");print_hex(__malloc_av_, 8); print("\n");
  print("&av: ");print_hex(&__malloc_av_, 8); print("\n");
  for (int ii=0; ii <128*2; ii++) {
    print_hex(&__malloc_av_[ii], 8); print("> "); print_hex(__malloc_av_[ii], 8); print("\n");
  }

  for (int ii=0; ii <20; ii++) {
    tm =  malloc(1);
    print_dec(ii); print(": ");print_hex(tm, 8); print("\n");
  }



  print_hex(0x12, 8);
  print("vector\n");
  std::vector<std::string> tt;
  print_hex(tt.data(), 8);
  print("add one\n");
  tt.push_back("one");
  print_hex(tt.data(), 8);
  print("add two\n");
  tt.push_back("two");
  print_hex(tt.data(), 8);
  print("add five\n");
  tt.push_back("FIVE!");
 //  print("add threesir\n");
 //  tt.push_back("three sir");
 //  print("add three\n");
 // tt.push_back("THREE!");

  print("loop\n");
  for (auto ii=tt.begin(); ii!=tt.end(); ++ii) {
    print(ii->c_str());
    print("\n");
  }

  print("1\n");
  //auto reporter = tflite::DefaultErrorReporter();
   print("1a\n");
   //std::unique_ptr<Interpreter> interpreter;

   tflite::Interpreter interpreter(&reporter);

   print("1b\n");
   int base;
   interpreter.AddTensors(4, &base);

   print("2\n");
   // interpreter.AddNodeWithParameters({0, 1}, {2}, nullptr, 0, nullptr,
   //                                             &reg_add);
   // interpreter.AddNodeWithParameters({2, 2}, {3}, nullptr, 0, nullptr,
   //                                             &reg_add);
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
   float* aIn = interpreter.typed_tensor<float>(0);
   float* bIn = interpreter.typed_tensor<float>(1);

   aIn[0] = 1.f;
   aIn[1] = 2.f;
   aIn[2] = 3.f;

   bIn[0] = -3.f;
   bIn[1] = -2.f;
   bIn[2] = 11.f;


    // printf("tflite input: 2*([%2.2f %2.2f %2.2f] + [%2.2f %2.2f %2.2f])\n",
    //        aIn[0], aIn[1], aIn[2],
    //        bIn[0], bIn[1], bIn[2]
    //       );

    // interpreter.Invoke();
    // float* cOut = interpreter.typed_tensor<float>(3);

//    printf( "tflite output: %f %f %f\n", cOut[0], cOut[1], cOut[2]);

    print("tf\n");
}
#endif

// --------------------------------------------------------

// --------------------------------------------------------

// --------------------------------------------------------


void main()
{
	reg_uart_clkdiv = 104;

        char *src = 0x180000;
        char *dst = (char*)&_data;

        print_hex(dst, 8); print("\n");
        print_hex(src, 8); print("\n");
        print("&_data: ");
        print_hex(&_data, 8); print("\n");
        print("&_end: ");
        print_hex(&_end, 8); print("\n");
        print("&_edata: ");
        print_hex(&_edata, 8); print("\n");

        /* ROM has data at end of text; copy it. */
        while (dst < &_edata) {
          *dst++ = *src++;
        }
        print_hex(dst, 8); print("\n");
        print_hex(src, 8); print("\n");

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
