// xml_file_writer.h            see license.txt for copyright and terms of use

// Serialization of file information for purposes of capturing the
// state of the SourceManager.

#ifndef XML_FILE_WRITER_H
#define XML_FILE_WRITER_H

#include "sobjset.h"            // SObjSet
#include "objlist.h"            // ObjList
#include "srcloc.h"             // SourceManager
#include "xml_writer.h"         // XmlWriter
#include "hashline.h"           // HashLineMap

class XmlFileWriter : public XmlWriter {
public:

  class XFW_SerializeOracle {
  public:
    virtual ~XFW_SerializeOracle() {}
    virtual bool shouldSerialize(SourceManager::File const *) {return true;}
    virtual bool shouldSerialize(HashLineMap const *) {return true;}
    virtual bool shouldSerialize(HashLineMap::HashLine const *) {return true;}
  };

  XFW_SerializeOracle *serializeOracle_m;

  XmlFileWriter(IdentityManager &idmgr0, std::ostream *out0, int &depth0, bool indent0,
                XFW_SerializeOracle *serializeOracle0);
  virtual ~XmlFileWriter() {}

  void toXml(SourceManager::FileList &files);
  void toXml(SourceManager::File *file);
  // this is an exception to the generic toXml() mechanism since
  // lineLengths are not self-contained
  void toXml_lineLengths(SourceManager::File *file);
  void toXml(HashLineMap *hashLines);
  void toXml(HashLineMap::HashLine *hashLine);
};

#endif // XML_FILE_WRITER_H
