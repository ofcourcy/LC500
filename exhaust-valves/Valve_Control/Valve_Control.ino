#include <limits.h>

// Define pins
// Signal from car
int car = 2;
// Signal to module
int module1 = 6;
int module2 = 7;
// Signal from remote
int remote = A0;

// Define constants for signals in and out
const double period = 32;
const double duty_open = .2;
const double duty_close = .8;

// Define vars for car duty cycle calculation
int duty;
volatile double duty_from_car = .2;
volatile unsigned long prev_time = 0;

// Define vars for module signal generation
double dutyvalves;
double high_delay = 0;
double low_delay = 0;

// Define vars for control decision making

int remote_val = 0;
bool car_control = false;
 
void setup() {
  Serial.begin(9600);
  
  // Set up pins
  pinMode(car, INPUT);
  pinMode(module1, OUTPUT);
  pinMode(module2, OUTPUT);
  pinMode(remote, INPUT);

  // Begin startup signal
  startup_singal();
  
  // When car pin goes high, call the rising function
  attachInterrupt(digitalPinToInterrupt(car), rising, RISING);
}
 
void loop() {
  remote_val = analogRead(remote);
  if(remote_val > 500){
    car_control = false;
    dutyvalves = duty_close;
    delay_cycle();
  }
  else{
    car_control = false;
    dutyvalves = duty_open;
    delay_cycle();
  }

}

double calc_duty_cycle(double period, double val){
  return double(period*(val/1023))/period;
}

void delay_cycle(){
    high_delay = period*dutyvalves;
    low_delay = period*(1-dutyvalves);
    digitalWrite(module1, HIGH);
    digitalWrite(module2, HIGH);         
    floatDelay(high_delay);
    digitalWrite(module1, LOW);
    digitalWrite(module2, LOW);
    floatDelay(low_delay);
}
 
void rising() {
  if (car_control){
    digitalWrite(module1, HIGH);
    digitalWrite(module2, HIGH);
  }
  attachInterrupt(digitalPinToInterrupt(car), falling, FALLING);
  prev_time = micros();
}
 
void falling() {
  if (car_control){
    digitalWrite(module1, LOW);
    digitalWrite(module2, LOW);
  }
  attachInterrupt(digitalPinToInterrupt(car), rising, RISING);
  duty = micros()-prev_time;
//  Sanity check for micros() overflow
  if(duty > 0){
    duty_from_car = double(duty)/(period*1000);
  }
  else{
    duty_from_car = double(duty+ULONG_MAX)/(period*1000);
  }
  
//  Serial.println(duty_from_car);
}

void startup_singal(){
  digitalWrite(module1, LOW);
  digitalWrite(module2, LOW);
  delay(5000);
  digitalWrite(module1, HIGH);
  digitalWrite(module2, HIGH);
  delay(66);
  digitalWrite(module1, LOW);
  digitalWrite(module2, LOW);
  delay(25);
  for(int x = 0; x<1100/32; x++){
    dutyvalves = .2;
    delay_cycle();
  }
  for(int x = 0; x<3300/32; x++){
    dutyvalves = .15;
    delay_cycle();
  }
}

// Combination of delay ms and us for smoother movement
void floatDelay(float ms)
{
  unsigned long whole = ms;
  int part = (ms - whole) * 1000;
  delay(whole);
  if (part > 4) delayMicroseconds(part);
}