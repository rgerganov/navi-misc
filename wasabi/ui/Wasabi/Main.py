""" Wasabi.Main

Top-level initialization of the Wasabi UI. This starts up the PyBZEngine and
hardware components, then uses Sequencer to start the menu.
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

import os, pygame
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, Input
from BZEngine import Event
from Wasabi import Hardware, Logos, Menu, IR, Icon, VideoSwitch


class Main:
    """Initializes other components and store references to them, runs the main loop"""
    def __init__(self):
        self.loop = Event.EventLoop()

        # Detect hardware
        self.hardware = Hardware.Devices(self.loop)

        # An nvidia-widget to sync to vertical blank, to avoid tearing. Should
        # have no effect on platforms where this isn't supported.
        os.environ['__GL_SYNC_TO_VBLANK'] = '1'

        # If we're running on actual wasabi hardware, make sure we use the local X display.
        # (Just a convenience, since this will generally be run over ssh for testing)
        if self.hardware.uvswitch:
            os.environ['DISPLAY'] = ':0'

        self.viewport = Viewport.OpenGLViewport(self.loop)
        self.viewport.setCaption("Wasabi UI")
        self.view = ThreeDRender.View(self.viewport)

        # If we're running on actual wasabi hardware, go fullscreen
        if self.hardware.uvswitch:
            self.viewport.display.toggle_fullscreen()
            pygame.mouse.set_visible(0)
        else:
            # Otherwise, set up a ThreeDControl to make debugging easier
            self.control = ThreeDControl.Viewing(self.view, self.viewport)

        # Set up our main book
        self.book = Sequencer.CyclicBook(self.view, self.getPages())

    def getPages(self):
        """Return a list of pages to include in the main sequencer book"""
        return [
            # Display the logo sequence until user interruption
            Sequencer.FadeOut(0.2, (1,1,1), userPageInterrupter(Logos.getLogoSubBook())),

            # Run the main menu, with fade in and out
            Sequencer.FadeIn(0.5, (1,1,1), Sequencer.FadeOut(0.25, (0,0,0), lambda book:
                                                             MainMenu(book, self.hardware)))

            # The main menu will insert additional pages at the end,
            # depending on the selection made.
            ]

    def run(self):
        """Run the main loop, doesn't return until the program exits"""
        self.loop.run()


def userPageInterrupter(page):
    """A wrapper around the book of logos that runs them until any IR code
       is received, the mouse is clicked, or the space/enter keys are pressed.
       """
    def f(book):
        events = [
            Input.KeyPress(book.viewport, pygame.K_SPACE),
            Input.KeyPress(book.viewport, pygame.K_RETURN),
            Input.MousePress(book.viewport, 1),
            IR.ButtonPress(),
            ]
        return Sequencer.PageInterrupter(events, page)(book)
    return f


class VideoChannelPage(Sequencer.Page):
    """A sequencer page that displays video from an external input, via uvswitch"""
    def __init__(self, view, hardware, channel):
        Sequencer.Page.__init__(self, view)
        self.hardware = hardware
        hardware.selectDirectVideo(channel)

    def finalize(self):
        self.hardware.selectWasabiVideo()


class VideoInput(Menu.Item):
    """A menu item representing a video input. This is added to the MainMenu whenever
       it becomes active as specified by the video switch.
       """
    def __init__(self, channel):
        self.channel = channel
        Menu.Item.__init__(self, VideoSwitch.getInputDict()[channel])

    def onSelected(self, menu):
        """When we're selected, add a page to the current book that will show
           video from our channel until a key is pressed.
           """
        menu.book.pushBack(userPageInterrupter(
            lambda book: VideoChannelPage(book, menu.hardware, self.channel)))


class MainMenu(Menu.RingMenu):
    """A RingMenu subclass that displays available devices, and lets the user select one.
       Devices can be made available and unavailable during the operation of the menu, with
       the menu properly reflecting this.
       """
    def __init__(self, book, hardware):
        self.hardware = hardware

        # Add items that always appear on the menu
        menuItems = [
            Menu.Item(Icon.load('navi')),
            ]

        # If we have a video switch device, integrate it with the main menu
        if self.hardware.uvswitch:

            # Add items for video devices that are already active
            self.channelItems = {}
            for channel in self.hardware.uvswitch.activeChannels:
                item = VideoInput(channel)
                self.channelItems[channel] = item
                menuItems.append(item)

                # Observe the video detection events, so icons can be dynamically added and removed
                self.hardware.uvswitch.onChannelActive.observe(self.addChannel)
                self.hardware.uvswitch.onChannelInactive.observe(self.removeChannel)

        Menu.RingMenu.__init__(self, book, menuItems)

    def addChannel(self, channel):
        """Observing the uvswitch onChannelActive event, this is called when a new
           channel becomes active, to add an icon to the menu.
           """
        item = VideoInput(channel)
        self.channelItems[channel] = item
        self.add(item)

    def removeChannel(self, channel):
        """Observing the uvswitch onChannelInactive event, this is called when a new
           channel becomes inactive, to remove its icon from the menu.
           """
        item = self.channelItems[channel]
        del self.channelItems[channel]
        self.remove(item)

### The End ###
