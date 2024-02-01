/* ------------------------------------------------------------------ *\
                         Type definitions
\* ------------------------------------------------------------------ */
typedef unsigned int  UINT;   // processor-optimized.
typedef uint8_t       UINT8;
typedef uint16_t      UINT16;
typedef uint32_t      UINT32;
typedef uint64_t      UINT64;
typedef unsigned char UCHAR;


/* ------------------------------------------------------------------ *\
                         Function prototypes
\* ------------------------------------------------------------------ */
/* Read ambient light level from Pico's analog-to-digital gpio. */
UINT16 adc_read_light(void);

/* Read Pico's internal temperature from Pico's analog-to-digital gpio. */
void adc_read_pico_temp(float *DegreeC, float *DegreeF);

/* Read power supply voltage from Pico's analog-to-digital gpio. */
float adc_read_voltage(void);

/* If auto-brightness is On, adjust clock brightness according to average ambient light level. */
void adjust_clock_brightness(void);

/* Clear all the leds on clock display. */
void clear_all_leds(void);

/* Clear the display framebuffer. */
void clear_framebuffer(UINT8 StartColumn);

/* Convert a 24-hour format time to 12-hour format. */
UINT8 convert_h24_to_h12(UINT8 Hour, UINT8 *AmFlag, UINT8 *PmFlag);

/* Convert "TmTime" to "Unix Time". */
UINT64 convert_tm_to_unix(struct tm *TmTime);

/* Convert "Unix Time" (Epoch) to "tm Time". Both Unix Time and TmTime are relative to UCT.  */
void convert_unix_to_tm(time_t UnixTime, struct tm *TmTime, UINT8 FlagLocalTime);

/* Queue a command to process while in main thread context. */
UINT8 command_queue(UINT8 Command, UINT16 Parameter);

/* Unqueue a command while in main thread context. */
UINT8 command_unqueue(UINT8 *Command, UINT16 *Parameter);

/* Thread to run on the second Pico's core. */
void core1_main(void);

/* Queue a command for the other core. */
UINT8 core_queue(UINT8 CoreNumber, UINT8 Command);

/* Unqueue a command received from the other core. */
UINT8 core_unqueue(UINT8 CoreNumber);

/* Generate a date stamp for debug info. */
void date_stamp(UCHAR *String);

/* Display data sent in argument through serial port. */
void display_data(UCHAR *Data, UINT32 Size);

/* Evaluate if it is time to blink data on the display (while in setup mode). */
void evaluate_blinking_time(void);

/* Evaluate if it is time to scroll characters ("one dot left") on clock display. */
void evaluate_scroll_time(void);

/* Fill the virtual framebuffer with the given ASCII character, beginning at the specified column position (using 5 X 7 character bitmap). */
UINT16 fill_display_buffer_5X7(UINT8 Column, UINT8 AsciiCharacter);

/* Fill the virtual framebuffer with the given ASCII character, beginning at the specified column position (using 4 X 7 character bitmap). */
void fill_display_buffer_4X7(UINT8 Column, UINT8 AsciiCharacter);

/* Compare crc16 between flash saved configuration and current active configuration. */
void flash_check_config(void);

/* Display flash content through external monitor. */
void flash_display(UINT32 Offset, UINT32 Length);

/* Display Green Clock configuration from flash memory. */
UINT8 flash_display_config(void);

/* Erase data in Pico flash memory. */
void flash_erase(UINT32 FlashMemoryOffset);

/* Read Green Clock configuration from flash memory. */
UINT8 flash_read_config(void);

/* Save current Green Clock configuration to flash memory. */
UINT8 flash_save_config(void);

/* Write data to Pico flash memory. */
UINT flash_write(UINT32 FlashMemoryOffset, UINT8 NewData[], UINT16 NewDataSize);

/* Format string to display temperature on clock display. */
void format_temp(UCHAR *TempString, UCHAR *PreString, float TempCelsius, float Humidity, float Pressure);

