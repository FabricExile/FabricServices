// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "Command.h"

using namespace FabricServices::Commands;

unsigned int Command::s_maxID = 1;

Command::Command()
{
  m_id = s_maxID++;
}

Command::~Command()
{

}

unsigned int Command::getID() const
{
  return m_id;
}

void Command::destroy()
{

}

bool Command::redo()
{
  return invoke();
}
