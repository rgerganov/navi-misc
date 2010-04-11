{{

 usb-fs-host-debug
──────────────────────────────────────────────────────────────────

Debugging wrapper for usb-fs-host, aka the Poor Man's USB Analyzer.

Logs every function call and every USB transfer to Parallax Serial
Terminal, including hex dumps of all incoming and outgoing buffers.

Usage:
  In the module(s) you want to debug, temporarily replace the
  "usb-fs-host" OBJ reference with "usb-fs-host-debug".

The latest version of this file should always be at:
http://svn.navi.cx/misc/trunk/propeller/usb-host/usb-fs-host-debug.spin

 ┌────────────────────────────────────────────────┐
 │ Copyright (c) 2010 Micah Dowty <micah@navi.cx> │               
 │ See end of file for terms of use.              │
 └────────────────────────────────────────────────┘

}}

OBJ
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"

CON
  ' Port connection status codes
  PORTC_NO_DEVICE  = hc#PORTC_NO_DEVICE
  PORTC_FULL_SPEED = hc#PORTC_FULL_SPEED
  PORTC_LOW_SPEED  = hc#PORTC_LOW_SPEED
  
  ' Standard device requests.

  REQ_CLEAR_DEVICE_FEATURE     = $0100
  REQ_CLEAR_INTERFACE_FEATURE  = $0101
  REQ_CLEAR_ENDPOINT_FEATURE   = $0102
  REQ_GET_CONFIGURATION        = $0880
  REQ_GET_DESCRIPTOR           = $0680
  REQ_GET_INTERFACE            = $0a81
  REQ_GET_DEVICE_STATUS        = $0000
  REQ_GET_INTERFACE_STATUS     = $0001
  REQ_GET_ENDPOINT_STATUS      = $0002
  REQ_SET_ADDRESS              = $0500
  REQ_SET_CONFIGURATION        = $0900
  REQ_SET_DESCRIPTOR           = $0700
  REQ_SET_DEVICE_FEATURE       = $0300
  REQ_SET_INTERFACE_FEATURE    = $0301
  REQ_SET_ENDPOINT_FEATURE     = $0302
  REQ_SET_INTERFACE            = $0b01
  REQ_SYNCH_FRAME              = $0c82

  ' Standard descriptor types.
  
  DESC_DEVICE           = $0100
  DESC_CONFIGURATION    = $0200
  DESC_STRING           = $0300
  DESC_INTERFACE        = $0400
  DESC_ENDPOINT         = $0500

  DESCHDR_DEVICE        = $01_12
  DESCHDR_CONFIGURATION = $02_09
  DESCHDR_INTERFACE     = $04_09
  DESCHDR_ENDPOINT      = $05_07
  
  ' Descriptor Formats

  DEVDESC_bLength             = 0
  DEVDESC_bDescriptorType     = 1
  DEVDESC_bcdUSB              = 2
  DEVDESC_bDeviceClass        = 4
  DEVDESC_bDeviceSubClass     = 5
  DEVDESC_bDeviceProtocol     = 6
  DEVDESC_bMaxPacketSize0     = 7
  DEVDESC_idVendor            = 8
  DEVDESC_idProduct           = 10
  DEVDESC_bcdDevice           = 12
  DEVDESC_iManufacturer       = 14
  DEVDESC_iProduct            = 15
  DEVDESC_iSerialNumber       = 16
  DEVDESC_bNumConfigurations  = 17
  DEVDESC_LEN                 = 18

  CFGDESC_bLength             = 0
  CFGDESC_bDescriptorType     = 1
  CFGDESC_wTotalLength        = 2
  CFGDESC_bNumInterfaces      = 4
  CFGDESC_bConfigurationValue = 5
  CFGDESC_iConfiguration      = 6
  CFGDESC_bmAttributes        = 7
  CFGDESC_MaxPower            = 8

  IFDESC_bLength              = 0
  IFDESC_bDescriptorType      = 1
  IFDESC_bInterfaceNumber     = 2
  IFDESC_bAlternateSetting    = 3
  IFDESC_bNumEndpoints        = 4
  IFDESC_bInterfaceClass      = 5
  IFDESC_bInterfaceSubClass   = 6
  IFDESC_bInterfaceProtocol   = 7
  IFDESC_iInterface           = 8

  EPDESC_bLength              = 0
  EPDESC_bDescriptorType      = 1
  EPDESC_bEndpointAddress     = 2
  EPDESC_bmAttributes         = 3
  EPDESC_wMaxPacketSize       = 4
  EPDESC_bInterval            = 6

  ' SETUP packet format

  SETUP_bmRequestType         = 0
  SETUP_bRequest              = 1
  SETUP_wValue                = 2
  SETUP_wIndex                = 4
  SETUP_wLength               = 6
  SETUP_LEN                   = 8

  ' Endpoint constants

  DIR_IN       = $80
  DIR_OUT      = $00

  TT_CONTROL   = $00
  TT_ISOC      = $01
  TT_BULK      = $02
  TT_INTERRUPT = $03
                
  ' Error codes

  E_SUCCESS       = 0

  E_NO_DEVICE     = -150        ' No device is attached
  E_LOW_SPEED     = -151        ' Low-speed devices are not supported
  
  E_TIMEOUT       = -160        ' Timed out waiting for a response
  E_TRANSFER      = -161        ' Generic low-level transfer error
  E_CRC           = -162        ' CRC-16 mismatch
  E_TOGGLE        = -163        ' DATA0/1 toggle error
  E_PID           = -164        ' Invalid or malformed PID
  E_STALL         = -165        ' USB STALL response (pipe error)
  
  E_OUT_OF_COGS   = -180        ' Not enough free cogs, can't initialize
  E_OUT_OF_MEM    = -181        ' Not enough space for the requested buffer sizes
  E_DESC_PARSE    = -182        ' Can't parse a USB descriptor