/* Get temperature from DS3231. */
float get_ambient_temperature(UINT8 TemperatureUnit);

/* Build the string containing the date to scroll on the clock display. */
void get_date_string(UCHAR *String);

/* Determine the day-of-year of date given in argument. */
UINT16 get_day_of_year(UINT16 YearNumber, UINT8 MonthNumber, UINT8 DayNumber);

/* Determine the day-of-year for DST start day and DST end day. */
void get_dst_days(void);

/* Determine if the microcontroller is a Pico or a Pico W. */
UINT8 get_microcontroller_type(void);

/* Return the number of days in a specific month (while checking if it is a leap year or not for February). */
UINT8 get_month_days(UINT16 CurrentYear, UINT8 MonthNumber);

/* Retrieve Pico's Unique ID from flash IC. */
void get_pico_unique_id(void);

/* Return the day-of-week, given the day-of-month, month and year. */
UINT8 get_day_of_week(UINT16 year_cnt, UINT8 month_cnt, UINT8 date_cnt);

/* Initialize all required GPIO ports of the Raspberry Pi Pico. */
int init_gpio(void);

/* Read a string from stdin. */
void input_string(UCHAR *String);

/* Interrupt handler for signal received from IR sensor. */
gpio_irq_callback_t isr_signal_trap(UINT8 gpio, UINT32 Events);

/* Test clock LED matrix, column-by-column, and also all display indicators. */
void matrix_test(UINT8 TestNumber);

/* Make pixel animation for the specified number of seconds. */
void pixel_twinkling(UINT16 Seconds);

/* Play a specific jingle. */
void play_jingle(UINT16 JingleNumber);

/* Handle the command that has been put in command queue. */
void process_command_queue(void);

/* Handle the tag that has been put in the scroll queue. */
void process_scroll_queue(void);

/* Initialize GPIOs to be used as PWM source for clock display brightness and passive buzzer. */
void pwm_initialize(void);

/* Turn On or Off the PWM signal specified in argument. */
void pwm_on_off(UINT8 PwmNumber, UINT8 FlagSwitch);

/* Set the duty cycle for the PWM used for clock display brightness. */
void pwm_set_duty_cycle(UINT8 DutyCycle);

/* Set the frequency for the PWM used for passive buzzer. */
void pwm_set_frequency(UINT16 Frequency);

/* Reverse the bit order of the byte given in argument. */
UINT8 reverse_bits(UINT8 InputByte);

/* Scroll the virtual framebuffer one dot to the left. */
void scroll_one_dot(void);

/* Queue the given tag in the scroll queue. */
UINT8 scroll_queue(UINT8 Tag);

/* Put a specific value in the queue so that we can check for a specific event even inside an ISR.
   MUST BE USED ONLY FOR DEBUG PURPOSES SINCE IT INTERFERES WITH (overwrites) CALENDAR EVENTS. */
UINT8 scroll_queue_value(UINT8 Tag, UCHAR *String);

/* Scroll the specified string on the display. */
void scroll_string(UINT8 StartColumn, UCHAR *String);

/* Unqueue next tag from the scroll queue. */
UINT8 scroll_unqueue(void);

/* Send data to the matrix controller IC. */
void send_data(UINT8 data);

/* Request Wi-Fi SSID and password from user and save them to Pico's flash. */
void set_and_save_credentials(void);

/* Exit current setup mode step. */
void set_mode_out(void);

/* Exit setup mode because of inactivity (time-out). */
void set_mode_timeout(void);

/* Turn On or Off the specified pixel. */
void set_pixel(UINT8 Row, UINT8 Column, UINT8 Flag);

/* Display current alarm parameters. */
void setup_alarm_frame(void);

/* Enter new alarm parameters. */
void setup_alarm_variables(UINT8 FlagButtonSelect);

/* Display current clock parameters. */
void setup_clock_frame(void);

/* Enter new clock parameters. */
void setup_clock_variables(UINT8 FlagButtonSelect);

/* Display current timer parameters. */
void setup_timer_frame(void);

