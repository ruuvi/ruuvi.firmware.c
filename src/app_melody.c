#include "app_melody.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "ruuvi_interface_gpio.h"
#include "ruuvi_interface_yield.h"
#include <stddef.h>
#if RB_BUZZER_PRESENT
//https://www.arduino.cc/en/Tutorial/PlayMelody

/* Play Melody
 * -----------
 *
 * Program to play a simple melody
 *
 * Tones are created by quickly pulsing a speaker on and off 
 *   using PWM, to create signature frequencies.
 *
 * Each note has a frequency, created by varying the period of 
 *  vibration, measured in microseconds. We'll use pulse-width
 *  modulation (PWM) to create that vibration.

 * We calculate the pulse-width to be half the period; we pulse 
 *  the speaker HIGH for 'pulse-width' microseconds, then LOW 
 *  for 'pulse-width' microseconds.
 *  This pulsing creates a vibration of the desired frequency.
 *
 * (cleft) 2005 D. Cuartielles for K3
 * Refactoring and comments 2006 clay.shirky@nyu.edu
 * See NOTES in comments at end for possible improvements
 */

// TONES  ==========================================
// Start by defining the relationship between 
//       note, period, &  frequency. 
#define  c     3830U    // 261 Hz 
#define  d     3400U    // 294 Hz 
#define  e     3038U    // 329 Hz 
#define  f     2864U    // 349 Hz 
#define  g     2550U    // 392 Hz 
#define  a     2272U    // 440 Hz 
#define  b     2028U    // 493 Hz 
#define  C     1912U    // 523 Hz 
// Define a special note, 'R', to represent a rest
#define  R     0U

// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
static const ri_gpio_id_t speakerOut = RB_BUZZER_1_PIN;

rd_status_t app_melody_init(void) 
{ 
    ri_gpio_configure(speakerOut, RI_GPIO_MODE_OUTPUT_HIGHDRIVE);
}

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 
static const uint16_t melody[] = {  c,  c,  c,  e,  d,   d,  d,  f,  e,  e, d, d, c  };
static const uint16_t beats[]  = {  8,  8,  8,  8,  8,   8,  8,  8,  8,  8, 8, 8, 16 }; 
static const size_t MAX_COUNT = sizeof(melody) / sizeof(melody[0]); // Melody length, for looping.

// Set overall tempo
static const uint32_t tempo = 25000U;
// Set length of pause between notes
static const uint32_t pause = 100000U;
// Loop variable to increase Rest length
static const uint32_t rest_count = 100U; //<- HACK; See NOTES

// Initialize core variables
static int32_t tone_ = 0;
static int32_t beat = 0;
static int32_t duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
static void app_melody_tone(void) 
{
  int32_t elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      ri_gpio_write(speakerOut, RI_GPIO_HIGH);
      ri_delay_us(tone_ / 2);

      // DOWN
      ri_gpio_write(speakerOut, RI_GPIO_LOW);
      ri_delay_us(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      ri_delay_us(duration);  
    }                                
  }                                 
}


void app_melody_play() {
  // Set up a counter to pull from melody[] and beats[]
  for (size_t i = 0; i < MAX_COUNT; i++) {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo; // Set up timing

    app_melody_tone(); 
    // A pause between notes...
    ri_delay_us(pause);
  }
}

/*
 * NOTES
 * The program purports to hold a tone for 'duration' microseconds.
 *  Lies lies lies! It holds for at least 'duration' microseconds, _plus_
 *  any overhead created by incremeting elapsed_time (could be in excess of 
 *  3K microseconds) _plus_ overhead of looping and two digitalWrites()
 *  
 * As a result, a tone of 'duration' plays much more slowly than a rest
 *  of 'duration.' rest_count creates a loop variable to bring 'rest' beats 
 *  in line with 'tone' beats of the same length. 
 * 
 * rest_count will be affected by chip architecture and speed, as well as 
 *  overhead from any program mods. Past behavior is no guarantee of future 
 *  performance. Your mileage may vary. Light fuse and get away.
 *  
 * This could use a number of enhancements:
 * ADD code to let the programmer specify how many times the melody should
 *     loop before stopping
 * ADD another octave
 * MOVE tempo, pause, and rest_count to #define statements
 * RE-WRITE to include volume, using analogWrite, as with the second program at
 *          http://www.arduino.cc/en/Tutorial/PlayMelody
 * ADD code to make the tempo settable by pot or other input device
 * ADD code to take tempo or volume settable by serial communication 
 *          (Requires 0005 or higher.)
 * ADD code to create a tone offset (higer or lower) through pot etc
 * REPLACE random melody with opening bars to 'Smoke on the Water'
 */
#else
rd_status_t app_melody_init(void)
{
  return RD_SUCCESS;
}
void app_melody_play(void)
{}
#endif
