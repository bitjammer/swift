//===--- DocCommentChecking.h - Checks for documentation comments ---------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_IDE_DOC_COMMENT_CHECKING_H
#define SWIFT_IDE_DOC_COMMENT_CHECKING_H

#include "swift/AST/DiagnosticEngine.h"
#include "swift/Frontend/FrontendOptions.h"
#include "swift/IDE/SourceEntityWalker.h"

namespace swift {

class DocCommentChecker : public SourceEntityWalker {
  FrontendOptions::DocCheckMode Mode;
  DiagnosticEngine &Diags;
  bool walkToDeclPre(Decl *D, CharSourceRange Range) override;
  void checkDocCommentConsistency(const AbstractFunctionDecl *Func,
                                  const DocComment *DC) const;
public:
  DocCommentChecker(CompilerInstance &CI);
};

} // end namespace swift

#endif // SWIFT_IDE_DOC_COMMENT_CHECKING_H
