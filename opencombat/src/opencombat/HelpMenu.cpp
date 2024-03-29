/* bzflag
 * Copyright (c) 1993 - 2004 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* interface header */
#include "HelpMenu.h"

/* system implementation headers */
#include <vector>
#include <string>

/* common implementation headers */
#include "KeyManager.h"
#include "Flag.h"
#include "FontManager.h"

/* local implementation headers */
#include "HUDui.h"
#include "HUDDialog.h"
#include "HUDDialogStack.h"
#include "MainMenu.h"


bool HelpMenuDefaultKey::keyPress(const BzfKeyEvent& key)
{
  if (key.button == BzfKeyEvent::PageUp) {
    HUDDialog* oldMenu = HUDDialogStack::get()->top();
    HUDDialogStack::get()->pop();
    HUDDialogStack::get()->push(HelpMenu::getHelpMenu(oldMenu, false));
    return true;
  }
  if (key.button == BzfKeyEvent::PageDown || key.ascii == 13) {
    HUDDialog* oldMenu = HUDDialogStack::get()->top();
    HUDDialogStack::get()->pop();
    HUDDialogStack::get()->push(HelpMenu::getHelpMenu(oldMenu, true));
    return true;
  }
  return MenuDefaultKey::keyPress(key);
}

bool HelpMenuDefaultKey::keyRelease(const BzfKeyEvent& key)
{
  if (key.button == BzfKeyEvent::PageUp ||
      key.button == BzfKeyEvent::PageDown || key.ascii == 13)
    return true;
  return MenuDefaultKey::keyRelease(key);
}


HelpMenu::HelpMenu(const char* title) : HUDDialog()
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel(title));
  list.push_back(createLabel("Page Down for next page",
			     "Page Up for previous page"));


  initNavigation(list, 1, 1);
}

HUDuiControl* HelpMenu::createLabel(const char* string,
				    const char* label)
{
  HUDuiLabel* control = new HUDuiLabel;
  control->setFontFace(MainMenu::getFontFace());
  if (string) control->setString(string);
  if (label) control->setLabel(label);
  return control;
}

float HelpMenu::getLeftSide(int, int height)
{
  return (float)height / 6.0f;
}

void HelpMenu::resize(int width, int height)
{
  HUDDialog::resize(width, height);

  // use a big font for title, smaller font for the rest
  const float titleFontSize = (float)height / 23.0f;
  const float fontSize = (float)height / 72.0f;
  FontManager &fm = FontManager::instance();

  // reposition title
  std::vector<HUDuiControl*>& list = getControls();
  HUDuiLabel* title = (HUDuiLabel*)list[0];
  title->setFontSize(titleFontSize);
  const float titleWidth = fm.getStrLength(MainMenu::getFontFace(), titleFontSize, title->getString());
  const float titleHeight = fm.getStrHeight(MainMenu::getFontFace(), titleFontSize, " ");
  float x = 0.5f * ((float)width - titleWidth);
  float y = (float)height - titleHeight;
  title->setPosition(x, y);

  // position focus holder off screen
  list[1]->setFontSize(fontSize);
  const float h = fm.getStrHeight(MainMenu::getFontFace(), fontSize, " ");
  y -= 1.25f * h;
  list[1]->setPosition(0.5f * ((float)width + h), y);

  // reposition options
  x = getLeftSide(width, height);
  y -= 1.5f * h;
  const int count = list.size();
  for (int i = 2; i < count; i++) {
    list[i]->setFontSize(fontSize);
    list[i]->setPosition(x, y);
    y -= 1.0f * h;
  }
}

//
// Help1Menu
//

class Help1Menu : public HelpMenu {
public:
  Help1Menu();
  ~Help1Menu() { }

  void		resize(int width, int height);

  void		onScan(const std::string& name, bool, const std::string&);
  static void		onScanCB(const std::string& name, bool press,
				 const std::string& cmd, void* userData);

protected:
  float		getLeftSide(int width, int height);

private:
  void		initKeymap(const std::string& name, int index);
  struct keymap {
    int index;	// ui label index
    std::string key1;
    std::string key2;
  };
  typedef std::map<std::string, keymap> KeyKeyMap;
  KeyKeyMap	mappable;
};

