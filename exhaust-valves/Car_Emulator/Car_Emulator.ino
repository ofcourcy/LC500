//Initializing LED Pin
int module = 6;
int potentiometer = A0;
double input = 0;
double duty_cycle = .50;
double cycle_time = 31.2;
double high_delay = 0;
double low_delay = 0;
void setup() {
  Serial.begin(9600);
  
  // Set up pins
  pinMode(module, OUTPUT);
  pinMode(potentiometer, INPUT);
  
  // Begin startup signal
  digitalWrite(module, LOW);
  delay(5000);
  digitalWrite(module, HIGH);          
  delay(66);
  digitalWrite(module, LOW);
  delay(25);
  for(int x = 0; x<1100/32; x++){
    duty_cycle = .2;
    cycle();
  }
  for(int x = 0; x<3300/32; x++){
    duty_cycle = .15;
    cycle();
  }
  // End startup signal
}
void loop() {
  input = analogRead(potentiometer);
  duty_cycle = calc_duty_cycle(cycle_time, input);
  if(duty_cycle < .2){
    duty_cycle = .2;
  }
  if(duty_cycle > .75){
    duty_cycle = .75;
  }
  Serial.println(duty_cycle);
  cycle();
}

double calc_duty_cycle(double cycle_time, double analog_value){
  // Use 1023 max value of analogRead() to calc percentage and convert to duty cycle
  return double(cycle_time*(analog_value/1023))/cycle_time;
}

void cycle(){
  high_delay = cycle_time*duty_cycle;
  low_delay = cycle_time*(1-duty_cycle);
  Serial.println(high_delay);
  Serial.println(low_delay);
  digitalWrite(module, HIGH);          
  floatDelay(high_delay);
  digitalWrite(module, LOW);
  floatDelay(low_delay);
}

// Combination of delay ms and us for smoother movement
void floatDelay(float ms)
{
  unsigned long whole = ms;
  int part = (ms - whole) * 1000;
  delay(whole);
  if (part > 4) delayMicroseconds(part);
}