/* Enter new timer parameters. */
void setup_timer_variables(UINT8 FlagButtonSelect);

/* Read the real-time clock IC and display time. */
void show_time(void);

/* Queue the given sound in the active buzzer sound queue. */
UINT16 sound_queue_active(UINT16 MSeconds, UINT16 RepeatCount);

/* Queue the given sound in the passive buzzer sound queue. */
UINT16 sound_queue_passive(UINT16 Frequency, UINT16 MSeconds);

/* Unqueue next sound from the active buzzer sound queue. */
UINT8 sound_unqueue_active(UINT16 *MSeconds, UINT16 *RepeatCount);

/* Unqueue next sound from the passive buzzer sound queue. */
UINT8 sound_unqueue_passive(UINT16 *Frequency, UINT16 *MSeconds);

/* Sound callback function (50 milliseconds period). */
bool sound_callback_ms(struct repeating_timer *Timer50MSec);

/* One millisecond period callback function. */
bool timer_callback_ms(struct repeating_timer *TimerMSec);

/* One second period callback function. */
bool timer_callback_s(struct repeating_timer *TimerSec);

/* Make a tone for the specified number of milliseconds. */
void tone(UINT16 MilliSeconds);

/* Return total number of "one" bits in the given integer. */
uint8_t total_one_bits(UINT32 Data, UINT8 Size);

/* Send a string to an external terminal emulator program through Pico's UART (or serial port). */
void uart_send(UINT LineNumber, UCHAR *Format, ...);

/* Return the string representing the uint64_t parameter in binary. */
void uint64_to_binary_string(UINT64 Value, UINT8 StringLength, UCHAR *BinaryString);

/* Set DST parameters (FlagSummerTime and Timezone) according to current DST country code and current date and time. */
void update_dst_status();

/* Update indicators at the left of the clock display. */
void update_left_indicators(void);

/* Turn ON the right DayOfWeek indicator on clock display and turn Off all others. */
void update_top_indicators(UINT8 DayOfWeek, UINT8 Flag);





/* ------------------------------------------------------------------ *\
                  BME280-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef BME280_SUPPORT
/* Read temperature, humidity and barometric pressure. */
UINT8 bme280_get_temp(void);

/* Compute calibration parameters from calibration data stored in the specific BME280 device used. */
UINT8 bme280_compute_calib_param(void);

/* Soft-reset the BME280. */
UINT8 bme280_init(void);

/* Read BME280 calibration data from the device's non volatile memory. */
UINT8 bme280_read_calib_data(void);

/* Read BME280 configuration. */
UINT8 bme280_read_config(void);

/* Read BME280 device ID. */
UINT8 bme280_read_device_id(void);

/* Read extra BME280 registers. */
UINT8 bme280_read_all_registers();

/* Read all BME280 data registers (temperature, humidity and barometric pressure). */
UINT8 bme280_read_registers(UINT8 *Register);

/* Read BME280 current status. */
UINT8 bme280_read_status(void);

/* Read BME280 unique id ("serial number" of the specific device used). */
UINT32 bme280_read_unique_id(void);
#endif



/* ------------------------------------------------------------------ *\
                   DHT22-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef DHT_SUPPORT
/* Read the temperature from external DHT22 (this is done by Pico's core 1). */
UINT8 read_dht(float *Temperature, float *Humidity);
#endif  // DHT_SUPPORT



/* ------------------------------------------------------------------ *\
                 IR sensor-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef IR_SUPPORT
/* Decode IR remote control command. */
UINT8 decode_ir_command(UCHAR FlagDebug, UINT8 *IrCommand);

/* Execute the command received from IR remote control. */
void process_ir_command(UINT8 IrCommand);
#endif  // IR_SUPPORT


UINT8 wfetch_day_of_month(void);

void wwrite_day_of_month(UINT8 NewDayOfMonth);

UINT8 wfetch_alarm(UINT8 alarm_to_fetch);

void wwrite_alarm(UINT8 alarm_to_write, UINT8 NewFlagValue);

