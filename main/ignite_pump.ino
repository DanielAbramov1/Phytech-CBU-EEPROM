typedef enum pump_sm_states {PUMP_IDLE, IGNITE_PUMP, WAIT_FOR_PUMP, PUMP_ON, PUMP_ON_FOR_60_SECONDS} pump_sm_states;
const char *pump_sm_states_strings[] = {"PUMP_IDLE", "IGNITE_PUMP", "WAIT_FOR_PUMP", "PUMP_ON", "PUMP_ON_FOR_60_SECONDS"};
void set_pump_sm_state(pump_sm_states st);
pump_sm_states get_pump_sm_state();

bool request = false;
bool timeout_5_seconds = false;
bool failed_3_times = false;
bool dry_contact = false;

pump_sm_states current_state = PUMP_IDLE;

void pump_sm_loop()
{
  switch (current_state)
  {
    case PUMP_IDLE:
      break;
    case IGNITE_PUMP:
      break;
    case WAIT_FOR_PUMP:
      break;
    case PUMP_ON:
      break;
    case PUMP_ON_FOR_60_SECONDS:
      break;
  }
}

void set_pump_sm_state(pump_sm_states st)
{
  current_state = st;
  printf("pump_sm_states: %s\n", pump_sm_states_strings[current_state]);
}

pump_sm_states get_pump_sm_state()
{
  return current_state;
}
