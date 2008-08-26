{

Access control example using rfid-lf
────────────────────────────────────

This is a functioning example for the rfid-lf module.
It scans for RFID cards that match a list of accepted codes.
When one is found, we activate a relay momentarily.
A bi-color LED indicates when access is allowed or denied.

See the rfid-lf module for the RFID reader schematic.

Other parts include a reed relay (don't forget the protection
diode) and a bi-color LED with two current limiting resistors.

There is also a TV output on pin 12, for debugging.

Micah Dowty <micah@navi.cx>

 ┌───────────────────────────────────┐
 │ Copyright (c) 2008 Micah Dowty    │               
 │ See end of file for terms of use. │
 └───────────────────────────────────┘

}

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  TV_PIN           = 12
  RED_LED_PIN      = 23 ' Active high (bi-color LED)
  GREEN_LED_PIN    = 22
  RELAY_PIN        = 17 ' Active low

OBJ
  debug : "TV_Text"
  rfid  : "rfid-lf"

VAR
  long  buffer[128]
  
PUB main | i
  debug.start(TV_PIN)
  hardwareInit
  rfid.start

  repeat
    debug.out(1)
    rfid.read(@buffer)
    repeat i from 0 to 31
      debug.hex(buffer[i], 8)
      debug.out(" ")
        

PRI hardwareInit
  LED_Off
  Relay_Off
  dira[RED_LED_PIN]~~
  dira[GREEN_LED_PIN]~~
  dira[RELAY_PIN]~~

PRI LED_Red
  outa[RED_LED_PIN]~~
  outa[GREEN_LED_PIN]~

PRI LED_Green
  outa[RED_LED_PIN]~
  outa[GREEN_LED_PIN]~~

PRI LED_Yellow
  outa[RED_LED_PIN]~~
  outa[GREEN_LED_PIN]~~

PRI LED_Off
  outa[RED_LED_PIN]~
  outa[GREEN_LED_PIN]~

PRI Relay_On
  outa[RELAY_PIN]~

PRI Relay_Off
  outa[RELAY_PIN]~~
  