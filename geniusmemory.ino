// RED (BUTTON AND LED PINS)
# define red_button 13
# define red_led 7

// GREEN (BUTTON AND LED PINS)
# define green_button 12
# define green_led 6

// YELLOW (BUTTON AND LED PINS)
# define yellow_button 11
# define yellow_led 5

// BLUE (BUTTON AND LED PINS)
# define blue_button 10
# define blue_led 4

// BUZZER PIN
# define buzzer 3

// TO START THE GAME
# define start_button 9

/// TIME COUNTING
typedef struct {
  unsigned long current;
  unsigned long variation;
  unsigned long start;
  unsigned long end;
}time_counting;

// DEFINING THE TIMER
time_counting leds_timer;

// GAME VARIABLES
int sequence[500];
int level = 1;
int play = 0;
int counter = 0;
bool game_started = false;
int led_to_turn_on = 3;
bool led_was_turned_off = true;

// SOUND VARIABLES
const int winFrequencies[] = { 523, 659, 784, 1047 }; // NOTES C5, E5, G5, C6
const int winDurations[] = { 200, 200, 200, 600 }; // DURATION IN MILLISSECONDS
const int loseFrequencies[] = { 294, 262, 220, 196 }; // NOTES D4, C4, A3, G3
const int loseDurations[] = { 300, 300, 300, 600 }; // DURATION IN MILLISSECONDS

// FUNCTIONS PROTOTYPES
void wating_start(void);
void started(void);
void showing_sequence(void);
void blink_led(int led, unsigned long time);
void waiting_play(void);
bool correct_play(int index);
void reset_game(void);
void play_sound(String sound);
bool timer(time_counting *time, unsigned long stipulated_time);
void reset_timer(time_counting *time);

void setup()
{

  pinMode(red_button, INPUT_PULLUP); // RED BUTTON
  pinMode(red_led, OUTPUT); // RED LED
  pinMode(green_button, INPUT_PULLUP); // GREEN BUTTON
  pinMode(green_led, OUTPUT); // GREEN LED
  pinMode(yellow_button, INPUT_PULLUP); // YELLOW BUTTON
  pinMode(yellow_led, OUTPUT); // YELLOW LED
  pinMode(blue_button, INPUT_PULLUP); // BLUE BUTTON
  pinMode(blue_led, OUTPUT); // BLUE LED
	
  pinMode(buzzer, OUTPUT); // BUZZER
  pinMode(start_button, INPUT_PULLUP); // TURN ON AND OFF DEVICE
  
  reset_timer(&leds_timer);
  
}

void loop()
{
  if (game_started){
  	showing_sequence();
    waiting_play();
  } else{
    wating_start();
  }
  delay(10);
}



// BEGINNING
void wating_start(void){
  if (led_was_turned_off){
    if (timer(&leds_timer, 200)){
      digitalWrite(led_to_turn_on + 4, HIGH);
      led_was_turned_off = !led_was_turned_off;
      reset_timer(&leds_timer);
    }
  } else {
    if (timer(&leds_timer, 200)){
      digitalWrite(led_to_turn_on + 4, LOW);
      led_was_turned_off = !led_was_turned_off;
      reset_timer(&leds_timer);
      led_to_turn_on ++;
      led_to_turn_on %= 4;
    }
  }
  started();
}

// STARTED?
void started(void){
  if (digitalRead(9) == LOW){
    game_started = true;
    play_sound("200");
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, HIGH);
  	digitalWrite(yellow_led, HIGH);
    digitalWrite(blue_led, HIGH);
    delay(1000);
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
  	digitalWrite(yellow_led, LOW);
    digitalWrite(blue_led, LOW);
    
    randomSeed(analogRead(0));
    delay(1000);
  }
}

// SHOWING SEQUENCE THAT PLAYER MUST REACH
void showing_sequence(void){
  led_to_turn_on = random(4);
  sequence[level - 1] = led_to_turn_on;
  
  for (int i = 0; i < level; i++){
    blink_led(sequence[i] + 4, 300);
  }

}

// TO BLINK LEDS
void blink_led(int led, unsigned long time){
  play_sound( (String) ((led + 1)*300));
  digitalWrite(led, HIGH);
  reset_timer(&leds_timer);
  while (!timer(&leds_timer, time)){}
  digitalWrite(led, LOW);
  reset_timer(&leds_timer);
  while (!timer(&leds_timer, time)){}
}

// WAITING PLAYER MAKE ALL PLAYS OF A LEVEL TO GO TO THE NEXT ONE
void waiting_play(void){
  play = 0;
  counter = 0;

  while (counter < level){
    // IF THE USER PRESS ANY BUTTON
    if (digitalRead(play + 10) == LOW){
      // WAIT UNTIL THE USER RELEASE THE PRESSED BUTTON
      while (digitalRead(play + 10) == LOW){}
      // BLINK THE CORRESPONDING LED
      blink_led(play + 4, 300);
      // CHECK IF THE PRESSED BUTTON IS CORRECT
      if (correct_play(counter)){
        counter ++;
      
      // WRONG LED
      } else {
        reset_game();
        play_sound("LOSE");
        return;
      }
    }
    // CHANGE THE LED CHECKING
    play ++;
    play = play % 4;
    delay(10);
  }
  // GO TO THE NEXT LEVEL
  level ++;
  play_sound("WIN");
  return;
}

// CHECK IF THE USER MADE A CORRECT PLAY
bool correct_play(int index){
  if (play == sequence[index]){
    return true;
  } else {
  	return false;
  }
}

// IN CASE THE USER LOSE THE GAME TO RESET SOME GAME VARIABLES
void reset_game(void){
  level = 1;
  game_started = false;
  led_to_turn_on = 3;
  led_was_turned_off = true;
}

// PLAY SOUNDS WITH THE BUZZER
void play_sound(String sound){
  // WIN SOUND
  if (sound.equals("WIN")){
    for (int i = 0; i < 4; i++) {
      tone(buzzer, winFrequencies[i], winDurations[i]);
      delay(winDurations[i] + 50);
    }
  
  // LOSE SOUND
  } else if (sound.equals("LOSE")) {
    for (int i = 0; i < 4; i++) {
      tone(buzzer, loseFrequencies[i], loseDurations[i]);
      delay(loseDurations[i] + 50); // Espera um pouco mais entre as notas
    }
  
  // BEEP SOUND
  } else {
    tone(buzzer, sound.toInt(), 200);
  }
}

// TIMER
bool timer(time_counting *time, unsigned long stipulated_time){

  time->end = millis();
  time->variation = time->end - time->start;
  
  // HANDLES WHEN MILLIS() EXCEEDS ITS MAX CAPACITY
  if (time->end < time->start)
    time->variation = 4294967295 + time->end - time->start;
  
  time->start = time->end; // RESET THE TIME COUNT
  
  // ADD VARIATION OF TIME TO CURRENT TIME
  time->current += time->variation;
  
  return time->current >= stipulated_time;
}

// TIMER RESETTER
void reset_timer(time_counting *time){
  time->current = 0;
  time->variation = 0;
  time->start = millis();
  time->end = millis();
}
