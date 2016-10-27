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
#include "HUDDialogStack.h"

/* system implementation headers */
#include <vector>

/* common implementation headers */
#include "BzfWindow.h"

/* local implementation headers */
#include "HUDui.h"
#include "MainWindow.h"

/* from playing.cxx */
MainWindow*		getMainWindow();


HUDDialogStack HUDDialogStack::globalStack;

HUDDialogStack::HUDDialogStack()
{
  // do nothing
}

HUDDialogStack::~HUDDialogStack()
{
  if (getMainWindow())
    getMainWindow()->getWindow()->removeResizeCallback(resize, this);
}

HUDDialogStack* HUDDialogStack::get()
{
  return &globalStack;
}

bool HUDDialogStack::isActive() const
{
  return stack.size() != 0;
}

HUDDialog* HUDDialogStack::top() const
{
  const int index = stack.size();
  if (index == 0) return NULL;
  return stack[index - 1];
}

void HUDDialogStack::push(HUDDialog* dialog)
{
  if (!dialog) return;
  if (isActive()) {
    const int index = stack.size() - 1;
    stack[index]->setFocus(HUDui::getFocus());
    stack[index]->dismiss();
  }
  else {
    getMainWindow()->getWindow()->addResizeCallback(resize, this);
  }
  stack.push_back(dialog);
  HUDui::setDefaultKey(dialog->getDefaultKey());
  HUDui::setFocus(dialog->getFocus());
  dialog->resize(getMainWindow()->getWidth(), getMainWindow()->getHeight());
  dialog->show();
}

void HUDDialogStack::pop()
{
  if (isActive())
	{
    const int index = stack.size() - 1;
    stack[index]->setFocus(HUDui::getFocus());
    stack[index]->dismiss();
    std::vector<HUDDialog*>::iterator it = stack.begin();
    for(int i = 0; i < index; i++) it++;
    stack.erase(it);
    if (index > 0) {
      HUDDialog* dialog = stack[index - 1];
      HUDui::setDefaultKey(dialog->getDefaultKey());
      HUDui::setFocus(dialog->getFocus());
      dialog->resize(getMainWindow()->getWidth(),
		     getMainWindow()->getHeight());
      dialog->show();
    }
    else {
      HUDui::setDefaultKey(NULL);
      HUDui::setFocus(NULL);
      getMainWindow()->getWindow()->removeResizeCallback(resize, this);
    }
  }
}

void HUDDialogStack::render()
{

	glTranslatef(0,0,-0.5f);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

  if (isActive())
		(*(stack.end()-1))->render();
  //  stack[stack.size() - 1]->render();

	glEnable(GL_LIGHTING);
}

void HUDDialogStack::resize(void* _self)
{
  HUDDialogStack* self = (HUDDialogStack*)_self;
  if (self->isActive())
    self->top()->resize(getMainWindow()->getWidth(),
			getMainWindow()->getHeight());
}


// Local Variables: ***
// mode: C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8