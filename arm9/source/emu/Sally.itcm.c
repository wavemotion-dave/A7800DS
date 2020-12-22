// ----------------------------------------------------------------------------
//   ___  ___  ___  ___       ___  ____  ___  _  _
//  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
// /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
//
// ----------------------------------------------------------------------------
// Copyright 2005 Greg Stanton
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// ----------------------------------------------------------------------------
// Sally.cpp
// ----------------------------------------------------------------------------
#include "Sally.h"
#include "ProSystem.h"

byte sally_a = 0;
byte sally_x = 0;
byte sally_y = 0;
byte sally_p = 0;
byte sally_s = 0;
pair sally_pc = {0};

static byte sally_opcode;
static pair sally_address;
static uint sally_cyclesX4;

// Whether the last operation resulted in a half cycle. (needs to be taken 
// into consideration by ProSystem when cycle counting). This can occur when
// a TIA or RIOT are accessed (drops to 1.19Mhz when the TIA or RIOT chips 
// are accessed)
bool half_cycle = false;

typedef struct {
  byte C;
  byte Z;
  byte I;
  byte D;
  byte B;
  byte R;
  byte V;
  byte N;
} Flag;

static const Flag SALLY_FLAG = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

typedef struct {
  word H;
  word L;
} Vector;

static const Vector SALLY_RES = {65533, 65532};
static const Vector SALLY_NMI = {65531, 65530};
static const Vector SALLY_IRQ = {65535, 65534}; 

static const byte SALLY_CYCLESX4[256] = {
	7*4,6*4,0*4,0*4,0*4,3*4,5*4,0*4,3*4,2*4,2*4,2*4,0*4,4*4,6*4,0*4,
	2*4,5*4,0*4,0*4,0*4,4*4,6*4,0*4,2*4,4*4,0*4,0*4,0*4,4*4,7*4,0*4,
	6*4,6*4,0*4,0*4,3*4,3*4,5*4,0*4,4*4,2*4,2*4,2*4,4*4,4*4,6*4,0*4,
	2*4,5*4,0*4,0*4,0*4,4*4,6*4,0*4,2*4,4*4,0*4,0*4,0*4,4*4,7*4,0*4,
	6*4,6*4,0*4,0*4,0*4,3*4,5*4,0*4,3*4,2*4,2*4,2*4,3*4,4*4,6*4,0*4,
	2*4,5*4,0*4,0*4,0*4,4*4,6*4,0*4,2*4,4*4,0*4,0*4,0*4,4*4,7*4,0*4,
	6*4,6*4,0*4,0*4,0*4,3*4,5*4,0*4,4*4,2*4,2*4,0*4,5*4,4*4,6*4,0*4,
	2*4,5*4,0*4,0*4,0*4,4*4,6*4,0*4,2*4,4*4,0*4,0*4,0*4,4*4,7*4,0*4,
	0*4,6*4,0*4,0*4,3*4,3*4,3*4,0*4,2*4,0*4,2*4,0*4,4*4,4*4,4*4,0*4,
	2*4,6*4,0*4,0*4,4*4,4*4,4*4,0*4,2*4,5*4,2*4,0*4,0*4,5*4,0*4,0*4,
	2*4,6*4,2*4,0*4,3*4,3*4,3*4,0*4,2*4,2*4,2*4,0*4,4*4,4*4,4*4,0*4,
	2*4,5*4,0*4,0*4,4*4,4*4,4*4,0*4,2*4,4*4,2*4,0*4,4*4,4*4,4*4,0*4,
	2*4,6*4,0*4,0*4,3*4,3*4,5*4,0*4,2*4,2*4,2*4,0*4,4*4,4*4,6*4,0*4,
	2*4,5*4,0*4,0*4,0*4,4*4,6*4,0*4,2*4,4*4,0*4,0*4,0*4,4*4,7*4,0*4,
	2*4,6*4,0*4,0*4,3*4,3*4,5*4,0*4,2*4,2*4,2*4,0*4,4*4,4*4,6*4,0*4,
	2*4,5*4,0*4,0*4,0*4,4*4,6*4,0*4,2*4,4*4,0*4,0*4,0*4,4*4,7*4,0*4
};

// For when you know it's an opcode or operand and, therefore, just a normal RAM/ROM fetch
static inline byte memory_Read_Fast(word addr)
{
    return memory_ram[addr];
}

// ----------------------------------------------------------------------------
// Push
// ----------------------------------------------------------------------------
static inline void sally_Push(byte data) 
{
  memory_Write(sally_s + 256, data);
  sally_s--;
}

// ----------------------------------------------------------------------------
// Pop
// ----------------------------------------------------------------------------
static inline byte sally_Pop( ) 
{
  sally_s++;
  return memory_ram[sally_s + 256];
}

// ----------------------------------------------------------------------------
// Flags
// ----------------------------------------------------------------------------
static inline void sally_Flags(byte data) {
#define _fC 0x01
#define _fZ 0x02
#define _fI 0x04
#define _fD 0x08
#define _fB 0x10
#define _fT 0x20
#define _fV 0x40
#define _fN 0x80

  sally_p = (sally_p & ~(_fN | _fZ)) |  ((data) & _fN) | (((data) == 0) ? _fZ : 0);
}


// ----------------------------------------------------------------------------
// Branch
// ----------------------------------------------------------------------------
static inline void sally_Branch(byte branch) {
  if(branch) {
    pair temp = sally_pc;
    sally_pc.w += (signed char)sally_address.b.l;
       
    if(temp.b.h != sally_pc.b.h) {
      sally_cyclesX4 += 8;
    }
    else {
      sally_cyclesX4 += 4;
    }
  }
}

// ----------------------------------------------------------------------------
// Delay
// ----------------------------------------------------------------------------
static inline void sally_Delay(byte delta) {
  pair address1 = sally_address;
  address1.w -= delta;
  if(address1.b.h != sally_address.b.h) {
    sally_cyclesX4 += 4;
  }
}

// ----------------------------------------------------------------------------
// Absolute
// ----------------------------------------------------------------------------
static inline void sally_Absolute( ) {
  sally_address.b.l = memory_Read_Fast(sally_pc.w++);
  sally_address.b.h = memory_Read_Fast(sally_pc.w++);
}

// ----------------------------------------------------------------------------
// AbsoluteX
// ----------------------------------------------------------------------------
static inline void sally_AbsoluteX( ) {
  sally_address.b.l = memory_Read_Fast(sally_pc.w++);
  sally_address.b.h = memory_Read_Fast(sally_pc.w++);
  sally_address.w += sally_x;
}

// ----------------------------------------------------------------------------
// AbsoluteY
// ----------------------------------------------------------------------------
static inline void sally_AbsoluteY( ) {
  sally_address.b.l = memory_Read_Fast(sally_pc.w++);
  sally_address.b.h = memory_Read_Fast(sally_pc.w++);
  sally_address.w += sally_y;
}

// ----------------------------------------------------------------------------
// Immediate
// ----------------------------------------------------------------------------
static inline void sally_Immediate( ) {
  sally_address.w = sally_pc.w++;
}

