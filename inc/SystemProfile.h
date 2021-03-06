/*
 * MIT License
 *
 * Copyright (c) 2021 Mark Retalack
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef INC_SYSTEMPROFILE_H_
#define INC_SYSTEMPROFILE_H_


#ifndef __CATFEEDER__
#define __CATFEEDER__
#endif

#include "GenericTypeDefs.h"

#ifndef MRF24J40
#define MRF24J40
#endif

// 11 + 8  + 8 + 12
// 91
#define RX_BUFFER_SIZE 70


#define INPUT

int RFIF_PIN();
void PHY_RESETn_LOW();
void PHY_RESETn_HIGH();
void PHYSetShortRAMAddr(INPUT BYTE address, INPUT BYTE value);
BYTE PHYGetShortRAMAddr(INPUT BYTE address);
BYTE PHYGetLongRAMAddr(INPUT WORD address);
void PHYSetLongRAMAddr(INPUT WORD address, INPUT BYTE value);



#endif /* INC_SYSTEMPROFILE_H_ */