Help1Menu::Help1Menu() : HelpMenu("Controls")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("Mouse Position", "Controls Tank Position:"));
  list.push_back(createLabel(NULL, "Fires Shot:"));
  list.push_back(createLabel(NULL, "Drops Flag (if not bad):"));
  list.push_back(createLabel(NULL, "Identifies Player (locks on GM):"));
  list.push_back(createLabel(NULL, "Short Radar Range:"));
  list.push_back(createLabel(NULL, "Medium Radar Range:"));
  list.push_back(createLabel(NULL, "Long Radar Range:"));
  list.push_back(createLabel(NULL, "Send Message to Everybody:"));
  list.push_back(createLabel(NULL, "Send Message to Teammates:"));
  list.push_back(createLabel(NULL, "Send Message to Nemesis:"));
  list.push_back(createLabel(NULL, "Send Message to Recipient:"));
  list.push_back(createLabel(NULL, "Jump (if allowed):"));
  list.push_back(createLabel(NULL, "Toggle Binoculars:"));
  list.push_back(createLabel(NULL, "Toggle Score Sheet:"));
  list.push_back(createLabel(NULL, "Toggle Tank Labels:"));
  list.push_back(createLabel(NULL, "Toggle Heads-up Flag Help:"));
  list.push_back(createLabel(NULL, "Set Time of Day Backward:"));
  list.push_back(createLabel(NULL, "Set Time of Day Forward:"));
  list.push_back(createLabel(NULL, "Pause/Resume:"));
  list.push_back(createLabel(NULL, "Self destruct/Cancel:"));
  list.push_back(createLabel(NULL, "Quit:"));
  list.push_back(createLabel(NULL, "Scroll Message Log Backward:"));
  list.push_back(createLabel(NULL, "Scroll Message Log Forward:"));
  list.push_back(createLabel(NULL, "Slow Keyboard Motion:"));
  list.push_back(createLabel(NULL, "Toggle Radar Flags:"));
  list.push_back(createLabel(NULL, "Toggle Main Flags:"));
  list.push_back(createLabel(NULL, "Silence/UnSilence:"));
  list.push_back(createLabel(NULL, "Server Admin:"));
  list.push_back(createLabel(NULL, "Hunt:"));
  list.push_back(createLabel(NULL, "Auto Pilot:"));
  list.push_back(createLabel("Esc", "Show/Dismiss menu:"));

  initKeymap("fire", 3);
  initKeymap("drop", 4);
  initKeymap("identify", 5);
  initKeymap("set displayRadarRange 0.25", 6);
  initKeymap("set displayRadarRange 0.5", 7);
  initKeymap("set displayRadarRange 1.0", 8);
  initKeymap("send all", 9);
  initKeymap("send team", 10);
  initKeymap("send nemesis", 11);
  initKeymap("send recipient", 12);
  initKeymap("jump", 13);
  initKeymap("toggle displayBinoculars", 14);
  initKeymap("toggle displayScore", 15);
  initKeymap("toggle displayLabels", 16);
  initKeymap("toggle displayFlagHelp", 17);
  initKeymap("time backward", 18);
  initKeymap("time forward", 19);
  initKeymap("pause", 20);
  initKeymap("destruct", 21);
  initKeymap("quit", 22);
  initKeymap("scrollpanel up", 23);
  initKeymap("scrollpanel down", 24);
  initKeymap("toggle slowKeyboard", 25);
  initKeymap("toggle displayRadarFlags", 26);
  initKeymap("toggle displayMainFlags", 27);
  initKeymap("silence", 28);
  initKeymap("servercommand", 29);
  initKeymap("hunt", 30);
  initKeymap("autopilot", 31);
}

void Help1Menu::onScan(const std::string& name, bool press,
		       const std::string& cmd)
{
  if (!press)
    return;
  KeyKeyMap::iterator it = mappable.find(cmd);
  if (it == mappable.end())
    return;
  if (it->second.key1.empty())
    it->second.key1 = name;
  else if (it->second.key2.empty())
    it->second.key2 = name;
}

void Help1Menu::onScanCB(const std::string& name, bool press,
			 const std::string& cmd, void* userData)
{
  reinterpret_cast<Help1Menu*>(userData)->onScan(name, press, cmd);
}

void Help1Menu::initKeymap(const std::string& name, int index)
{
  mappable[name].key1 = "";
  mappable[name].key2 = "";
  mappable[name].index = index;
}

float Help1Menu::getLeftSide(int width, int height)
{
  return 0.5f * width - height / 20.0f;
}

void Help1Menu::resize(int width, int height)
{
  // get current key mapping and set strings appropriately
  KeyKeyMap::iterator it;
  // clear
  for (it = mappable.begin(); it != mappable.end(); it++) {
    it->second.key1 = "";
    it->second.key2 = "";
  }
  // load current settings
  KEYMGR.iterate(&onScanCB, this);
  std::vector<HUDuiControl*>& list = getControls();
  for (it = mappable.begin(); it != mappable.end(); it++) {
    std::string value = "";
    if (it->second.key1.empty()) {
      value = "<not mapped>";
    } else {
      value += it->second.key1;
      if (!it->second.key2.empty())
	value += " or " + it->second.key2;
    }
    ((HUDuiLabel*)list[it->second.index])->setString(value);
  }

  // now do regular resizing
  HelpMenu::resize(width, height);
}

