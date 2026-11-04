/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//****************************************************************************
//
// HAL_TMP006.c - Hardware abstraction layer for interfacing TMP006
//
//****************************************************************************

#include "HAL_TMP006.h"
#include "HAL_I2C.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "math.h"

/* Calibration constant for TMP006 */
static long double S0 = 0;


/***************************************************************************//**
 * @brief  Configures the TMP006 Infrared Thermopile Sensor
 * @param  none
 * @return none
 ******************************************************************************/
void TMP006_init(void)
{
    /* Specify slave address for TMP006 */
    I2C_setslave(TMP006_SLAVE_ADDRESS);

    /* Reset TMP006 */
    I2C_write16(TMP006_WRITE_REG, TMP006_RST);

    volatile int i;
    for (i=10000; i>0;i--);

    /* Power-up and re-enable device */
    I2C_write16(TMP006_WRITE_REG, TMP006_POWER_UP | TMP006_CR_2);
}

int TMP006_readDeviceId(void)
{
    /* Specify slave address for TMP006 */
    I2C_setslave(TMP006_SLAVE_ADDRESS);

    return I2C_read16(TMP006_P_DEVICE_ID);
}

int TMP006_readObjectVoltage(void)
{
    /* Specify slave address for TMP006 */
    I2C_setslave(TMP006_SLAVE_ADDRESS);

    return I2C_read16(TMP006_P_VOBJ);
}

int TMP006_readAmbientTemperature(void)
{
    /* Specify slave address for TMP006 */
    I2C_setslave(TMP006_SLAVE_ADDRESS);

    return I2C_read16(TMP006_P_TABT);
}


/***************************************************************************//**
 * @brief  Reads data from the sensor and calculates the object temperature
 * @param  none
 * @return Object temperature in degrees fahrenheit
 ******************************************************************************/
long double TMP006_getTemp(void)
{
    volatile int Vobj = 0;
    volatile int Tdie = 0;

    Vobj = TMP006_readDeviceId();

    /* Read the object voltage */
    Vobj = TMP006_readObjectVoltage();

    /* Read the ambient temperature */
    Tdie = TMP006_readAmbientTemperature();
    Tdie = Tdie >> 2;

    /* Calculate TMP006. This needs to be reviewed and calibrated */
    long double Vobj2 = (double)Vobj*.00000015625;
    long double Tdie2 = (double)Tdie*.03525 + 273.15;

    /* Initialize constants */
    S0 = 6 * pow(10, -14);
    long double a1 = 1.75*pow(10, -3);
    long double a2 = -1.678*pow(10, -5);
    long double b0 = -2.94*pow(10, -5);
    long double b1 = -5.7*pow(10, -7);
    long double b2 = 4.63*pow(10, -9);
    long double c2 = 13.4;
    long double Tref = 298.15;

    /* Calculate values */
    long double S = S0*(1+a1*(Tdie2 - Tref)+a2*pow((Tdie2 - Tref),2));
    long double Vos = b0 + b1*(Tdie2 - Tref) + b2*pow((Tdie2 - Tref),2);
    volatile long double fObj = (Vobj2 - Vos) + c2*pow((Vobj2 - Vos),2);
    volatile long double Tobj = pow(pow(Tdie2,4) + (fObj/S),.25);
    Tobj = (9.0/5.0)*(Tobj - 273.15) + 32;

    /* Return temperature of object */
    return (Tobj);
}
