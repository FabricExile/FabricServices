// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __Commands_CompoundCommand__
#define __Commands_CompoundCommand__

#include "Command.h"

namespace FabricServices
{

  namespace Commands
  {
    class CompoundCommand : public Command
    {
    public:

      CompoundCommand();
      virtual ~CompoundCommand();

      virtual const char * getName() const { return "Compound"; }
      virtual const char * getShortDesc() const { return "A command container."; }
      virtual const char * getFullDesc() const { return "A command to contain other commands."; }

      virtual bool add(Command * command);
      virtual bool isEmpty() const;

      unsigned int getNbCommands() const;
      Command * getCommand(unsigned int index);

      Command * getFirstNonCompoundCommand();
      Command * getLastNonCompoundCommand();

    protected:
      
      virtual bool invoke();
      virtual bool undo();
      virtual bool redo();
      virtual void destroy(); 

    private:

      CommandVector m_commands;
    };

  };

};

#endif // __Commands_CompoundCommand__
