// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "CommandStack.h"

#include <cstddef>

using namespace FabricServices::Commands;

CommandStack::CommandStack()
{
  m_limit = 0;
}

CommandStack::~CommandStack()
{
  for(size_t i=0;i<m_undoCommands.size();i++)
    delete(m_undoCommands[i]);
  for(size_t i=0;i<m_redoCommands.size();i++)
    delete(m_redoCommands[i]);
}

uint32_t CommandStack::getLimit() const
{
  return m_limit;
}

void CommandStack::setLimit(uint32_t limit)
{
  m_limit = limit;

  capCommandsForLimit();
}

bool CommandStack::add(Command * command)
{
  if(!command->invoke())
    return false;

  m_undoCommands.push_back(command);

  for(size_t i=0;i<m_redoCommands.size();i++)
  {
    m_redoCommands[i]->destroy();
    delete(m_redoCommands[i]);
  }
  m_redoCommands.clear();

  capCommandsForLimit();

  return true;
}

void CommandStack::clear()
{
  for(size_t i=0;i<m_undoCommands.size();i++)
  {
    m_undoCommands[i]->destroy();
    delete(m_undoCommands[i]);
  }
  m_undoCommands.clear();
  for(size_t i=0;i<m_redoCommands.size();i++)
  {
    m_redoCommands[i]->destroy();
    delete(m_redoCommands[i]);
  }
  m_redoCommands.clear();
}

bool CommandStack::undo(unsigned int id)
{
  if(m_undoCommands.size() == 0)
    return false;

  Command * command = m_undoCommands.back();

  if(id != UINT_MAX && command->getID() != id)
    return false;

  m_undoCommands.pop_back();

  if(command->undo())
  {
    m_redoCommands.push_back(command);
    capCommandsForLimit();
    return true;
  }
  else
  {
    command->destroy();
    delete(command);
  }
  return false;
}

bool CommandStack::redo(unsigned int id)
{
  if(m_redoCommands.size() == 0)
    return false;

  Command * command = m_redoCommands.back();

  if(id != UINT_MAX && command->getID() != id)
    return false;

  m_redoCommands.pop_back();

  if(command->redo())
  {
    m_undoCommands.push_back(command);
    capCommandsForLimit();
    return true;
  }
  else
  {
    command->destroy();
    delete(command);
  }

  return false;
}

void CommandStack::capCommandsForLimit()
{
  if(m_limit == 0)
    return;

  if(m_undoCommands.size() > m_limit)
  {
    for(size_t i=0;i<m_undoCommands.size() - m_limit; i++)
    {
      m_undoCommands[i]->destroy();
      delete(m_undoCommands[i]);
    }
    m_undoCommands = CommandVector(m_undoCommands.begin() + (m_undoCommands.size() - m_limit), m_undoCommands.end()); 
  }

  if(m_redoCommands.size() > m_limit)
  {
    for(size_t i=0;i<m_redoCommands.size() - m_limit; i++)
    {
      m_redoCommands[i]->destroy();
      delete(m_redoCommands[i]);
    }
    m_redoCommands = CommandVector(m_redoCommands.begin() + (m_redoCommands.size() - m_limit), m_redoCommands.end()); 
  }
}