// ----------------------------------------------------------------------------
// Indirect
// ----------------------------------------------------------------------------
static inline void sally_Indirect( ) {
  pair base;
  base.b.l = memory_Read_Fast(sally_pc.w++);
  base.b.h = memory_Read_Fast(sally_pc.w++);
  sally_address.b.l = memory_Read(base.w);
  sally_address.b.h = memory_Read(base.w + 1);
}

// ----------------------------------------------------------------------------
// IndirectX
// ----------------------------------------------------------------------------
static inline void sally_IndirectX( ) {
  sally_address.b.l = memory_Read_Fast(sally_pc.w++) + sally_x;
  sally_address.b.h = memory_Read(sally_address.b.l + 1);
  sally_address.b.l = memory_Read(sally_address.b.l);
}

// ----------------------------------------------------------------------------
// IndirectY
// ----------------------------------------------------------------------------
static inline void sally_IndirectY( ) {
  sally_address.b.l = memory_Read_Fast(sally_pc.w++);
  sally_address.b.h = memory_Read(sally_address.b.l + 1);
  sally_address.b.l = memory_Read(sally_address.b.l);
  sally_address.w += sally_y;
}

// ----------------------------------------------------------------------------
// Relative
// ----------------------------------------------------------------------------
static inline void sally_Relative( ) {
  sally_address.w = memory_Read_Fast(sally_pc.w++);
}

// ----------------------------------------------------------------------------
// Zero Page
// ----------------------------------------------------------------------------
static inline void sally_ZeroPage( ) {
  sally_address.w = memory_Read_Fast(sally_pc.w++);
}

// ----------------------------------------------------------------------------
// ZeroPageX
// ----------------------------------------------------------------------------
static inline void sally_ZeroPageX( ) {
  sally_address.w = memory_Read_Fast(sally_pc.w++);
  sally_address.b.l += sally_x;
}

// ----------------------------------------------------------------------------
// ZeroPageY
// ----------------------------------------------------------------------------
static inline void sally_ZeroPageY( ) {
  sally_address.w = memory_Read_Fast(sally_pc.w++);
  sally_address.b.l += sally_y;
}

// ----------------------------------------------------------------------------
// ADC
// ----------------------------------------------------------------------------
static inline void sally_ADC( ) {
  byte data = memory_Read(sally_address.w);
    
  if(sally_p & SALLY_FLAG.D) {
    word al = (sally_a & 15) + (data & 15) + (sally_p & SALLY_FLAG.C);
    word ah = (sally_a >> 4) + (data >> 4);

    if(al > 9) {
      al += 6;
      ah++;
    }

    if(!(sally_a + data + (sally_p & SALLY_FLAG.C))) {
      sally_p |= SALLY_FLAG.Z;
    }
    else {
      sally_p &= ~SALLY_FLAG.Z;
    }

    if((ah & 8) != 0) {
      sally_p |= SALLY_FLAG.N;      
    }
    else {
      sally_p &= ~SALLY_FLAG.N;
    }

    if(~(sally_a ^ data) & ((ah << 4) ^ sally_a) & 128) {
      sally_p |= SALLY_FLAG.V;
    }
    else {
      sally_p &= ~SALLY_FLAG.V;
    }

    if(ah > 9) {
      ah += 6;
    }

    if(ah > 15) {
      sally_p |= SALLY_FLAG.C;      
    }
    else {
      sally_p &= ~SALLY_FLAG.C;
    }

    sally_a = (ah << 4) | (al & 15);
  }
  else {
    pair temp;
    temp.w = sally_a + data + (sally_p & SALLY_FLAG.C);

    if(temp.b.h) {
      sally_p |= SALLY_FLAG.C;
    }
    else {
      sally_p &= ~SALLY_FLAG.C;
    }
        
    if(~(sally_a ^ data) & (sally_a ^ temp.b.l) & 128) {
      sally_p |= SALLY_FLAG.V;
    }
    else {
      sally_p &= ~SALLY_FLAG.V;
    }
        
    sally_Flags(temp.b.l);
    sally_a = temp.b.l;
  }
}

