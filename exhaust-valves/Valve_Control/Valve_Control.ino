// Signal from car
int car = 2;
// Signal to module
int module = 6;
// Signal from remote
int remote = A0;

// Define constants for signals in and out
const double period = 31.2;
const double duty_open = .2;
const double duty_close = .8;

// Define vars for car duty cycle calculation
int duty;
volatile double duty_cycle = .2;
volatile int prev_time = 0;

// Define vars for module signal generation
double sending_duty_cycle;
double high_delay = 0;
double low_delay = 0;

// Define vars for control decision making
int remote_val = 0;
bool car_control = false;
 
void setup() {
  Serial.begin(9600);
  
  // Set up pins
  pinMode(car, INPUT);
  pinMode(module, OUTPUT);
  pinMode(remote, INPUT);
  
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
  
  // When pin car pin goes high, call the rising function
  attachInterrupt(digitalPinToInterrupt(car), rising, RISING);
}
 
void loop() {
  remote_val = analogRead(remote);
  if(remote_val > 500){
    car_control = false;
    sending_duty_cycle = duty_close;
    cycle();
  }
  else{
    car_control = false;
    sending_duty_cycle = duty_open;
    cycle();
  }

}

double calc_duty_cycle(double period, double val){
  return double(period*(val/1023))/period;
}

void cycle(){
    high_delay = period*sending_duty_cycle;
    low_delay = period*(1-sending_duty_cycle);
    digitalWrite(module, HIGH);          
    floatDelay(high_delay);
    digitalWrite(module, LOW);
    floatDelay(low_delay);
}
 
void rising() {
  if (car_control){
    digitalWrite(module, HIGH);
  }
  attachInterrupt(digitalPinToInterrupt(car), falling, FALLING);
  prev_time = micros();
}
 
void falling() {
  if (car_control){
    digitalWrite(module, LOW);
  }
  attachInterrupt(digitalPinToInterrupt(car), rising, RISING);
  duty = micros()-prev_time;
  duty_cycle = double(duty)/(period*1000);
//  Serial.println(duty_cycle);
}

// Combination of delay ms and us for smoother movement
void floatDelay(float ms)
{
  unsigned long whole = ms;
  int part = (ms - whole) * 1000;
  delay(whole);
  if (part > 4) delayMicroseconds(part);
}
