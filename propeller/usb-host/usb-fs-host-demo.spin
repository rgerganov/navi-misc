
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000

OBJ
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"
  
VAR

PUB main : value
  term.Start(115200)
  hc.Start
      
  repeat
    waitcnt(cnt + clkfreq/2)
        
    value := hc.PacketTXRX

    term.str(string(term#NL))
    term.bin(LONG[value+12], 32)
    term.str(string(" "))
    term.bin(LONG[value+8], 32)
    term.str(string(" "))
    term.bin(LONG[value+4], 32)
    term.str(string(" "))
    term.bin(LONG[value], 32)
    