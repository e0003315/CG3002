
int count = 0;
//int count2 = 0;
int flag = 0;

void setup()
{
  Serial.begin(115200);
  
  // initialize timer3 
  noInterrupts();           // disable all interrupts
  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3  = 0;

  OCR3A = 3125;                           // compare match register 16MHz/64/80Hz (ans must be <65536)
  TCCR3B |= (1 << WGM32);                 // CTC mode (clear timer on compare match)
  TCCR3B |= (1 << CS31) | (1 << CS30);    // 64 prescaler 
  TIMSK3 |= (1 << OCIE3A);                // enable timer compare interrupt
  interrupts();                           // enable all interrupts
}

ISR(TIMER3_COMPA_vect)                     // timer3 compare interrupt service routine
{
  if (flag==1)
    Serial.print("miss a beat");
  else
    flag = 1;
}

void loop()
{
  if(flag == 1){
    flag = 0;
    
//    if(count >= 80){    //code to check 80Hz - count to 80 and print via serial monitor 
//      count=0;          //delete if not in use
//      count2=count2+1;
//      Serial.println(count2);
//    }
//    else{
//      count = count+1;
//    }

//tasks goes here

  }
}
