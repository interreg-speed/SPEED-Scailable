#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "app_main.h"

#include "esp_attr.h"

#include "ssd1306.h"

#include "m3/wasm3.h"
#include "m3/m3_env.h"

#include "m3_api_esp_wasi.h"
#include "../sclbl-wasi/sclbl.wasm.h"

#define GPIO_BACKLIGHT GPIO_NUM_4

#define FATAL(msg, ...){ printf("Fatal: " msg "\n", ##__VA_ARGS__);  return; }

// '3', 28x28px  --- 32 height, as multiple 8 
const unsigned char three [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0x60, 0x60, 0x60, 0x60, 0x60, 0xc0, 0xc0, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xff, 0xdf, 0xc0, 0x80, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x81, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0xc0, 0xe1, 0x7f, 0x1f, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define FLT_MIN -1.175494351e-38F

int max_float_in_str(char *buf) {
  char *err, *p = buf;
  float val;
  int index =0;
  int max_index = 0;
  float max_value = FLT_MIN;
  while (*p) {
    val = strtod(p, &err);
    if (p == err) p++;
    else if ((err == NULL) || (*err == 0)) { 
        if (val > max_value) { max_index = index;  max_value = val;}
        ++index; 
        break; 
    } else { 
        if (val > max_value) { max_index = index;  max_value = val;}
        ++index; 
        p = err + 1; 
    }
  }
  return max_index;
}

const char input_string[] = "{\"input\":\"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AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAsbAwPYWEhD6EgwM/j44OP4+ODj+Qjw8/j44OP4SDAz/FxMQ+ubg4PqGgoDwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\"}";

template <typename... Args>
void c_printf(const char *fmt, Args... args)
{
    char buffer[400];
    sprintf(buffer, fmt, args...);
    printf(buffer);
    //ssd1306_printFixedN (0, 96, "X", STYLE_NORMAL, FONT_SIZE_4X);
    ssd1306_print(buffer);
}

char* ltostr(long value) {
    char buffer[ 64 ] = { 0 };
    snprintf(buffer, sizeof(buffer), "%li", value);
    return strdup(buffer);
}

uint64_t m3_RunFunction(IM3Runtime runtime, const char *functionName, uint32_t i_argc, const char **i_argv) {
    IM3Function f;
    M3Result result = m3Err_none;
    result = m3_FindFunction(&f, runtime, functionName);
    if (result) { printf("Fatal: m3_FindFunction for %s\n", functionName, result);  exit(1); }
    result = m3_CallWithArgs(f, i_argc, i_argv);
    if (result) { printf("Fatal: m3_CallWithArgs for %s\n", functionName, result);  exit(1); }
    long value = *(uint64_t *)(runtime->stack);
    return value;
}

static void run_wasi(void)
{
    c_printf("\n   Keras->Onnx->Wasi->Wasm3\n\n");

    /////////////// initialization

    c_printf("  initialize neural network\n\n");

    IM3Function f;
    M3Result result = m3Err_none;

    const char *i_argv[1] = {NULL};

    uint8_t *wasm = (uint8_t *)sclbl_wasm;
    uint32_t fsize = sclbl_wasm_len - 1;

    printf("Loading WebAssembly...\n");
    IM3Environment env = m3_NewEnvironment();
    if (!env) FATAL("m3_NewEnvironment failed");

    IM3Runtime runtime = m3_NewRuntime(env, 20 * 1024, NULL); 
    if (!runtime) FATAL("m3_NewRuntime failed");

    IM3Module module;
    result = m3_ParseModule(env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule: %s", result);

    result = m3_LoadModule(runtime, module);
    if (result) FATAL("m3_LoadModule: %s", result);

    result = m3_LinkEspWASI(runtime->modules);
    if (result) FATAL("m3_LinkEspWASI: %s", result);

    result = m3_FindFunction(&f, runtime, "_start");
    if (result) FATAL("m3_FindFunction: %s", result);

    /////////////// WASI calls

    printf("Start of WASI call section\n");

    /////////////// get raw pointer to linear memory and some further info

    uint32_t memorySizeInBytes;
    const uint8_t *memory_pointer = m3_GetMemory(runtime, &memorySizeInBytes, 0);
    printf("Raw pointer to start of linear memory: %p\n", (void *)memory_pointer);
    printf("Linear memory size in bytes: %i\n", memorySizeInBytes);

    /////////////// Create input string, get length including zero terminator

    int i_inLength = strlen(input_string) + 1;
    char c_inStringLength[10];
    itoa(i_inLength, c_inStringLength, 10);
    printf("Input string length including zero terminator: %s\n", c_inStringLength);

    /////////////// Make WASM allocate area in linear memory for input string

    i_argv[0] = c_inStringLength;
    long value = m3_RunFunction(runtime, "malloc_buffer", 1, i_argv);
    printf("Integer pointer memory address of input string: %ld\n", value);

    /////////////// Copy input string to linear memory area assigned by WASM

    strcpy((char *)(memory_pointer + value), input_string);
    
    c_printf("        draw a number:\n\n\n\n\n");
    ssd1306_drawBitmap (100, 15, 28, 32, three);

    /////////////// Run Scailable's default "pred()" function

    m3_RunFunction(runtime, "pred", 0, NULL);
    
    c_printf("       ...computing...\n\n\n");

    /////////////// Free memory of input buffer

    i_argv[0] = ltostr(value);
    m3_RunFunction(runtime, "free_buffer", 1, i_argv);

    /////////////// Get output string location in linear memory and print results

    value = m3_RunFunction(runtime, "get_out_loc", 0, NULL);
    printf("Integer pointer memory address of output string: %ld\n", value);
    
    c_printf("%s\n\n", (char *)memory_pointer + value);
    c_printf("    LeNet on ESP32 says: %d\n\n", max_float_in_str((char *)memory_pointer + value));

    /////////////// Free memory of output buffer

    i_argv[0] = ltostr(value);
    m3_RunFunction(runtime, "free_buffer", 1, i_argv);
}

extern "C" void app_main(void)
{

    //default I2C_NUM_1 SDA --> 21 SCL --> 22
    //ssd1306_setFixedFont(ssd1306xled_font6x8);
    //ssd1306_128x64_i2c_init();
    //ssd1306_flipHorizontal(1);
    //ssd1306_flipVertical(1);
    //ssd1306_clearScreen();

    // TTGO T4 v1.3
    ssd1306_setFixedFont(ssd1306xled_font8x16);
    ili9341_240x320_spi_init(5, 27, 32); 
    gpio_pad_select_gpio(GPIO_NUM_4);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_4, 1);
    ili9341_setRotation(2);
    ssd1306_clearScreen();

    printf("\nWasm3 v" M3_VERSION " on ESP32, build " __DATE__ " " __TIME__ "\n");

    clock_t start = clock();
    run_wasi();
    clock_t end = clock();

    printf("Elapsed: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    vTaskDelay(30 / portTICK_PERIOD_MS);
}
