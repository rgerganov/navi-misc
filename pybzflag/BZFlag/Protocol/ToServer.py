""" BZFlag.Protocol.FromServer

Low-level python abstractions for the BZFlag protocol messages
sent from the game server to the clients.
"""
# 
# Python BZFlag Protocol Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
# 
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#  
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
#

from BZFlag.Protocol import *
from BZFlag.Protocol import Common


class MsgExit(Common.Message):
    messageId = 0x6578

class MsgEnter(Common.Message):
    messageId = 0x656E
    entries = [
        StructEntry(UInt16,             'playerType'),
        StructEntry(UInt16,             'team'),
        StructEntry(ScalarType("32s"),  'callSign'),
        StructEntry(ScalarType("128s"), 'emailAddress'),
        ]


messages = [MsgExit, MsgEnter]
    
### The End ###
        
    