// ----------------------------------------------------------------------------
// AND
// ----------------------------------------------------------------------------
static inline void sally_AND( ) {
  sally_a &= memory_Read(sally_address.w);
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// ASLA
// ----------------------------------------------------------------------------
static inline void sally_ASLA( ) {
  if(sally_a & 128) {
    sally_p |= SALLY_FLAG.C;
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  sally_a <<= 1;
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// ASL
// ----------------------------------------------------------------------------
static inline void sally_ASL( ) {
  byte data = memory_Read(sally_address.w);
    
  if(data & 128) {
    sally_p |= SALLY_FLAG.C;
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  data <<= 1;
  memory_Write(sally_address.w, data);
  sally_Flags(data);
}

// ----------------------------------------------------------------------------
// BCC
// ----------------------------------------------------------------------------
static inline void sally_BCC( ) {
  sally_Branch(!(sally_p & SALLY_FLAG.C));
}

// ----------------------------------------------------------------------------
// BCS
// ----------------------------------------------------------------------------
static inline void sally_BCS( ) {
  sally_Branch(sally_p & SALLY_FLAG.C);
}

// ----------------------------------------------------------------------------
// BEQ
// ----------------------------------------------------------------------------
static inline void sally_BEQ( ) {
  sally_Branch(sally_p & SALLY_FLAG.Z);
}

// ----------------------------------------------------------------------------
// BIT
// ----------------------------------------------------------------------------
static inline void sally_BIT( ) {
  byte data = memory_Read(sally_address.w);
    
  if(!(data & sally_a)) {
    sally_p |= SALLY_FLAG.Z;
  }
  else {
    sally_p &= ~SALLY_FLAG.Z;
  }

  sally_p &= ~SALLY_FLAG.V;
  sally_p &= ~SALLY_FLAG.N;  
  sally_p |= data & 64;
  sally_p |= data & 128;
}

// ----------------------------------------------------------------------------
// BMI
// ----------------------------------------------------------------------------
static inline void sally_BMI( ) {
  sally_Branch(sally_p & SALLY_FLAG.N);
}

// ----------------------------------------------------------------------------
// BNE
// ----------------------------------------------------------------------------
static inline void sally_BNE( ) {
  sally_Branch(!(sally_p & SALLY_FLAG.Z));
}

// ----------------------------------------------------------------------------
// BPL
// ----------------------------------------------------------------------------
static inline void sally_BPL( ) {
  sally_Branch(!(sally_p & SALLY_FLAG.N));
}

// ----------------------------------------------------------------------------
// BRK
// ----------------------------------------------------------------------------
static inline void sally_BRK( ) {
  sally_pc.w++;
  sally_p |= SALLY_FLAG.B;
    
  sally_Push(sally_pc.b.h);
  sally_Push(sally_pc.b.l);
  sally_Push(sally_p);

  sally_p |= SALLY_FLAG.I;
  sally_pc.b.l = memory_ram[SALLY_IRQ.L];
  sally_pc.b.h = memory_ram[SALLY_IRQ.H];
}

// ----------------------------------------------------------------------------
// BVC
// ----------------------------------------------------------------------------
static inline void sally_BVC( ) {
  sally_Branch(!(sally_p & SALLY_FLAG.V));
}

// ----------------------------------------------------------------------------
// BVS
// ----------------------------------------------------------------------------
static inline void sally_BVS( ) {
  sally_Branch(sally_p & SALLY_FLAG.V);
}

// ----------------------------------------------------------------------------
// CLC
// ----------------------------------------------------------------------------
static inline void sally_CLC( ) {
  sally_p &= ~SALLY_FLAG.C;
}

// ----------------------------------------------------------------------------
// CLD
// ----------------------------------------------------------------------------
static inline void sally_CLD( ) {
  sally_p &= ~SALLY_FLAG.D;
}

// ----------------------------------------------------------------------------
// CLI
// ----------------------------------------------------------------------------
static inline void sally_CLI( ) {
  sally_p &= ~SALLY_FLAG.I;
}

// ----------------------------------------------------------------------------
// CLV
// ----------------------------------------------------------------------------
static inline void sally_CLV( ) {
  sally_p &= ~SALLY_FLAG.V;
}

// ----------------------------------------------------------------------------
// CMP
// ----------------------------------------------------------------------------
static inline void sally_CMP( ) {
  byte data = memory_Read(sally_address.w);
    
  if(sally_a >= data) {
    sally_p |= SALLY_FLAG.C;
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  sally_Flags(sally_a - data);
}

// ----------------------------------------------------------------------------
// CPX
// ----------------------------------------------------------------------------
static inline void sally_CPX( ) {
  byte data = memory_Read(sally_address.w);
    
  if(sally_x >= data) {
    sally_p |= SALLY_FLAG.C;  
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  sally_Flags(sally_x - data);
}

// ----------------------------------------------------------------------------
// CPY
// ----------------------------------------------------------------------------
static inline void sally_CPY( ) {
  byte data = memory_Read(sally_address.w);

  if(sally_y >= data) {
    sally_p |= SALLY_FLAG.C;
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  sally_Flags(sally_y - data);
}

// ----------------------------------------------------------------------------
// DEC
// ----------------------------------------------------------------------------
static inline void sally_DEC( ) {
  byte data = memory_Read(sally_address.w);
  memory_Write(sally_address.w, --data);
  sally_Flags(data);
}

// ----------------------------------------------------------------------------
// DEX
// ----------------------------------------------------------------------------
static inline void sally_DEX( ) {
  sally_Flags(--sally_x);
}

// ----------------------------------------------------------------------------
// DEY
// ----------------------------------------------------------------------------
static inline void sally_DEY( ) {
  sally_Flags(--sally_y);
}

// ----------------------------------------------------------------------------
// EOR
// ----------------------------------------------------------------------------
static inline void sally_EOR( ) {
  sally_a ^= memory_Read(sally_address.w);
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// INC
// ----------------------------------------------------------------------------
static inline void sally_INC( ) {
  byte data = memory_Read(sally_address.w);
  memory_Write(sally_address.w, ++data);
  sally_Flags(data);
}

// ----------------------------------------------------------------------------
// INX
// ----------------------------------------------------------------------------
static inline void sally_INX( ) {
  sally_Flags(++sally_x);
}

// ----------------------------------------------------------------------------
// INY
// ----------------------------------------------------------------------------
static inline void sally_INY( ) {
  sally_Flags(++sally_y);
}

// ----------------------------------------------------------------------------
// JMP
// ----------------------------------------------------------------------------
static inline void sally_JMP( ) {
  sally_pc = sally_address;
}

// ----------------------------------------------------------------------------
// JSR
// ----------------------------------------------------------------------------
static inline void sally_JSR( ) {
  sally_pc.w--;
  sally_Push(sally_pc.b.h);
  sally_Push(sally_pc.b.l);
    
  sally_pc = sally_address;
}

// ----------------------------------------------------------------------------
// LDA
// ----------------------------------------------------------------------------
static inline void sally_LDA( ) {
  sally_a = memory_Read(sally_address.w);
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// LDX
// ----------------------------------------------------------------------------
static inline void sally_LDX( ) {
  sally_x = memory_Read(sally_address.w);
  sally_Flags(sally_x);
}

// ----------------------------------------------------------------------------
// LDY
// ----------------------------------------------------------------------------
static inline void sally_LDY( ) {
  sally_y = memory_Read(sally_address.w);
  sally_Flags(sally_y);
}

// ----------------------------------------------------------------------------
// LSRA
// ----------------------------------------------------------------------------
static inline void sally_LSRA( ) {
  sally_p &= ~SALLY_FLAG.C;
  sally_p |= sally_a & 1;
    
  sally_a >>= 1;
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// LSR
// ----------------------------------------------------------------------------
static inline void sally_LSR( ) {
  byte data = memory_Read(sally_address.w);
    
  sally_p &= ~SALLY_FLAG.C;
  sally_p |= data & 1;

  data >>= 1;
  memory_Write(sally_address.w, data);
  sally_Flags(data);
}

// ----------------------------------------------------------------------------
// NOP
// ----------------------------------------------------------------------------
static inline void sally_NOP( ) {
}

// ----------------------------------------------------------------------------
// ORA
// ----------------------------------------------------------------------------
static inline void sally_ORA( ) {
  sally_a |= memory_Read(sally_address.w);
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// PHA
// ----------------------------------------------------------------------------
static inline void sally_PHA( ) {
  sally_Push(sally_a);    
}

// ----------------------------------------------------------------------------
// PHP
// ----------------------------------------------------------------------------
static inline void sally_PHP( ) {
  sally_Push(sally_p);
}

// ----------------------------------------------------------------------------
// PLA
// ----------------------------------------------------------------------------
static inline void sally_PLA( ) {
  sally_a = sally_Pop( );
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// PLP
// ----------------------------------------------------------------------------
static inline void sally_PLP( ) {
  sally_p = sally_Pop( );
}

// ----------------------------------------------------------------------------
// ROLA
// ----------------------------------------------------------------------------
static inline void sally_ROLA( ) {
  byte temp = sally_p;

  if(sally_a & 128) {
    sally_p |= SALLY_FLAG.C;  
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  sally_a <<= 1;
  sally_a |= temp & SALLY_FLAG.C;
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// ROL
// ----------------------------------------------------------------------------
static inline void sally_ROL( ) {
  byte data = memory_Read(sally_address.w);
  byte temp = sally_p;
    
  if(data & 128) {
    sally_p |= SALLY_FLAG.C;
  }
  else {
    sally_p &= ~SALLY_FLAG.C;
  }

  data <<= 1;
  data |= temp & 1;
  memory_Write(sally_address.w, data);
  sally_Flags(data);
}

// ----------------------------------------------------------------------------
// RORA
// ----------------------------------------------------------------------------
static inline void sally_RORA( ) {
  byte temp = sally_p;

  sally_p &= ~SALLY_FLAG.C;
  sally_p |= sally_a & 1;
    
  sally_a >>= 1;
  if(temp & SALLY_FLAG.C) {
    sally_a |= 128;
  }
    
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// ROR
// ----------------------------------------------------------------------------
static inline void sally_ROR( ) {
  byte data = memory_Read(sally_address.w);
  byte temp = sally_p;
    
  sally_p &= ~SALLY_FLAG.C;
  sally_p |= data & 1;

  data >>= 1;
  if(temp & 1) {
     data |= 128;
  }

  memory_Write(sally_address.w, data);
  sally_Flags(data);
}

// ----------------------------------------------------------------------------
// RTI
// ----------------------------------------------------------------------------
static inline void sally_RTI( ) {
  sally_p = sally_Pop( );
  sally_pc.b.l = sally_Pop( );
  sally_pc.b.h = sally_Pop( );
}

// ----------------------------------------------------------------------------
// RTS
// ----------------------------------------------------------------------------
static inline void sally_RTS( ) {
  sally_pc.b.l = sally_Pop( );
  sally_pc.b.h = sally_Pop( );
  sally_pc.w++;
}

// ----------------------------------------------------------------------------
// SBC
// ----------------------------------------------------------------------------
static inline void sally_SBC( ) {
  byte data = memory_Read(sally_address.w);

  if(sally_p & SALLY_FLAG.D) {
    word al = (sally_a & 15) - (data & 15) - !(sally_p & SALLY_FLAG.C);
    word ah = (sally_a >> 4) - (data >> 4);
        
    if(al > 9) {
      al -= 6;
      ah--;
    }

    if(ah > 9) {
      ah -= 6;
    }
    
    pair temp;
    temp.w = sally_a - data - !(sally_p & SALLY_FLAG.C);

    if(!temp.b.h) {
      sally_p |= SALLY_FLAG.C;
    }
    else {
      sally_p &= ~SALLY_FLAG.C;
    }
                
    if((sally_a ^ data) & (sally_a ^ temp.b.l) & 128) {
      sally_p |= SALLY_FLAG.V;
    }
    else {
      sally_p &= ~SALLY_FLAG.V;      
    }

    sally_Flags(temp.b.l);
    sally_a = (ah << 4) | (al & 15);
  }
  else {
    pair temp;
    temp.w = sally_a - data - !(sally_p & SALLY_FLAG.C);
        
    if(!temp.b.h) {
      sally_p |= SALLY_FLAG.C;
    }
    else {
      sally_p &= ~SALLY_FLAG.C;
    }
                
    if((sally_a ^ data) & (sally_a ^ temp.b.l) & 128) {
      sally_p |= SALLY_FLAG.V;
    }
    else {
      sally_p &= ~SALLY_FLAG.V;
    }
        
    sally_Flags(temp.b.l);
    sally_a = temp.b.l;
  }
}

// ----------------------------------------------------------------------------
// SEC
// ----------------------------------------------------------------------------
static inline void sally_SEC( ) {
  sally_p |= SALLY_FLAG.C;  
}

// ----------------------------------------------------------------------------
// SED
// ----------------------------------------------------------------------------
static inline void sally_SED( ) {
  sally_p |= SALLY_FLAG.D;
}

// ----------------------------------------------------------------------------
// SEI
// ----------------------------------------------------------------------------
static inline void sally_SEI( ) {
  sally_p |= SALLY_FLAG.I;
}

// ----------------------------------------------------------------------------
// STA
// ----------------------------------------------------------------------------
static inline void sally_STA( ) {
  memory_Write(sally_address.w, sally_a);
}

// ----------------------------------------------------------------------------
// STX
// ----------------------------------------------------------------------------
static inline void sally_stx( ) {
  memory_Write(sally_address.w, sally_x);
}

// ----------------------------------------------------------------------------
// STY
// ----------------------------------------------------------------------------
static inline void sally_STY( ) {
  memory_Write(sally_address.w, sally_y);
}

// ----------------------------------------------------------------------------
// TAX
// ----------------------------------------------------------------------------
static inline void sally_TAX( ) {
  sally_x = sally_a;
  sally_Flags(sally_x);
}

// ----------------------------------------------------------------------------
// TAY
// ----------------------------------------------------------------------------
static inline void sally_TAY( ) {
  sally_y = sally_a;
  sally_Flags(sally_y);
}

// ----------------------------------------------------------------------------
// TSX
// ----------------------------------------------------------------------------
static inline void sally_TSX( ) {
  sally_x = sally_s;
  sally_Flags(sally_x);
}

// ----------------------------------------------------------------------------
// TXA
// ----------------------------------------------------------------------------
static inline void sally_TXA( ) {
  sally_a = sally_x;
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// TXS
// ----------------------------------------------------------------------------
static inline void sally_TXS( ) {
  sally_s = sally_x;
}

// ----------------------------------------------------------------------------
// TYA
// ----------------------------------------------------------------------------
static inline void sally_TYA( ) {
  sally_a = sally_y;
  sally_Flags(sally_a);
}

// ----------------------------------------------------------------------------
// Reset
// ----------------------------------------------------------------------------
void sally_Reset( ) {
  sally_a = 0;
  sally_x = 0;
  sally_y = 0;
  sally_p = SALLY_FLAG.R;
  sally_s = 0;
  sally_pc.w = 0;
}

// ----------------------------------------------------------------------------
// ExecuteRES
// ----------------------------------------------------------------------------
uint sally_ExecuteRES( ) {
  sally_p = SALLY_FLAG.I | SALLY_FLAG.R | SALLY_FLAG.Z;
  sally_pc.b.l = memory_ram[SALLY_RES.L];
  sally_pc.b.h = memory_ram[SALLY_RES.H];
  return 6;
}

// ----------------------------------------------------------------------------
// ExecuteNMI
// ----------------------------------------------------------------------------
uint sally_ExecuteNMI( ) {
  sally_Push(sally_pc.b.h);
  sally_Push(sally_pc.b.l);
  sally_p &= ~SALLY_FLAG.B;
  sally_Push(sally_p);
  sally_p |= SALLY_FLAG.I;
  sally_pc.b.l = memory_ram[SALLY_NMI.L];
  sally_pc.b.h = memory_ram[SALLY_NMI.H];
  return 7;
}

// ----------------------------------------------------------------------------
// Execute IRQ
// ----------------------------------------------------------------------------
uint sally_ExecuteIRQ( ) {
  if(!(sally_p & SALLY_FLAG.I)) {
    sally_Push(sally_pc.b.h);
    sally_Push(sally_pc.b.l);
    sally_p &= ~SALLY_FLAG.B;
    sally_Push(sally_p);
    sally_p |= SALLY_FLAG.I;
    sally_pc.b.l = memory_ram[SALLY_IRQ.L];
    sally_pc.b.h = memory_ram[SALLY_IRQ.H];
  }
  return 7;
}

extern uint prosystem_cycles;

void sally_Execute(unsigned int cycles )  
{
      __label__ 
    l_0x00, l_0x01, l_0x02, l_0x03, l_0x04, l_0x05, l_0x06, l_0x07, l_0x08,
    l_0x09, l_0x0a, l_0x0b, l_0x0c, l_0x0d, l_0x0e, l_0x0f, l_0x10, l_0x11,
    l_0x12, l_0x13, l_0x14, l_0x15, l_0x16, l_0x17, l_0x18, l_0x19, l_0x1a,
    l_0x1b, l_0x1c, l_0x1d, l_0x1e, l_0x1f, l_0x20, l_0x21, l_0x22, l_0x23,
    l_0x24, l_0x25, l_0x26, l_0x27, l_0x28, l_0x29, l_0x2a, l_0x2b, l_0x2c,
    l_0x2d, l_0x2e, l_0x2f, l_0x30, l_0x31, l_0x32, l_0x33, l_0x34, l_0x35,
    l_0x36, l_0x37, l_0x38, l_0x39, l_0x3a, l_0x3b, l_0x3c, l_0x3d, l_0x3e,
    l_0x3f, l_0x40, l_0x41, l_0x42, l_0x43, l_0x44, l_0x45, l_0x46, l_0x47,
    l_0x48, l_0x49, l_0x4a, l_0x4b, l_0x4c, l_0x4d, l_0x4e, l_0x4f, l_0x50,
    l_0x51, l_0x52, l_0x53, l_0x54, l_0x55, l_0x56, l_0x57, l_0x58, l_0x59,
    l_0x5a, l_0x5b, l_0x5c, l_0x5d, l_0x5e, l_0x5f, l_0x60, l_0x61, l_0x62,
    l_0x63, l_0x64, l_0x65, l_0x66, l_0x67, l_0x68, l_0x69, l_0x6a, l_0x6b,
    l_0x6c, l_0x6d, l_0x6e, l_0x6f, l_0x70, l_0x71, l_0x72, l_0x73, l_0x74,
    l_0x75, l_0x76, l_0x77, l_0x78, l_0x79, l_0x7a, l_0x7b, l_0x7c, l_0x7d,
    l_0x7e, l_0x7f, l_0x80, l_0x81, l_0x82, l_0x83, l_0x84, l_0x85, l_0x86,
    l_0x87, l_0x88, l_0x89, l_0x8a, l_0x8b, l_0x8c, l_0x8d, l_0x8e, l_0x8f,
    l_0x90, l_0x91, l_0x92, l_0x93, l_0x94, l_0x95, l_0x96, l_0x97, l_0x98,
    l_0x99, l_0x9a, l_0x9b, l_0x9c, l_0x9d, l_0x9e, l_0x9f, l_0xa0, l_0xa1,
    l_0xa2, l_0xa3, l_0xa4, l_0xa5, l_0xa6, l_0xa7, l_0xa8, l_0xa9, l_0xaa,
    l_0xab, l_0xac, l_0xad, l_0xae, l_0xaf, l_0xb0, l_0xb1, l_0xb2, l_0xb3,
    l_0xb4, l_0xb5, l_0xb6, l_0xb7, l_0xb8, l_0xb9, l_0xba, l_0xbb, l_0xbc,
    l_0xbd, l_0xbe, l_0xbf, l_0xc0, l_0xc1, l_0xc2, l_0xc3, l_0xc4, l_0xc5,
    l_0xc6, l_0xc7, l_0xc8, l_0xc9, l_0xca, l_0xcb, l_0xcc, l_0xcd, l_0xce,
    l_0xcf, l_0xd0, l_0xd1, l_0xd2, l_0xd3, l_0xd4, l_0xd5, l_0xd6, l_0xd7,
    l_0xd8, l_0xd9, l_0xda, l_0xdb, l_0xdc, l_0xdd, l_0xde, l_0xdf, l_0xe0,
    l_0xe1, l_0xe2, l_0xe3, l_0xe4, l_0xe5, l_0xe6, l_0xe7, l_0xe8, l_0xe9,
    l_0xea, l_0xeb, l_0xec, l_0xed, l_0xee, l_0xef, l_0xf0, l_0xf1, l_0xf2,
    l_0xf3, l_0xf4, l_0xf5, l_0xf6, l_0xf7, l_0xf8, l_0xf9, l_0xfa, l_0xfb,
    l_0xfc, l_0xfd, l_0xfe, l_0xff;

    static const void* const a_jump_table[256] = 
    {
        &&l_0x00, &&l_0x01, &&l_0x02, &&l_0x03, &&l_0x04, &&l_0x05, &&l_0x06, &&l_0x07, &&l_0x08,
        &&l_0x09, &&l_0x0a, &&l_0x0b, &&l_0x0c, &&l_0x0d, &&l_0x0e, &&l_0x0f, &&l_0x10, &&l_0x11,
        &&l_0x12, &&l_0x13, &&l_0x14, &&l_0x15, &&l_0x16, &&l_0x17, &&l_0x18, &&l_0x19, &&l_0x1a,
        &&l_0x1b, &&l_0x1c, &&l_0x1d, &&l_0x1e, &&l_0x1f, &&l_0x20, &&l_0x21, &&l_0x22, &&l_0x23,
        &&l_0x24, &&l_0x25, &&l_0x26, &&l_0x27, &&l_0x28, &&l_0x29, &&l_0x2a, &&l_0x2b, &&l_0x2c,
        &&l_0x2d, &&l_0x2e, &&l_0x2f, &&l_0x30, &&l_0x31, &&l_0x32, &&l_0x33, &&l_0x34, &&l_0x35,
        &&l_0x36, &&l_0x37, &&l_0x38, &&l_0x39, &&l_0x3a, &&l_0x3b, &&l_0x3c, &&l_0x3d, &&l_0x3e,
        &&l_0x3f, &&l_0x40, &&l_0x41, &&l_0x42, &&l_0x43, &&l_0x44, &&l_0x45, &&l_0x46, &&l_0x47,
        &&l_0x48, &&l_0x49, &&l_0x4a, &&l_0x4b, &&l_0x4c, &&l_0x4d, &&l_0x4e, &&l_0x4f, &&l_0x50,
        &&l_0x51, &&l_0x52, &&l_0x53, &&l_0x54, &&l_0x55, &&l_0x56, &&l_0x57, &&l_0x58, &&l_0x59,
        &&l_0x5a, &&l_0x5b, &&l_0x5c, &&l_0x5d, &&l_0x5e, &&l_0x5f, &&l_0x60, &&l_0x61, &&l_0x62,
        &&l_0x63, &&l_0x64, &&l_0x65, &&l_0x66, &&l_0x67, &&l_0x68, &&l_0x69, &&l_0x6a, &&l_0x6b,
        &&l_0x6c, &&l_0x6d, &&l_0x6e, &&l_0x6f, &&l_0x70, &&l_0x71, &&l_0x72, &&l_0x73, &&l_0x74,
        &&l_0x75, &&l_0x76, &&l_0x77, &&l_0x78, &&l_0x79, &&l_0x7a, &&l_0x7b, &&l_0x7c, &&l_0x7d,
        &&l_0x7e, &&l_0x7f, &&l_0x80, &&l_0x81, &&l_0x82, &&l_0x83, &&l_0x84, &&l_0x85, &&l_0x86,
        &&l_0x87, &&l_0x88, &&l_0x89, &&l_0x8a, &&l_0x8b, &&l_0x8c, &&l_0x8d, &&l_0x8e, &&l_0x8f,
        &&l_0x90, &&l_0x91, &&l_0x92, &&l_0x93, &&l_0x94, &&l_0x95, &&l_0x96, &&l_0x97, &&l_0x98,
        &&l_0x99, &&l_0x9a, &&l_0x9b, &&l_0x9c, &&l_0x9d, &&l_0x9e, &&l_0x9f, &&l_0xa0, &&l_0xa1,
        &&l_0xa2, &&l_0xa3, &&l_0xa4, &&l_0xa5, &&l_0xa6, &&l_0xa7, &&l_0xa8, &&l_0xa9, &&l_0xaa,
        &&l_0xab, &&l_0xac, &&l_0xad, &&l_0xae, &&l_0xaf, &&l_0xb0, &&l_0xb1, &&l_0xb2, &&l_0xb3,
        &&l_0xb4, &&l_0xb5, &&l_0xb6, &&l_0xb7, &&l_0xb8, &&l_0xb9, &&l_0xba, &&l_0xbb, &&l_0xbc,
        &&l_0xbd, &&l_0xbe, &&l_0xbf, &&l_0xc0, &&l_0xc1, &&l_0xc2, &&l_0xc3, &&l_0xc4, &&l_0xc5,
        &&l_0xc6, &&l_0xc7, &&l_0xc8, &&l_0xc9, &&l_0xca, &&l_0xcb, &&l_0xcc, &&l_0xcd, &&l_0xce,
        &&l_0xcf, &&l_0xd0, &&l_0xd1, &&l_0xd2, &&l_0xd3, &&l_0xd4, &&l_0xd5, &&l_0xd6, &&l_0xd7,
        &&l_0xd8, &&l_0xd9, &&l_0xda, &&l_0xdb, &&l_0xdc, &&l_0xdd, &&l_0xde, &&l_0xdf, &&l_0xe0,
        &&l_0xe1, &&l_0xe2, &&l_0xe3, &&l_0xe4, &&l_0xe5, &&l_0xe6, &&l_0xe7, &&l_0xe8, &&l_0xe9,
        &&l_0xea, &&l_0xeb, &&l_0xec, &&l_0xed, &&l_0xee, &&l_0xef, &&l_0xf0, &&l_0xf1, &&l_0xf2,
        &&l_0xf3, &&l_0xf4, &&l_0xf5, &&l_0xf6, &&l_0xf7, &&l_0xf8, &&l_0xf9, &&l_0xfa, &&l_0xfb,
        &&l_0xfc, &&l_0xfd, &&l_0xfe, &&l_0xff 
    };

  // Reset half cycle flag
  half_cycle = false;

  while (prosystem_cycles<cycles) 
  {
    sally_opcode = memory_ram[sally_pc.w++];
    sally_cyclesX4 = SALLY_CYCLESX4[sally_opcode];
  
	goto *a_jump_table[sally_opcode];
  
    l_0x00:
      sally_BRK( ); 
      goto next_inst;

    l_0x01:
      sally_IndirectX( ); 
      sally_ORA( ); 
      goto next_inst;
    
    l_0x05:
      sally_ZeroPage( );  
      sally_ORA( ); 
      goto next_inst;

    l_0x06: 
      sally_ZeroPage( );
      sally_ASL( );
      goto next_inst;

    l_0x08: 
      sally_PHP( );
      goto next_inst;

    l_0x09: 
      sally_Immediate( ); 
      sally_ORA( ); 
      goto next_inst;        

    l_0x0a: 
      sally_ASLA( ); 
      goto next_inst;        

    l_0x0d: 
      sally_Absolute( );  
      sally_ORA( ); 
      goto next_inst;

    l_0x0e: 
      sally_Absolute( );  
      sally_ASL( ); 
      goto next_inst;

    l_0x10: 
      sally_Relative( );
      sally_BPL( );
      goto next_inst;        

    l_0x11: 
      sally_IndirectY( ); 
      sally_ORA( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0x15: 
      sally_ZeroPageX( ); 
      sally_ORA( ); 
      goto next_inst;

    l_0x16: 
      sally_ZeroPageX( ); 
      sally_ASL( ); 
      goto next_inst;

    l_0x18: 
      sally_CLC( );
      goto next_inst;

    l_0x19: 
      sally_AbsoluteY( ); 
      sally_ORA( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0x1d: 
      sally_AbsoluteX( ); 
      sally_ORA( ); 
      sally_Delay(sally_x); 
      goto next_inst;

    l_0x1e: 
      sally_AbsoluteX( ); 
      sally_ASL( ); 
      goto next_inst;

    l_0x20: 
      sally_Absolute( );  
      sally_JSR( ); 
      goto next_inst;

    l_0x21: 
      sally_IndirectX( );
      sally_AND( );
      goto next_inst;

    l_0x24: 
      sally_ZeroPage( );
      sally_BIT( );
      // Add a half cycle if RIOT/TIA location is accessed. We only track
      // INPT4 since it is the only one that is accessed during the lightgun
      // hit detection loop. This should be extended to take into consideration
      // all RIOT and TIA accesses.
      if( sally_address.w == INPT4 )
      {
        half_cycle = true;
      }
      goto next_inst;

    l_0x25: 
      sally_ZeroPage( );
      sally_AND( ); 
      goto next_inst;

    l_0x26: 
      sally_ZeroPage( );
      sally_ROL( );
      goto next_inst;

    l_0x28:
      sally_PLP( );
      goto next_inst;

    l_0x29:
      sally_Immediate( );
      sally_AND( );
      goto next_inst;

    l_0x2a: 
      sally_ROLA( );
      goto next_inst;

    l_0x2c: 
      sally_Absolute( );
      sally_BIT( );
      goto next_inst;

    l_0x2d: 
      sally_Absolute( );
      sally_AND( );
      goto next_inst;

    l_0x2e: 
      sally_Absolute( );
      sally_ROL( );
      goto next_inst;

    l_0x30:
      sally_Relative( );
      sally_BMI( );
      goto next_inst;

    l_0x31: 
      sally_IndirectY( );
      sally_AND( );
      sally_Delay(sally_y);
      goto next_inst;

    l_0x35: 
      sally_ZeroPageX( ); 
      sally_AND( ); 
      goto next_inst;

    l_0x36: 
      sally_ZeroPageX( ); 
      sally_ROL( ); 
      goto next_inst;

    l_0x38: 
      sally_SEC( );
      goto next_inst;

    l_0x39: 
      sally_AbsoluteY( );
      sally_AND( );
      sally_Delay(sally_y);
      goto next_inst;

    l_0x3d: 
      sally_AbsoluteX( ); 
      sally_AND( );
      sally_Delay(sally_x);
      goto next_inst;

    l_0x3e: 
      sally_AbsoluteX( );
      sally_ROL( );
      goto next_inst;

    l_0x40: 
      sally_RTI( );
      goto next_inst;

    l_0x41: 
      sally_IndirectX( ); 
      sally_EOR( ); 
      goto next_inst;

    l_0x45: 
      sally_ZeroPage( );
      sally_EOR( );
      goto next_inst;

    l_0x46: 
      sally_ZeroPage( );
      sally_LSR( );
      goto next_inst;

    l_0x48: 
      sally_PHA( );
      goto next_inst;

    l_0x49: 
      sally_Immediate( ); 
      sally_EOR( ); 
      goto next_inst;  
    
    l_0x4a: 
      sally_LSRA( ); 
      goto next_inst; 
    
    l_0x4c: 
      sally_Absolute( );  
      sally_JMP( ); 
      goto next_inst;

    l_0x4d: 
      sally_Absolute( );  
      sally_EOR( ); 
      goto next_inst;

    l_0x4e: 
      sally_Absolute( );
      sally_LSR( );
      goto next_inst;

    l_0x50: 
      sally_Relative( );
      sally_BVC( );
      goto next_inst;

    l_0x51: 
      sally_IndirectY( ); 
      sally_EOR( ); 
      sally_Delay(sally_y); 
      goto next_inst;      

    l_0x55: 
      sally_ZeroPageX( ); 
      sally_EOR( ); 
      goto next_inst;

    l_0x56: 
      sally_ZeroPageX( ); 
      sally_LSR( ); 
      goto next_inst;

    l_0x58: 
      sally_CLI( );
      goto next_inst;

    l_0x59: 
      sally_AbsoluteY( ); 
      sally_EOR( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0x5d: 
      sally_AbsoluteX( ); 
      sally_EOR( ); 
      sally_Delay(sally_x); 
      goto next_inst;

    l_0x5e: 
      sally_AbsoluteX( ); 
      sally_LSR( ); 
      goto next_inst;

    l_0x60: 
      sally_RTS( );
      goto next_inst;

    l_0x61: 
      sally_IndirectX( ); 
      sally_ADC( ); 
      goto next_inst;

    l_0x65: 
      sally_ZeroPage( );
      sally_ADC( ); 
      goto next_inst;

    l_0x66: 
      sally_ZeroPage( );  
      sally_ROR( ); 
      goto next_inst;

    l_0x68: 
      sally_PLA( );
      goto next_inst;

    l_0x69: 
      sally_Immediate( ); 
      sally_ADC( ); 
      goto next_inst;

    l_0x6a: 
      sally_RORA( ); 
      goto next_inst;

    l_0x6c: 
      sally_Indirect( );
      sally_JMP( ); 
      goto next_inst;

    l_0x6d: 
      sally_Absolute( );
      sally_ADC( ); 
      goto next_inst;
    
    l_0x6e: 
      sally_Absolute( );  
      sally_ROR( ); 
      goto next_inst;

    l_0x70: 
      sally_Relative( );  
      sally_BVS( );
      goto next_inst;

    l_0x71: 
      sally_IndirectY( ); 
      sally_ADC( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0x75: 
      sally_ZeroPageX( ); 
      sally_ADC( ); 
      goto next_inst;

    l_0x76: 
      sally_ZeroPageX( ); 
      sally_ROR( ); 
      goto next_inst;

    l_0x78: 
      sally_SEI( );
      goto next_inst;

    l_0x79: 
      sally_AbsoluteY( ); 
      sally_ADC( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0x7d: 
      sally_AbsoluteX( ); 
      sally_ADC( ); 
      sally_Delay(sally_x); 
      goto next_inst;

    l_0x7e: 
      sally_AbsoluteX( ); 
      sally_ROR( ); 
      goto next_inst;

    l_0x81: 
      sally_IndirectX( ); 
      sally_STA( ); 
      goto next_inst;

    l_0x84: 
      sally_ZeroPage( );  
      sally_STY( ); 
      goto next_inst;

    l_0x85: 
      sally_ZeroPage( );  
      sally_STA( ); 
      goto next_inst;

    l_0x86: 
      sally_ZeroPage( );  
      sally_stx( ); 
      goto next_inst;

    l_0x88: 
      sally_DEY( );
      goto next_inst;

    l_0x8a: 
      sally_TXA( );
      goto next_inst;

    l_0x8c: 
      sally_Absolute( );  
      sally_STY( ); 
      goto next_inst;

    l_0x8d: 
      sally_Absolute( );  
      sally_STA( ); 
      goto next_inst;

    l_0x8e: 
      sally_Absolute( );  
      sally_stx( ); 
      goto next_inst;

    l_0x90: 
      sally_Relative( );
      sally_BCC( );
      goto next_inst;

    l_0x91: 
      sally_IndirectY( ); 
      sally_STA( ); 
      goto next_inst;

    l_0x94: 
      sally_ZeroPageX( ); 
      sally_STY( ); 
      goto next_inst;

    l_0x95: 
      sally_ZeroPageX( ); 
      sally_STA( ); 
      goto next_inst;

    l_0x96: 
      sally_ZeroPageY( ); 
      sally_stx( ); 
      goto next_inst;

    l_0x98: 
      sally_TYA( );
      goto next_inst;

    l_0x99: 
      sally_AbsoluteY( ); 
      sally_STA( ); 
      goto next_inst;

    l_0x9a: 
      sally_TXS( );
      goto next_inst;

    l_0x9d: 
      sally_AbsoluteX( ); 
      sally_STA( ); 
      goto next_inst;

    l_0xa0: 
      sally_Immediate( ); 
      sally_LDY( ); 
      goto next_inst;

    l_0xa1: 
      sally_IndirectX( ); 
      sally_LDA( ); 
      goto next_inst;

    l_0xa2: 
      sally_Immediate( ); 
      sally_LDX( ); 
      goto next_inst;

    l_0xa4: 
      sally_ZeroPage( );  
      sally_LDY( ); 
      goto next_inst;

    l_0xa5: 
      sally_ZeroPage( );  
      sally_LDA( ); 
      goto next_inst;

    l_0xa6: 
      sally_ZeroPage( );  
      sally_LDX( ); 
      goto next_inst;

    l_0xa8: 
      sally_TAY( );
      goto next_inst;

    l_0xa9: 
      sally_Immediate( ); 
      sally_LDA( ); 
      goto next_inst;

    l_0xaa: 
      sally_TAX( );
      goto next_inst;

    l_0xac: 
      sally_Absolute( );  
      sally_LDY( ); 
      goto next_inst;

    l_0xad: 
      sally_Absolute( );  
      sally_LDA( ); 
      goto next_inst;

    l_0xae: 
      sally_Absolute( );  
      sally_LDX( ); 
      goto next_inst;

    l_0xb0: 
      sally_Relative( );  
      sally_BCS( );
      goto next_inst;

    l_0xb1: 
      sally_IndirectY( ); 
      sally_LDA( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xb4: 
      sally_ZeroPageX( ); 
      sally_LDY( ); 
      goto next_inst;

    l_0xb5: 
      sally_ZeroPageX( ); 
      sally_LDA( ); 
      goto next_inst;

    l_0xb6: 
      sally_ZeroPageY( ); 
      sally_LDX( ); 
      goto next_inst;

    l_0xb8: 
      sally_CLV( );
      goto next_inst;

    l_0xb9: 
      sally_AbsoluteY( ); 
      sally_LDA( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xba: 
      sally_TSX( );
      goto next_inst;

    l_0xbc: 
      sally_AbsoluteX( ); 
      sally_LDY( ); 
      sally_Delay(sally_x); 
      goto next_inst;

    l_0xbd: 
      sally_AbsoluteX( ); 
      sally_LDA( ); 
      sally_Delay(sally_x);
      goto next_inst;

    l_0xbe: 
      sally_AbsoluteY( ); 
      sally_LDX( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xc0: 
      sally_Immediate( ); 
      sally_CPY( ); 
      goto next_inst;

    l_0xc1: 
      sally_IndirectX( ); 
      sally_CMP( ); 
      goto next_inst;

    l_0xc4: 
      sally_ZeroPage( );  
      sally_CPY( ); 
      goto next_inst;

    l_0xc5: 
      sally_ZeroPage( );  
      sally_CMP( ); 
      goto next_inst;

    l_0xc6: 
      sally_ZeroPage( );  
      sally_DEC( ); 
      goto next_inst;

    l_0xc8: 
      sally_INY( );
      goto next_inst;

    l_0xc9: 
      sally_Immediate( ); 
      sally_CMP( ); 
      goto next_inst;

    l_0xca: 
      sally_DEX( );
      goto next_inst;

    l_0xcc: 
      sally_Absolute( );  
      sally_CPY( ); 
      goto next_inst;

    l_0xcd: 
      sally_Absolute( );  
      sally_CMP( ); 
      goto next_inst;

    l_0xce: 
      sally_Absolute( );  
      sally_DEC( ); 
      goto next_inst;

    l_0xd0: 
      sally_Relative( );  
      sally_BNE( );
      goto next_inst;          

    l_0xd1: 
      sally_IndirectY( ); 
      sally_CMP( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xd5: 
      sally_ZeroPageX( ); 
      sally_CMP( ); 
      goto next_inst;

    l_0xd6: 
      sally_ZeroPageX( ); 
      sally_DEC( ); 
      goto next_inst;

    l_0xd8: 
      sally_CLD( );
      goto next_inst;

    l_0xd9: 
      sally_AbsoluteY( ); 
      sally_CMP( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xdd: 
      sally_AbsoluteX( ); 
      sally_CMP( ); 
      sally_Delay(sally_x); 
      goto next_inst;

    l_0xde: 
      sally_AbsoluteX( ); 
      sally_DEC( ); 
      goto next_inst;

    l_0xe0: 
      sally_Immediate( ); 
      sally_CPX( ); 
      goto next_inst;

    l_0xe1: 
      sally_IndirectX( ); 
      sally_SBC( ); 
      goto next_inst;

    l_0xe4: 
      sally_ZeroPage( );  
      sally_CPX( ); 
      goto next_inst;

    l_0xe5: 
      sally_ZeroPage( );  
      sally_SBC( ); 
      goto next_inst;

    l_0xe6: 
      sally_ZeroPage( );  
      sally_INC( ); 
      goto next_inst;

    l_0xe8: 
      sally_INX( );
      goto next_inst;

    l_0xe9: 
      sally_Immediate( ); 
      sally_SBC( ); 
      goto next_inst;

    l_0xea:
      sally_NOP( );
      goto next_inst;

    l_0xec: 
      sally_Absolute( );  
      sally_CPX( ); 
      goto next_inst;

    l_0xed: 
      sally_Absolute( );  
      sally_SBC( ); 
      goto next_inst;

    l_0xee: 
      sally_Absolute( );  
      sally_INC( ); 
      goto next_inst;

    l_0xf0: 
      sally_Relative( );
      sally_BEQ( );
      goto next_inst;

    l_0xf1: 
      sally_IndirectY( ); 
      sally_SBC( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xf5: 
      sally_ZeroPageX( ); 
      sally_SBC( ); 
      goto next_inst;

    l_0xf6: 
      sally_ZeroPageX( ); 
      sally_INC( ); 
      goto next_inst;

    l_0xf8: 
      sally_SED( );
      goto next_inst;

    l_0xf9: 
      sally_AbsoluteY( ); 
      sally_SBC( ); 
      sally_Delay(sally_y); 
      goto next_inst;

    l_0xfd: 
      sally_AbsoluteX( ); 
      sally_SBC( ); 
      sally_Delay(sally_x); 
      goto next_inst;

    l_0xfe: 
      sally_AbsoluteX( ); 
      sally_INC( ); 
      goto next_inst;

    l_0x4b:
      sally_Immediate();
      sally_AND();
      sally_LSRA();
      goto next_inst;
    
    l_0x0b:
    l_0x2b:
      sally_Immediate();
      sally_AND();
      if (sally_a & 128) {
        sally_p |= SALLY_FLAG.C;
      }
      else {
        sally_p = (sally_p & ~SALLY_FLAG.C) & 0xFF;
      }    
      goto next_inst;
      
l_0xff:
l_0xfc:
l_0xfb:
l_0xfa:
l_0xf7:
l_0xf4:
l_0xf3:
l_0xf2:
l_0xef:
l_0xeb:
l_0xe7:
l_0xe3:
l_0xe2:
l_0xdf:
l_0xdc:
l_0xdb:
l_0xda:
l_0xd7:
l_0xd4:
l_0xd3:
l_0xd2:
l_0xcf:
l_0xcb:
l_0xc7:
l_0xc3:
l_0xc2:
l_0xbf:
l_0xbb:
l_0xb7:
l_0xb3:
l_0xb2:
l_0xaf:
l_0xab:
l_0xa7:
l_0xa3:
l_0x9f:
l_0x9e:
l_0x9c:
l_0x9b:
l_0x97:
l_0x93:
l_0x92:
l_0x8f:
l_0x8b:
l_0x89:
l_0x87:
l_0x83:
l_0x82:
l_0x80:
l_0x7f:
l_0x7c:
l_0x7b:
l_0x7a:
l_0x77:
l_0x74:
l_0x73:
l_0x72:
l_0x6f:
l_0x6b:
l_0x67:
l_0x64:
l_0x63:
l_0x62:
l_0x5f:
l_0x5c:
l_0x5b:
l_0x5a:
l_0x57:
l_0x54:
l_0x53:
l_0x52:
l_0x4f:
l_0x47:
l_0x44:
l_0x43:
l_0x42:
l_0x3f:
l_0x3c:
l_0x3b:
l_0x3a:
l_0x37:
l_0x34:
l_0x33:
l_0x32:
l_0x2f:
l_0x27:
l_0x23:
l_0x22:
l_0x1f:
l_0x1c:
l_0x1b:
l_0x1a:
l_0x17:
l_0x14:
l_0x13:
l_0x12:
l_0x0f:
l_0x0c:
l_0x07:
l_0x04:
l_0x03:
l_0x02:

next_inst:
    prosystem_cycles += sally_cyclesX4;
    if( half_cycle ) prosystem_cycles += 2;
#ifndef DS_LITE        
    if(riot_timing) 
    {
      riot_UpdateTimer(sally_cyclesX4 >> 2);
    }
    if(memory_ram[WSYNC])   // Will only write true here if cartridge_uses_wsync is true in Memory.c
    {
      prosystem_cycles = 456;
      memory_ram[WSYNC] = false;
      break;
    }
#endif      
  }
}
