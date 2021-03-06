/**HEADER********************************************************************
 Copyright (c) 2010-2014 Freescale Semiconductor, Inc.
 ALL RIGHTS RESERVED.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY DIRECT, 
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************
*                                                                            
*        Standard Software Flash Driver For FTFx                             
*                                                                            
* FILE NAME     :  PFlashSwap.c                                              
* DATE          :  Dec 25, 2013                                              
*                                                                            
* AUTHOR        :  FPT Team                                                  
* E-mail        :  r56611@freescale.com                                      
*                                                                            
*****************************************************************************
0.0.1       06.09.2010      FPT Team      Initial Version
1.0.0       12.25.2013      FPT Team      Optimize code 
1.0.1       01.16.2014      FPT Team      Add to provide CurrentSwapBlockStatus 
                                          and NextSwapBlockStatus information
*END*************************************************************************/
/* include the header files */
#include "SSD_FTFx.h"

#ifdef SWAP
/************************************************************************
*
*  Function Name    : PFlashSwapCtl
*  Description      : Execute swap command represented by a control code
*                     
*  Arguments        : PFLASH_SSD_CONFIG, UINT32, UINT8,
*                     UINT8* pCurrentSwapMode,UINT8* pCurrentSwapBlockStatus,
*                     UINT8* pNextSwapBlockStatus,
*                     pFLASHCOMMANDSEQUENCE
*                     
*  Return Value     : UINT32
*
*************************************************************************/
UINT32 PFlashSwapCtl(PFLASH_SSD_CONFIG pSSDConfig,UINT32 addr, UINT8 swapcmd,UINT8* pCurrentSwapMode, \
                        UINT8* pCurrentSwapBlockStatus, \
                        UINT8* pNextSwapBlockStatus, \
                        pFLASHCOMMANDSEQUENCE pFlashCommandSequence)
{
    UINT32 ret;      /* Return code variable */
    addr = WORD2BYTE(addr - pSSDConfig->PFlashBlockBase);
    /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register. Write 1 to clear*/
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FSTAT_OFFSET,FTFx_SSD_FSTAT_ERROR_BITS);
    
    /* passing parameter to the command */
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB0_OFFSET, FTFx_PFLASH_SWAP);
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB1_OFFSET, GET_BIT_16_23(addr));
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB2_OFFSET, GET_BIT_8_15(addr));
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB3_OFFSET, GET_BIT_0_7(addr));
    
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB4_OFFSET, swapcmd);
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB5_OFFSET, 0xFF);
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB6_OFFSET, 0xFF);
    REG_WRITE(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB7_OFFSET, 0xFF);

    /* calling flash command sequence function to execute the command */
    ret = pFlashCommandSequence(pSSDConfig);

    if (FTFx_OK == ret)
    {
        *pCurrentSwapMode = REG_READ(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB5_OFFSET);
        *pCurrentSwapBlockStatus = REG_READ(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB6_OFFSET);
        *pNextSwapBlockStatus = REG_READ(pSSDConfig->ftfxRegBase + FTFx_SSD_FCCOB7_OFFSET);
    }   
    
    /* Enter Debug state if enabled */
    if (TRUE == (pSSDConfig->DebugEnable))
    {
        ENTER_DEBUG_MODE;
    }
    return (ret);
}
#endif
/* end of file */