//
// Help2Menu
//

class Help2Menu : public HelpMenu {
public:
  Help2Menu();
  ~Help2Menu() { }
};

Help2Menu::Help2Menu() : HelpMenu("General")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("BZFlag is a multi-player networked tank battle game.  There are five teams:"));
  list.push_back(createLabel("red, green, blue, purple, and rogues (rogue tanks are black).  Destroying a"));
  list.push_back(createLabel("player on another team scores a win, while being destroyed or destroying a"));
  list.push_back(createLabel("teammate scores a loss.  Individual and aggregate team scores are tallied."));
  list.push_back(createLabel("Rogues have no teammates (not even other rogues),so they cannot shoot"));
  list.push_back(createLabel("teammates and they don't have a team score."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("There are three styles of play, determined by the server configuration:  capture-"));
  list.push_back(createLabel("the-flag, rabbit-chase and free-for-all.  In free-for-all the object is simply to get the"));
  list.push_back(createLabel("highest score by shooting opponents.  The object in rabbit chase is to be the highest score"));
  list.push_back(createLabel("so that you have the white tank, then everyone is against you. The object in capture-the-flag is to"));
  list.push_back(createLabel("capture enemy flags while preventing opponents from capturing yours.  In this"));
  list.push_back(createLabel("style, each team (but not rogues) has a team base and each team with at least"));
  list.push_back(createLabel("one player has a team flag which has the color of the team.  To capture a flag,"));
  list.push_back(createLabel("you must grab it and bring it back to your team base (you must be on the ground"));
  list.push_back(createLabel("in your base to register the capture).  Capturing a flag destroys all the players"));
  list.push_back(createLabel("on that team and gives your team score a bonus;  the players will restart on"));
  list.push_back(createLabel("their team base.  Taking your flag onto an enemy base counts as a capture against"));
  list.push_back(createLabel("your team but not for the enemy team."));
}

//
// Help3Menu
//

class Help3Menu : public HelpMenu {
public:
  Help3Menu();
  ~Help3Menu() { }
};

Help3Menu::Help3Menu() : HelpMenu("Environment")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("The world environment contains an outer wall and several buildings."));
  list.push_back(createLabel("You cannot go outside the outer wall (you can't even jump over it)."));
  list.push_back(createLabel("You cannot normally drive or shoot through buildings."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("The server may be configured to include teleporters:  large transparent"));
  list.push_back(createLabel("black slabs.  Objects entering one side of a teleporter are instantly"));
  list.push_back(createLabel("moved to one side of another (or possibly the same) teleporter.  The"));
  list.push_back(createLabel("teleport is reversible;  reentering the same side of the destination"));
  list.push_back(createLabel("teleporter brings you back to where you started.  Teleport connections"));
  list.push_back(createLabel("are fixed at the start of the game and don't change during the game."));
  list.push_back(createLabel("The connections are always the same in the capture-the-flag style."));
  list.push_back(createLabel("Each side of a teleporter teleports independently of the other side."));
  list.push_back(createLabel("It's possible for a teleporter to teleport to the opposite side of"));
  list.push_back(createLabel("itself.  Such a thru-teleporter acts almost as if it wasn't there."));
  list.push_back(createLabel("A teleporter can also teleport to the same side of itself.  This is a"));
  list.push_back(createLabel("reverse teleporter.  Shooting at a reverse teleporter is likely to be"));
  list.push_back(createLabel("self destructive;  shooting a laser at one is invariably fatal."));
}

//
// Help4Menu
//

class Help4Menu : public HelpMenu {
public:
  Help4Menu();
  ~Help4Menu() { }
};

