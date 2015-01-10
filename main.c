/*
 * main.c
 */

#include <stdio.h>
#include <file.h>

#include "DSP28x_Project.h"     // DSP28x Headerfile
//#include "ti_ascii.h"

#include "f2802x_common/include/adc.h"
#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/flash.h"
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/pll.h"
#include "f2802x_common/include/sci.h"
#include "f2802x_common/include/sci_io.h"
#include "f2802x_common/include/wdog.h"

extern void DSP28x_usDelay(Uint32 Count);

CLK_Handle myClk;
GPIO_Handle myGpio;
PIE_Handle myPie;
int main(void) {
    volatile int status = 0;
    //volatile FILE *fid;

    CPU_Handle myCpu;
    PLL_Handle myPll;
    WDOG_Handle myWDog;

    // Initialize all the handles needed for this application
    myClk = CLK_init((void *)CLK_BASE_ADDR, sizeof(CLK_Obj));
    myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));
    myPll = PLL_init((void *)PLL_BASE_ADDR, sizeof(PLL_Obj));
    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));
    myWDog = WDOG_init((void *)WDOG_BASE_ADDR, sizeof(WDOG_Obj));

    // Perform basic system initialization
    WDOG_disable(myWDog);
    CLK_enableAdcClock(myClk);
    (*Device_cal)();

    //Select the internal oscillator 1 as the clock source
    //InternalOsc rate is 10MHz
    CLK_setOscSrc(myClk, CLK_OscSrc_Internal);

    // Setup the PLL for x12 /2 which will yield 60Mhz = 10Mhz * 12 / 2
    PLL_setup(myPll, PLL_Multiplier_12, PLL_DivideSelect_ClkIn_by_2);

    // Disable the PIE and all interrupts
    PIE_disable(myPie);
    PIE_disableAllInts(myPie);
    CPU_disableGlobalInts(myCpu);
    CPU_clearIntFlags(myCpu);


    // Initalize GPIO
    // Enable XCLOCKOUT to allow monitoring of oscillator 1
    GPIO_setMode(myGpio, GPIO_Number_18, GPIO_18_Mode_XCLKOUT);
    CLK_setClkOutPreScaler(myClk, CLK_ClkOutPreScaler_SysClkOut_by_1);

    // Configure GPIO 0-3 as outputs - these are the onboard LEDs
    GPIO_setMode(myGpio, GPIO_Number_0, GPIO_0_Mode_GeneralPurpose);
    GPIO_setMode(myGpio, GPIO_Number_1, GPIO_0_Mode_GeneralPurpose);
    GPIO_setMode(myGpio, GPIO_Number_2, GPIO_0_Mode_GeneralPurpose);
    GPIO_setMode(myGpio, GPIO_Number_3, GPIO_0_Mode_GeneralPurpose);

    GPIO_setDirection(myGpio, GPIO_Number_0, GPIO_Direction_Output);
    GPIO_setDirection(myGpio, GPIO_Number_1, GPIO_Direction_Output);
    GPIO_setDirection(myGpio, GPIO_Number_2, GPIO_Direction_Output);
    GPIO_setDirection(myGpio, GPIO_Number_3, GPIO_Direction_Output);

    //This is config for the onboard pushbutton
    GPIO_setMode(myGpio, GPIO_Number_12, GPIO_12_Mode_GeneralPurpose);
    GPIO_setDirection(myGpio, GPIO_Number_12, GPIO_Direction_Input);
    GPIO_setPullUp(myGpio, GPIO_Number_12, GPIO_PullUp_Disable);

    //Scan the LEDs until the pushbutton is pressed
    while(GPIO_getData(myGpio, GPIO_Number_12) != 1)
    {
        GPIO_setHigh(myGpio, GPIO_Number_0);
        GPIO_setHigh(myGpio, GPIO_Number_1);
        GPIO_setHigh(myGpio, GPIO_Number_2);
        GPIO_setLow(myGpio, GPIO_Number_3);
        DELAY_US(50000);

        GPIO_setHigh(myGpio, GPIO_Number_0);
        GPIO_setHigh(myGpio, GPIO_Number_1);
        GPIO_setLow(myGpio, GPIO_Number_2);
        GPIO_setHigh(myGpio, GPIO_Number_3);
        DELAY_US(50000);

        GPIO_setHigh(myGpio, GPIO_Number_0);
        GPIO_setLow(myGpio, GPIO_Number_1);
        GPIO_setHigh(myGpio, GPIO_Number_2);
        GPIO_setHigh(myGpio, GPIO_Number_3);
        DELAY_US(50000);

        GPIO_setLow(myGpio, GPIO_Number_0);
        GPIO_setHigh(myGpio, GPIO_Number_1);
        GPIO_setHigh(myGpio, GPIO_Number_2);
        GPIO_setHigh(myGpio, GPIO_Number_3);
        DELAY_US(500000);
    }

	return 0;
}
