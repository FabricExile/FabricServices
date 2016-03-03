// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __Commands_CommandStack__
#define __Commands_CommandStack__

#include "Command.h"

#include <limits.h>
#include <stdint.h>

namespace FabricServices
{

  namespace Commands
  {

    class CommandStack
    {
    public:

      CommandStack();
      virtual ~CommandStack();

      uint32_t getLimit() const;
      void setLimit(uint32_t limit);

      virtual bool add(Command * command);
      virtual void clear();
      virtual bool undo(unsigned int id = UINT_MAX);
      virtual bool redo(unsigned int id = UINT_MAX);

    private:

      void capCommandsForLimit();

      uint32_t m_limit;
      CommandVector m_undoCommands;
      CommandVector m_redoCommands;
    };

  };

};

#endif // __Commands_CommandStack__