Help4Menu::Help4Menu() : HelpMenu("Flags")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("Flags come in two varieties:  team flags and super flags.  Team flags"));
  list.push_back(createLabel("are used only in the capture-the-flag style.  The server may also be"));
  list.push_back(createLabel("configured to supply super flags, which give your tank some advantage"));
  list.push_back(createLabel("or disadvantage.  You normally can't tell which until you pick one up,"));
  list.push_back(createLabel("but good flags generally outnumber bad flags two to one."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("Team flags are not allowed to be in Bad Places.  Bad Places are:  on"));
  list.push_back(createLabel("a building or on an enemy base.  Team flags dropped in a Bad Place are"));
  list.push_back(createLabel("moved to a safety position.  Captured flags are placed back on their"));
  list.push_back(createLabel("team base.  Super flags dropped above a building always disappear."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("A random good super flag will remain for up to 4 possessions.  After"));
  list.push_back(createLabel("that it'll disappear and will eventually be replaced by a new random"));
  list.push_back(createLabel("flag.  Bad random super flags disappear after the first possession."));
  list.push_back(createLabel("Bad super flags can't normally be dropped.  The server can be set to"));
  list.push_back(createLabel("automatically drop the flag for you after some time, after you destroy"));
  list.push_back(createLabel("a certain number of enemies, and/or when you grab an antidote flag."));
  list.push_back(createLabel("Antidote flags are yellow and only appear when you have a bad flag."));
}

//
// Help5Menu
//

class Help5Menu : public HelpMenu {
public:
  Help5Menu();
  ~Help5Menu() { }

protected:
  float getLeftSide(int width, int height);
};

Help5Menu::Help5Menu() : HelpMenu("Good Flags")
{
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("", "Good Flags:"));

  FlagSet fs = Flag::getGoodFlags();
  for (FlagSet::iterator it = fs.begin(); it != fs.end(); it++) {

    if (((*it)->flagQuality != FlagGood) || 
	((*it)->flagTeam != NoTeam) ||
	(strcmp((*it)->flagName,"") == 0)) {
      continue;
    }

    list.push_back(createLabel((*it)->flagHelp, (*it)->label().c_str())); 
  }
}

float			Help5Menu::getLeftSide(int width, int)
{
  return 0.35f * width;
}

//
// Help6Menu
//

class Help6Menu : public HelpMenu {
public:
  Help6Menu();
  ~Help6Menu() { }

protected:
  float		getLeftSide(int width, int height);
};

Help6Menu::Help6Menu() : HelpMenu("Bad Flags")
{
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("", "Bad Flags:"));

  FlagSet fs = Flag::getBadFlags();
  for (FlagSet::iterator it = fs.begin(); it != fs.end(); it++) {

    if (((*it)->flagQuality != FlagBad) ||
	((*it)->flagTeam != NoTeam) ||
	(strcmp((*it)->flagName,"") == 0)) {
      continue;
    }

    list.push_back(createLabel((*it)->flagHelp, (*it)->label().c_str())); 
  }
}

float Help6Menu::getLeftSide(int width, int)
{
  return 0.35f * width;
}

//
// Help7Menu
//

class Help7Menu : public HelpMenu {
public:
  Help7Menu();
  ~Help7Menu() { }
};

Help7Menu::Help7Menu() : HelpMenu("Readouts I")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("The radar is on the left side of the control panel.  It shows an overhead"));
  list.push_back(createLabel("x-ray view of the game.  Buildings and the outer wall are shown in light"));
  list.push_back(createLabel("blue.  Team bases are outlined in the team color.  Teleporters are short"));
  list.push_back(createLabel("yellow lines.  Tanks are dots in the tank's team color, except rogues are"));
  list.push_back(createLabel("yellow.  The size of the tank's dot is a rough indication of the tank's"));
  list.push_back(createLabel("altitude:  higher tanks have larger dots.  Flags are small crosses.  Team"));
  list.push_back(createLabel("flags are in the team color, superflags are white, and the antidote flag"));
  list.push_back(createLabel("is yellow.  Shots are small dots (or lines or circles, for lasers and"));
  list.push_back(createLabel("shock waves, respectively).  Your tank is always dead center and forward"));
  list.push_back(createLabel("is always up on the radar.  The yellow V is your field of view.  North"));
  list.push_back(createLabel("is indicated by the letter N."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("The heads-up-display (HUD) has several displays.  The two boxes in the"));
  list.push_back(createLabel("center of the view are the motion control boxes;  within the small box"));
  list.push_back(createLabel("your tank won't move, outside the large box you don't move any faster than"));
  list.push_back(createLabel("at the edge of the large box.  Moving the mouse above or below the small"));
  list.push_back(createLabel("box moves forward or backward, respectively.  Similarly for left and right."));
  list.push_back(createLabel("The distance away from the small box determines the speed."));
}

//
// Help8Menu
//

class Help8Menu : public HelpMenu {
public:
  Help8Menu();
  ~Help8Menu() { }
};

