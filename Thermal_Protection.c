/*******************************************************************************
* Copyright (C) 2013 Spansion LLC. All Rights Reserved. 
*
* This software is owned and published by: 
* Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with Spansion 
* components. This software is licensed by Spansion to be adapted only 
* for use in systems utilizing Spansion components. Spansion shall not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein.  Spansion is providing this software "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the software.  
*
* SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS), 
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING, 
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED 
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED 
* WARRANTY OF NONINFRINGEMENT.  
* SPANSION SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, 
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT 
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, 
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR 
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, 
* SAVINGS OR PROFITS, 
* EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED 
* FROM, THE SOFTWARE.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Disclaimer and Copyright notice must be 
* included with each copy of this software, whether used in part or whole, 
* at all times.  
*/
/******************************************************************************/
/** \file Thermal_Protection.c
 **
 ** Add description here...
 **
 ** History:
 **   - 2014-07-03	0.1.0  devin.zhang  first version
 *****************************************************************************/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "H05_User\AllInOne.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/
stc_thermalprotection_t       Motor1_Thermal_Protection;
stc_lpfparams_t     	Motor1_ThermalLPFPar = {Q12(0.05),Q20(0.0)};
/****************************************************
Function name:	Thermal_Protection
description:    using the ntc or ptc instead of the temperature protection
input:			Pointer of stc_thermalprotection_t
output:			none
****************************************************/
void Thermal_Protect(stc_thermalprotection_t *pstcThermalParams)
{
#if(MOTOR1_THERMAL_PROTECTION == 1)                                    
/*******************************************************************
选取温度对应的 AD
70℃    2.228 （KΩ） 746   (AD NTC connect GND)    3349 (AD NTC connect +5V)
75℃    1.925     661                               3434
80℃    1.669     586                              3509
85℃    1.452     519                              3577
90℃    1.268     461                              3634
95℃    1.110     409                              3686
100℃   0.974     363                               3732
105℃   0.858     324                                3771
110℃   0.758     289                             3806
115℃   0.672     258                              3837
120℃   0.596     230                              3865
125℃   0.531     206                             3889
计算公式：
AD = (4095*R/(R+10)) // ntc 接地端
AD = (4095*10/(R+10)) // ntc 接+5v端  
*********************************************************************/
                    pstcThermalParams->Table[0]= pstcThermalParams->Table[1];
                    pstcThermalParams->Table[1]= pstcThermalParams->Table[2];
                    pstcThermalParams->Table[2]= pstcThermalParams->Table[3];
                    pstcThermalParams->Table[3]= pstcThermalParams->Table[4];
                    pstcThermalParams->Table[4]= pstcThermalParams->Table[5];
                    pstcThermalParams->Table[5]= pstcThermalParams->Table[6];
                    pstcThermalParams->Table[6]= pstcThermalParams->Table[7];
                    pstcThermalParams->Table[7]= gRsltOfADC[MOTOR1_THERMAL_PIN];
                    pstcThermalParams->Average =(pstcThermalParams->Table[0]+pstcThermalParams->Table[1]+pstcThermalParams->Table[2]+pstcThermalParams->Table[3]\
                      +pstcThermalParams->Table[4]+pstcThermalParams->Table[5]+pstcThermalParams->Table[6]+pstcThermalParams->Table[7])>>3;
                    if(pstcThermalParams->Table[0] ==0)
                    {
                       pstcThermalParams->Average =pstcThermalParams->Table[7];
                       pstcThermalParams->Averagef =pstcThermalParams->Table[7];
                    }                    
                    Lpf(pstcThermalParams->Average,&pstcThermalParams->Averagef,&Motor1_ThermalLPFPar);            
                    if(pstcThermalParams->Averagef >= 3771)//105 
                    {
                            pstcThermalParams->TargetSpeed = 0;
                         //   Motor1_RunPar.ErroType |= OVER_TEMPERATURE;
                    }
                    else if(pstcThermalParams->Averagef >= 3686)//95 
                    {
                            pstcThermalParams->TargetSpeed = 200;
                    }
                    else if(pstcThermalParams->Averagef >= 3577)//85
                    {
                            pstcThermalParams->TargetSpeed = 400;                          
                    }
                    else if(pstcThermalParams->Averagef >= 3434)//75
                    {
                            pstcThermalParams->TargetSpeed = 600;                       
                    }
                    else
                    {
                        pstcThermalParams->TargetSpeed = 0xfffffe;
                    }
#else
                    pstcThermalParams->TargetSpeed = 0xfffffe;                   
#endif
}