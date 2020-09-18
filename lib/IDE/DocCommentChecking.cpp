//===--- DocCommentChecking.cpp - Checks for documentation comments -------===//
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

#include "swift/AST/Comment.h"
#include "swift/AST/Decl.h"
#include "swift/AST/DiagnosticsCommon.h"
#include "swift/IDE/DocCommentChecking.h"
#include "swift/Frontend/Frontend.h"

using namespace swift;

DocCommentChecker::DocCommentChecker(CompilerInstance &CI)
  : Mode(CI.getInvocation().getFrontendOptions().CheckDocComments),
    Diags(CI.getDiags()) {}

void
DocCommentChecker::checkDocCommentConsistency(const AbstractFunctionDecl *Func,
                                              const DocComment *DC) const {
  // Diagnose missing parameter fields in the doc comment
  if (const auto *Params = Func->getParameters()) {
    for (const auto *Param : Params->getArray()) {
      const auto DCParamFields = DC->getParts().ParamFields;
      const auto Found = std::find_if(DCParamFields.begin(),
                                      DCParamFields.end(),
                                      [&](const markup::ParamField *P) -> bool {
        return P->getName() == Param->getParameterName().str();
      });
      if (Found == DCParamFields.end()) {
        Diags.diagnose(Func->getLoc(),
                       diag::public_api_missing_param_description,
                       Func->getDescriptiveKind(),
                       Func->getName(),
                       Param->getParameterName());
      }
    }
  }

  if (const auto *FTy = Func->getInterfaceType()->getAs<FunctionType>()) {
    // Diagnose missing `- returns:` field in the doc comment if the function
    // has a non-Void return type.
    if (const auto Result = FTy->getResult()) {
      if (!Result->isVoid() && !DC->getParts().ReturnsField.hasValue()) {
        Diags.diagnose(Func->getLoc(),
                       diag::public_api_missing_return_description,
                       Func->getDescriptiveKind(),
                       Func->getName());
      }
    }

    // Diagnose missing `- throws:` field in the doc comment if the function
    // is marked as `throws`.
    if (FTy->isThrowing() && !DC->getParts().ThrowsField.hasValue()) {
      Diags.diagnose(Func->getLoc(),
                     diag::public_api_missing_throws_description,
                     Func->getDescriptiveKind(),
                     Func->getName());
    }
  }
}

bool DocCommentChecker::walkToDeclPre(Decl *D, CharSourceRange Range) {
  if (Mode == FrontendOptions::DocCheckMode::Disabled) {
    return false;
  }

  switch (D->getKind()) {
      // We'll check the following kinds of declarations
    case DeclKind::Class:
    case DeclKind::Struct:
    case DeclKind::Enum:
    case DeclKind::EnumElement:
    case DeclKind::Protocol:
    case DeclKind::Constructor:
    case DeclKind::Func:
    case DeclKind::Var:
    case DeclKind::Subscript:
    case DeclKind::TypeAlias:
    case DeclKind::AssociatedType:
    case DeclKind::EnumCase:
      break;
    default:
      // We'll descend into everything else.
      return true;
  }

  if (D->nameOrContextImpliesOmissionFromDocs(AccessLevel::Public,
                                    /*Consider context*/ true,
                                    /*Consider SPI hidden */false)) {
    return false;
  }

  const auto *VD = cast<ValueDecl>(D);

  markup::MarkupContext MC;
  const auto *DC = getCascadingDocComment(MC, VD);
  if (!DC || DC->getParts().isEmpty()) {
    Diags.diagnose(VD->getLoc(),
                   diag::public_api_missing_doc_comment,
                   VD->getDescriptiveKind(), VD->getName());
    return true;
  }

  if (Mode == FrontendOptions::DocCheckMode::Consistency) {
    if (const auto *Func = dyn_cast<AbstractFunctionDecl>(VD)) {
      checkDocCommentConsistency(Func, DC);
    }
  }

  return true;
}