Help8Menu::Help8Menu() : HelpMenu("Readouts II")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("Above the larger box is a tape showing your current heading.  North is"));
  list.push_back(createLabel("0, east is 90, etc.  If jumping is allowed or you have the jumping flag,"));
  list.push_back(createLabel("an altitude tape appears to the right of the larger box."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("Small colored diamonds or arrows may appear on the heading tape.  An"));
  list.push_back(createLabel("arrow pointing left means that a particular flag is to your left, an"));
  list.push_back(createLabel("arrow pointing right means that the flag is to your right, and a diamond"));
  list.push_back(createLabel("indicates the heading to the flag by its position on the heading tape."));
  list.push_back(createLabel("In capture-the-flag mode a marker always shows where your team flag is."));
  list.push_back(createLabel("A yellow marker shows the way to the antidote flag."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("At the top of the display are, from left to right, your callsign and"));
  list.push_back(createLabel("score, your status, and the flag you have.  Your callsign is in the"));
  list.push_back(createLabel("color of your team.  Your status is one of:  ready, dead, sealed, zoned"));
  list.push_back(createLabel("or reloading (showing the time until reloaded).  It can also show the"));
  list.push_back(createLabel("time until a bad flag is dropped (if there's a time limit)."));
  list.push_back(createLabel(""));
  list.push_back(createLabel("Other informational messages may occasionally flash on the HUD."));
}

//
// Help9Menu
//

class Help9Menu : public HelpMenu {
public:
  Help9Menu();
  ~Help9Menu() { }

protected:
  float getLeftSide(int width, int height);
};

Help9Menu::Help9Menu() : HelpMenu("Credits")
{
  // add controls
  std::vector<HUDuiControl*>& list = getControls();
  list.push_back(createLabel("Jeff Myers", "Mod Lead:"));
  list.push_back(createLabel("", ""));
  list.push_back(createLabel("Chris Schoeneman,Original Author:", "im Riker, Original Maintainer:"));
  list.push_back(createLabel("", ""));
  list.push_back(createLabel("David Hoeferlin, Tom Hubina", "Code Contributors:"));
  list.push_back(createLabel("Dan Kartch, Jed Lengyel", ""));
  list.push_back(createLabel("Jeff Myers, Tim Olson", ""));
  list.push_back(createLabel("Brian Smits, Greg Spencer", ""));
  list.push_back(createLabel("Daryll Strauss, Frank Thilo", ""));
  list.push_back(createLabel("Dave Brosius, David Trowbridge", ""));
  list.push_back(createLabel("Sean Morrison, Tupone Alfredo", ""));
  list.push_back(createLabel("Lars Luthman, Nils McCarthy", ""));
  list.push_back(createLabel("Daniel Remenak", ""));
  list.push_back(createLabel("", ""));
  list.push_back(createLabel("Tamar Cohen", "Tank Models:"));
  list.push_back(createLabel("", ""));
  list.push_back(createLabel("Kevin Novins, Rick Pasetto", "Special Thanks:"));
  list.push_back(createLabel("Adam Rosen, Erin Shaw", ""));
  list.push_back(createLabel("Ben Trumbore, Don Greenberg", ""));
  list.push_back(createLabel("", ""));
  list.push_back(createLabel("http://BZFlag.org/", "BZFlag Home Page:"));
  list.push_back(createLabel("", ""));
  list.push_back(createLabel("Tim Riker", "Copyright (c) 1993 - 2004"));
}

float Help9Menu::getLeftSide(int width, int height)
{
  return 0.5f * width - height / 20.0f;
}

//
// help menu getter
//

static const int numHelpMenus = 9;
HelpMenu** HelpMenu::helpMenus = NULL;

HelpMenu* HelpMenu::getHelpMenu(HUDDialog* dialog, bool next)
{
  if (!helpMenus) {
    helpMenus = new HelpMenu*[numHelpMenus];
    helpMenus[0] = new Help1Menu;
    helpMenus[1] = new Help2Menu;
    helpMenus[2] = new Help3Menu;
    helpMenus[3] = new Help4Menu;
    helpMenus[4] = new Help5Menu;
    helpMenus[5] = new Help6Menu;
    helpMenus[6] = new Help7Menu;
    helpMenus[7] = new Help8Menu;
    helpMenus[8] = new Help9Menu;
  }
  for (int i = 0; i < numHelpMenus; i++) {
    if (dialog == helpMenus[i]) {
      if (next) {
	return helpMenus[(i + 1) % numHelpMenus];
      } else {
	return helpMenus[(i - 1 + numHelpMenus) % numHelpMenus];
      }
    }
  }
  return next ? helpMenus[0] : helpMenus[numHelpMenus - 1];
}

void			HelpMenu::done()
{
  if (helpMenus) {
    for (int i = 0; i < numHelpMenus; i++) {
      delete helpMenus[i];
    }
    delete[] helpMenus;
    helpMenus = NULL;
  }
}


// Local Variables: ***
// mode: C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8
