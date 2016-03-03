// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __Commands_Command__
#define __Commands_Command__

#include <vector>
#include <string>

namespace FabricServices
{

  namespace Commands
  {
    class Command
    {
      friend class CommandStack;
      friend class CompoundCommand;

    public:

      Command();
      virtual ~Command();

      unsigned int getID() const;
      virtual const char * getName() const = 0;
      virtual const char * getShortDesc() const = 0;
      virtual const char * getFullDesc() const = 0;

    protected:
      
      virtual bool invoke() = 0;
      virtual bool undo() = 0;
      virtual bool redo();
      virtual void destroy(); 

    private:

      unsigned int m_id;
      static unsigned int s_maxID;
    };

    typedef std::vector<Command*> CommandVector;

  };

};

#endif // __Commands_Command__
