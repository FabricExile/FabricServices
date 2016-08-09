// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __Persistence_RTValFromJSONDecoder__
#define __Persistence_RTValFromJSONDecoder__

#include <vector>
#include <string>
#include <FabricCore.h>
#include <FTL/JSONDec.h>

namespace FabricServices
{

  namespace Persistence
  {
    
    class RTValFromJSONDecoder : public FabricCore::RTValFromJSONDecoder
    {

    public:

      virtual FabricCore::RTValCodecResult decodeRTValFromJSON(
        FabricCore::Context const &context,
        FabricCore::RTVal &rtVal,
        FabricCore::RTValCodecContext codecContext,
        char const *jsonData,
        uint32_t jsonSize,
        FabricCore::RTValFromJSONDecoder::MetadataLookupFunctor const &metadataLookup
        )
      {
        if(jsonSize <= 2)
          return FabricCore::RTValCodecResult_Accept_Pending;

        try
        {
          // DFG_METADATA_UIPERSISTVALUE was checked only when encoding binding args.
          // Here, we always decode if the value exists.

          if(!rtVal.isValid())
            return FabricCore::RTValCodecResult_Accept_Pending;
          if(!rtVal.isObject())
            return FabricCore::RTValCodecResult_Accept_Pending;
          if(rtVal.isNullObject())
            return FabricCore::RTValCodecResult_Accept_Pending;

          std::string decodedString;
          {
            FTL::StrRef jsonStr( jsonData, jsonSize );
            FTL::JSONStrWithLoc strWithLoc( jsonStr );
            FTL::JSONDec<FTL::JSONStrWithLoc> jsonDec( strWithLoc );
            FTL::JSONEnt<FTL::JSONStrWithLoc> jsonEnt;
            if ( !jsonDec.getNext( jsonEnt )
              || jsonEnt.getType() != jsonEnt.Type_String )
              return FabricCore::RTValCodecResult_Accept_Pending;
            jsonEnt.stringAppendTo( decodedString );
          }

          FabricCore::RTVal cast = FabricCore::RTVal::Construct(context, "RTValFromJSONDecoder", 1, &rtVal);
          if(!cast.isInterface())
            return FabricCore::RTValCodecResult_Accept_Pending;
          if(cast.isNullObject())
            return FabricCore::RTValCodecResult_Accept_Pending;

          FabricCore::RTVal data =
            FabricCore::RTVal::ConstructString(
              context,
              decodedString.data(),
              decodedString.size()
              );
          FabricCore::RTVal result = cast.callMethod("Boolean", "convertFromString", 1, &data);
          if(!result.isValid())
            return FabricCore::RTValCodecResult_Accept_Pending;

          return result.getBoolean()?
            FabricCore::RTValCodecResult_Accept_Complete:
            FabricCore::RTValCodecResult_Accept_Pending;
        }
        catch(FabricCore::Exception e)
        {
          printf("decodeRTValFromJSON: Hit exception: %s\n", e.getDesc_cstr());
        }

        return FabricCore::RTValCodecResult_Accept_Pending;
      }

    };

  };

};

#endif // __Persistence_RTValFromJSONDecoder__
