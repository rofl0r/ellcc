// ccundolog.h            see license.txt for copyright and terms of use
// data structures used for reversing CPP

#ifndef CCUNDOLOG_H
#define CCUNDOLOG_H

#include "taillist.h"           // TailList
#include "strobjdict.h"         // StringObjDict
#include "SourceLocation.h"     // SourceLocation
using ellcc::SourceLocation;
#include "taillist.h"

class MacroUndoEntry;

class MacroDefinition {
public:
  SourceLocation fromLoc;
  SourceLocation toLoc;
  const char *name;
  MacroDefinition(const char *name, SourceLocation fromLoc, SourceLocation toLoc):
  fromLoc(fromLoc),
    toLoc(toLoc),
    name(name)
    {
    }
};

class MacroUndoEntry {
public:
  // post-expansion start & end position
  // helps determine if an ast node is part of a macro
  SourceLocation postStartLoc;
  SourceLocation postEndLoc;

  // pre-expansion positions of the macro
  SourceLocation preStartLoc;
  SourceLocation preEndLoc;

  // macro name
  const char *name;
  // parent macro
  MacroUndoEntry *parent;
  
  TailList<MacroDefinition> params;

  MacroUndoEntry(SourceLocation postStartLoc, SourceLocation preStartLoc, SourceLocation preEndLoc,
		 const char *name, MacroUndoEntry *parent):
    postStartLoc(postStartLoc),
    postEndLoc(SL_UNKNOWN),
    preStartLoc(preStartLoc),
    preEndLoc(preEndLoc),
    name(name),
    parent(parent)
  {
  }

  bool isParam() {
    return strchr(name, ':') != NULL;
  }
 
};

class CPPSourceLocation {
public:
  MacroUndoEntry *macroExpansion;
  CPPSourceLocation(SourceLocation loc);

  bool hasExactPosition() const {
    return _loc != SL_UNKNOWN && (!macroExpansion || exactPosition);
  }

  SourceLocation loc() const {
    return _loc;
  }

  // overriding loc implies that the position is now exact
  void overrideLoc(SourceLocation loc) {
    _loc = loc;
    exactPosition = true;
  } 
private:
  // gdb wont let me debug the constructor
  void init(SourceLocation loc);
  SourceLocation _loc;
  // can be true when a position within a macro can be traced 
  // to an ectual position(ie at start/end or in a param).
  // This flag is only valid when macroExpansion != NULL
  // because the position is exact when macroExpansion == NULL
  bool exactPosition;
};

// main undo log structure
extern TailList<MacroUndoEntry> macroUndoLog;

#endif // CCUNDOLOG_H
