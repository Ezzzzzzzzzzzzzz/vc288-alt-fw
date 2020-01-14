#include "config.h"
#include "gpio.h"
#include "timer.h"
#include "display_7seg.h"
#include <stdint.h>

/* Private variables */
static const uint8_t seven_seg_lut[] = { 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 
                                         0x5F, 0x70, 0x7F, 0x7B, 0x77, 0x1F,
                                         0x4E, 0x3D, 0x4F, 0x47, 0x00 };

static uint8_t display_data[6] = { 0 };
static uint8_t display_pos     = 0;

//
//  Timer 2 Overflow handler.
//
#pragma vector = TIM2_OVR_UIF_vector
__interrupt void TIM2_UPD_OVF_IRQHandler(void)
{
    if (++display_pos == 7)
    {
        display_pos = 0;
    }

    set_segments(0);                            /* Blank first */
    select_digit(display_pos);                  /* Select the digit */
    set_segments(display_data[display_pos]);    /* And set the data */

    // Reset the interrupt, otherwise it will fire again straight away.
    TIM2_SR1_UIF = 0;
}

void set_display_from_int(uint16_t number, 
                          uint8_t row,
                          uint8_t decimal_dots)
{
    // Note: 8th bit controls the decimal place separator
    display_data[row+2] = seven_seg_lut[number % 10] | 
                          (decimal_dots & 1 ? 0x80 : 0);
    number /= 10;
    decimal_dots >>= 1;
    display_data[row+1] = seven_seg_lut[number % 10] | 
                          (decimal_dots & 1 ? 0x80 : 0);
    number /= 10;
    decimal_dots >>= 1;
    display_data[row+0] = seven_seg_lut[number % 10] | 
                          (decimal_dots & 1 ? 0x80 : 0);
}