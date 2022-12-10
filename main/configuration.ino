#define RESET_CCU 27
#define RTS_CCU 14

//  latch
#define L_Relay_S_1 32
#define L_Relay_R_1 33

#define L_Relay_S_2 23
#define L_Relay_R_2 13

// LED1
#define LED1 12

// buttons
#define wake_up_button 36

//  global variables
String pump_relay_state, valve_relay_state;

void IO_config() {

  // LED
  pinMode(LED1, OUTPUT);
  
  // RESET CCU
  pinMode(RESET_CCU, OUTPUT);
  pinMode(RTS_CCU, OUTPUT);

  //  pins config
  pinMode(wake_up_button, INPUT);

  //  latch
  pinMode(L_Relay_S_1, OUTPUT);
  pinMode(L_Relay_R_1, OUTPUT);
  pinMode(L_Relay_S_2, OUTPUT);
  pinMode(L_Relay_R_2, OUTPUT);

  //  RESET CCU
  digitalWrite(RESET_CCU, LOW);
}
