""" Wasabi.Settings

Implements Wasabi's settings menu and its submenus.
"""
#
# Wasabi Project
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

from Wasabi import Menu, Icon


class SettingsMenu(Menu.ArcMenu):
    def __init__(self, book):
        items = [
            Menu.PageItem(Icon.load('background'), Menu.defaultFades(BackgroundsMenu)),
            ]
        Menu.ArcMenu.__init__(self, book, items, "Settings")


class BackgroundsMenu(Menu.ArcMenu):
    def __init__(self, book):
        items = []

        Menu.ArcMenu.__init__(self, book, items, "Backgrounds")

### The End ###
