  ; A78 Header v4.2
  ; 
  ; Use this file to add an a78 header via the source code of your ROM.
  ;
  ; _Implementation Notes_
  ;
  ; * Include this header near the beginning of your DASM source, but after
  ;   your initial ROM ORG statement.
  ; * Change the fields withn the file to describe your game's hardware
  ;   requirements to emulators and flash carts.
  ; * All unused/reserved bits and bytes must be set to zero.

.ROMSIZE = $20000                  ; Update with your total ROM size.

  ; Uncomment the following entry to force older v3 style headers.
;V3ONLY = 1

  ; Uncomment the following entry to break support with platforms that only
  ; have v3 headers implemented.
;V4ONLY = 1

  ; Auto-header ROM allocation follows. If the current address is page aligned,
  ; we backup 128 bytes. This may cause issues if you use a different ORG+RORG
  ; at the start of your ROM - in that case, account for the 128 bytes of
  ; header within your game ROM start ORG+RORG statements.

    if ( . & $FF ) = 0             ; Check if we're at an even page.
        ORG  (. - 128),0           ; If so, go -128 bytes, for header space.
    else
        ORG .,0                    ; In case zero-fill wasn't specified
    endif                          ; orginally.

    SEG     ROM

.HEADER = .

  ; Format detection - do not modify.
    #ifconst V3ONLY
    DC.B    3                  ; 0          header major version
    #else
    DC.B    4                  ; 0          header major version
    #endif ; V3ONLY
    DC.B    "ATARI7800"        ; 1..16      header magic string - zero pad


    ORG .HEADER+$11,0
    DC.B    "Game Name Here"   ; 17..48     cartridge title string - zero pad


    ORG .HEADER+$31,0
    DC.B    (.ROMSIZE>>24)     ; 49..52     cartridge ROM size
    DC.B    (.ROMSIZE>>16&$FF)
    DC.B    (.ROMSIZE>>8&$FF)
    DC.B    (.ROMSIZE&$FF)

    #ifnconst V4ONLY

    ; The following 2 cartridge type bytes are deprecated as of header v4.0.
    ; It's recommended that you still populate these bytes for support with
    ; platforms that don't yet support v4.

    DC.B    %00000000          ; 53         cartridge type A
    DC.B    %00000000          ; 54         cartridge type B
    ; _Cartridge Type A_
    ;    bit 7 ; POKEY @ $0800 - $080F
    ;    bit 6 ; EXRAM/M2                   (halt banked RAM)
    ;    bit 5 ; BANKSET
    ;    bit 4 ; SOUPER
    ;    bit 3 ; YM2151 @ $0460 - $0461 
    ;    bit 2 ; POKEY @ $0440 - $044F 
    ;    bit 1 ; ABSOLUTE
    ;    bit 0 ; ACTIVISION
    ; _Cartridge Type B_
    ;    bit 7 ; EXRAM/A8                   (mirror RAM)
    ;    bit 6 ; POKEY @ $0450 - $045F 
    ;    bit 5 ; EXRAM/X2                   (hotspot banked RAM)
    ;    bit 4 ; EXFIX                      (2nd last bank @ $4000)
    ;    bit 3 ; EXROM                      (ROM @ $4000)
    ;    bit 2 ; EXRAM                      (RAM @ $4000)
    ;    bit 1 ; SUPERGAME
    ;    bit 0 ; POKEY @ $4000 - $7FFF

    #else
    DC.B    %11111111
    DC.B    %11111111
    #endif ; !V4ONLY

    DC.B    1                  ; 55         controller 1 device type
    DC.B    1                  ; 56         controller 2 device type
    ;    0 = none
    ;    1 = 7800 joystick
    ;    2 = lightgun
    ;    3 = paddle
    ;    4 = trakball
    ;    5 = 2600 joystick
    ;    6 = 2600 driving
    ;    7 = 2600 keypad
    ;    8 = ST mouse
    ;    9 = Amiga mouse
    ;   10 = AtariVox
    ;   11 = SNES2Atari
    ;   12 = Mega7800


    DC.B    %00000000          ; 57         tv type
    ;    bits 7..3 ; reserved
    ;    bit  2    ; 0:single-region,1:multi-region
    ;    bit  1    ; 0:component,1:composite
    ;    bit  0    ; 0:NTSC,1:PAL


    DC.B    %00000000          ; 58         save peripheral
    ;    bits 7..2 ; reserved
    ;    bit  1    ; SaveKey/AtariVox
    ;    bit  0    ; High Score Cart (HSC)


    ; The following irq source byte is deprecated as of header v4.0.
    ; It's recommended that you still populate this byte for support with
    ; platforms that don't yet support v4.

    ORG     .HEADER+62,0
    DC.B    %00000000          ; 62         external irq source
    ;    bits 7..5 ; reserved
    ;    bit  4    ; POKEY  @ $0800 - $080F
    ;    bit  3    ; YM2151 @ $0460 - $0461
    ;    bit  2    ; POKEY  @ $0440 - $044F
    ;    bit  1    ; POKEY  @ $0450 - $045F
    ;    bit  0    ; POKEY  @ $4000 - $7FFF

    DC.B    %00000000          ; 63         slot passthrough device
    ;    bits 7..1 ; reserved
    ;    bit  0    ; XM module

    #ifnconst V3ONLY

    ; The following 6 bytes are v4 header specific. You should populate
    ; them with valid info if you're not using V3ONLY, because they will
    ; take precedence over v3 headers.

    DC.B    0                  ; 64         mapper
    ;    0 = linear
    ;    1 = supergame
    ;    2 = activision
    ;    3 = absolute
    ;    4 = souper


    DC.B    0                  ; 65         mapper options
    ; linear_
    ;    bit  7      ; bankset rom
    ;    bits 0-1    ; option @4000...
    ;       0 = none
    ;       1 = 16K EXRAM
    ;       2 = 8K  EXRAM/A8
    ;       3 = 32K EXRAM/M2
    ; supergame_
    ;    bit  7      ; bankset rom
    ;    bits 0-2    ; option @4000...
    ;       0 = none
    ;       1 = 16K EXRAM
    ;       2 = 8K  EXRAM/A8
    ;       3 = 32K EXRAM/M2
    ;       4 = 16K EXROM
    ;       5 = EXFIX
    ;       6 = 32K EXRAM/X2

    DC.B    %00000000          ; 66         audio hi
    DC.B    %00000000          ; 67         audio lo
    ;    bit  5      ; adpcm@420
    ;    bit  4      ; covox@430
    ;    bit  3      ; ym2151@460
    ;    bits 0-2    ; pokey...
    ;       0 = none
    ;       1 = pokey@440
    ;       2 = pokey@450
    ;       3 = dual pokey @440+@450
    ;       4 = pokey@800
    ;       5 = pokey@4000

    DC.B    %00000000          ; 68         interrupt hi
    DC.B    %00000000          ; 69         interrupt lo
    ;    bit  2    ; YM2151
    ;    bit  1    ; pokey 2 (@440)
    ;    bit  0    ; pokey 1 (@4000, @450, or @800)

    #endif ; !V3ONLY


    ORG     .HEADER+100,0       ; 100..127  footer magic string
    DC.B    "ACTUAL CART DATA STARTS HERE"