PUB Enumerate
  term.start(115200)

  term.str(string(term#NL, term#NL, "** [Enumerate] -> "))
  result := \hc.Enumerate
  term.dec(result)
  if result < 0
    abort
  
PUB Configure
  term.str(string(term#NL, term#NL, "** [Configure] -> "))
  result := \hc.Configure
  term.dec(result)
  if result < 0
    abort

PUB ClearHalt(epd)
  term.str(string(term#NL, term#NL, "** [ClearHalt] ("))
  term.hex(epd,2)
  term.str(string(") -> "))
  result := \hc.ClearHalt(epd)
  term.dec(result)
  if result < 0
    abort

PUB DeviceReset
  term.str(string(term#NL, term#NL, "** [DevReset ] -> "))
  result := \hc.DeviceReset
  term.dec(result)
  if result < 0
    abort

PUB DeviceAddress
  term.str(string(term#NL, term#NL, "** [DevAddr  ] -> "))
  result := \hc.DeviceAddress
  term.dec(result)
  if result < 0
    abort

PUB Control(req, value, index)
  term.str(string(term#NL, term#NL, "** [Control  ] ("))
  term.hex(req, 4)
  term.str(string(", "))
  term.hex(value, 4)
  term.str(string(", "))
  term.hex(index, 4)
  term.str(string(") -> "))
  result := \hc.Control(req, value, index)
  term.dec(result)
  if result < 0
    abort

PUB ControlRead(req, value, index, bufferPtr, length)
  term.str(string(term#NL, term#NL, "** [ControlRd] ("))
  term.hex(req, 4)
  term.str(string(", "))
  term.hex(value, 4)
  term.str(string(", "))
  term.hex(index, 4)
  term.str(string(") -> "))
  result := \hc.ControlRead(req, value, index, bufferPtr, length)
  term.dec(result)
  if result < 0
    abort
  hexDump(bufferPtr, length)

PUB InterruptRead(epd, buffer, length) : actual
  term.str(string(term#NL, term#NL, "** [Intr   Rd] ("))
  term.hex(epd, 4)
  term.str(string(", "))
  term.hex(buffer, 4)
  term.str(string(", "))
  term.hex(length, 4)
  term.str(string(") -> "))
  result := \hc.InterruptRead(epd, buffer, length)
  term.dec(result)
  if result < 0
    abort
  hexDump(buffer, length)

PUB BulkWrite(epd, buffer, length)
  term.str(string(term#NL, term#NL, "** [Bulk   Wr] ("))
  term.hex(epd, 4)
  term.str(string(", "))
  term.hex(buffer, 4)
  term.str(string(", "))
  term.hex(length, 4)
  term.str(string(") -> "))
  result := \hc.BulkWrite(epd, buffer, length)
  term.dec(result)
  hexDump(buffer, length)
  if result < 0
    abort
     
PUB BulkRead(epd, buffer, length) : actual
  term.str(string(term#NL, term#NL, "** [Bulk   Rd] ("))
  term.hex(epd, 4)
  term.str(string(", "))
  term.hex(buffer, 4)
  term.str(string(", "))
  term.hex(length, 4)
  term.str(string(") -> "))
  result := \hc.BulkRead(epd, buffer, length)
  term.dec(result)
  if result < 0
    abort
  hexDump(buffer, length)

PUB DeviceDescriptor
  return hc.DeviceDescriptor
PUB ConfigDescriptor
  return hc.ConfigDescriptor
PUB VendorID
  return hc.VendorID  
PUB ProductID
  return hc.ProductID
PUB NextDescriptor(ptrIn)
  return hc.NextDescriptor(ptrIn)
PUB NextHeaderMatch(ptrIn, header)
  return hc.NextHeaderMatch(ptrIn, header)
PUB FirstInterface
  return hc.FirstInterface
PUB NextInterface(curIf)
  return hc.NextInterface(curIf)
PUB NextEndpoint(curIf)
  return hc.NextEndpoint(curIf)
PUB FindInterface(class)
  return hc.FindInterface(class)
PUB EndpointDirection(epd)
  return hc.EndpointDirection(epd)
PUB EndpointType(epd)
  return hc.EndpointType(epd)
PUB UWORD(addr)
  return hc.UWORD(addr)

PRI hexDump(buffer, bytes) | x, y, b
  ' A basic 16-byte-wide hex/ascii dump

  repeat y from 0 to ((bytes + 15) >> 4) - 1
    term.char(term#NL)
    term.hex(y << 4, 4)
    term.str(string(": "))

    repeat x from 0 to 15
      term.hex(BYTE[buffer + x + (y<<4)], 2)
      term.char(" ")

    term.char(" ")

    repeat x from 0 to 15
      b := BYTE[buffer + x + (y<<4)]
      case b
        32..126:
          term.char(b)
        other:
          term.char(".")


DAT
{{

┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                   TERMS OF USE: MIT License                                                  │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}