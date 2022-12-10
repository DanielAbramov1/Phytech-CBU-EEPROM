#include "driver/pcnt.h" // Library ESP32 PCNT          

#define PCNT_COUNT_UNIT       PCNT_UNIT_0                                 // Set Pulse Counter Unit - 0 
#define PCNT_COUNT_CHANNEL    PCNT_CHANNEL_0                              // Set Pulse Counter channel - 0 

#define PCNT_INPUT_SIG_IO     GPIO_NUM_34                                 // Set Pulse Counter input - Freq Meter Input GPIO 34
#define PCNT_H_LIM_VAL        overflow                                    // Overflow of Pulse Counter 
uint32_t        overflow      = 32767;                                    // Max Pulse Counter value

int16_t         pulses        = 0;                                        // Pulse Counter value
uint32_t        multPulses    = 0;                                        // Quantidade de overflows do contador PCNT

#define sample_time_milliseconds 1000
long last_sample_time = 0;
float frq = 0;

esp_timer_create_args_t create_args;                                      // Create an esp_timer instance
esp_timer_handle_t timer_handle;                                          // Create an single timer

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;                     // portMUX_TYPE to do synchronism


//----------------------------------------------------------------------------------
static void IRAM_ATTR pcnt_intr_handler(void *arg)                        // Counting overflow pulses
{
  portENTER_CRITICAL_ISR(&timerMux);                                      // disabling the interrupts
  multPulses++;                                                           // increment Overflow counter
  PCNT.int_clr.val = BIT(PCNT_COUNT_UNIT);                                // Clear Pulse Counter interrupt bit
  portEXIT_CRITICAL_ISR(&timerMux);                                       // enabling the interrupts
}

//----------------------------------------------------------------------------------
void init_PCNT(void)         //2                                             // Initialize and run PCNT unit
{
  pcnt_config_t pcnt_config = { };                                        // PCNT unit instance

  gpio_set_pull_mode(PCNT_INPUT_SIG_IO, GPIO_PULLUP_ONLY);


  pcnt_config.pulse_gpio_num = PCNT_INPUT_SIG_IO;                         // Pulse input GPIO 34 - Freq Meter Input
  //  pcnt_config.ctrl_gpio_num = PCNT_INPUT_CTRL_IO;                         // Control signal input GPIO 35
  pcnt_config.unit = PCNT_COUNT_UNIT;                                     // Unidade de contagem PCNT - 0
  pcnt_config.channel = PCNT_COUNT_CHANNEL;                               // PCNT unit number - 0
  pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;                             // Maximum counter value - 20000
  pcnt_config.pos_mode = PCNT_COUNT_INC;                                  // PCNT positive edge count mode - inc
  pcnt_config.neg_mode = PCNT_COUNT_INC;                                  // PCNT negative edge count mode - inc
  pcnt_config.lctrl_mode = PCNT_MODE_DISABLE;                             // PCNT low control mode - disable
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;                                // PCNT high control mode - won't change counter mode
  pcnt_unit_config(&pcnt_config);                                         // Initialize PCNT unit

  pcnt_counter_pause(PCNT_COUNT_UNIT);                                    // Pause PCNT unit
  pcnt_counter_clear(PCNT_COUNT_UNIT);                                    // Clear PCNT unit

  pcnt_event_enable(PCNT_COUNT_UNIT, PCNT_EVT_H_LIM);                     // Enable event to watch - max count
  pcnt_isr_register(pcnt_intr_handler, NULL, 0, NULL);                    // Setup Register ISR handler
  pcnt_intr_enable(PCNT_COUNT_UNIT);                                      // Enable interrupts for PCNT unit

  pcnt_counter_resume(PCNT_COUNT_UNIT);                                   // Resume PCNT unit - starts count
}

//----------------------------------------------------------------------------------
void read_PCNT(void *p)                                                   // Read Pulse Counter
{
  should_calc_frequency = true;
  pcnt_get_counter_value(PCNT_COUNT_UNIT, &pulses);                       // Read Pulse Counter value
}

//---------------------------------------------------------------------------------
void init_frequencyMeter () // setup
{
  init_PCNT();                                                            // Initialize and run PCNT unit

  create_args.callback = read_PCNT;                                       // Set esp-timer argument
  esp_timer_create(&create_args, &timer_handle);                          // Create esp-timer instance
  esp_timer_start_once(timer_handle, sample_time_milliseconds * 1000);                            // Initialize High resolution timer (1 sec)
  gpio_matrix_in(PCNT_INPUT_SIG_IO, SIG_IN_FUNC226_IDX, false);           // Set GPIO matrin IN - Freq Meter input
}

float calc_freq()
{
  //    float frq = (pulses + (multPulses * overflow)) / 2  ;                     // Calculation of frequency
  should_calc_frequency = false;

  if (pulses > 0)
  {
    frq = (float)(pulses + (multPulses * overflow)) * 1000 / (millis() - last_sample_time);
    frq /= 2.0;
    multPulses = 0;                               // Clear overflow counter
    pcnt_counter_clear(PCNT_COUNT_UNIT);          // Clear Pulse Counter

    last_sample_time = millis();
  }

  if (millis() - last_sample_time > 5 * 1000)
    frq = 0;

  esp_timer_start_once(timer_handle, sample_time_milliseconds * 1000);  // Initialize High resolution timer (1 sec)

  return frq;
}
