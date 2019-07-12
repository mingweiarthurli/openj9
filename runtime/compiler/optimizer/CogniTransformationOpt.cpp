/*******************************************************************************
 * Copyright (c) 2000, 2019 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/
#include "env/VerboseLog.hpp"
#include "optimizer/CogniTransformationOpt.hpp"
#include "il/SymbolReference.hpp"
#include "compile/ResolvedMethod.hpp"
#include "il/Block.hpp"

//hardcoded for testing
//TODO replace
char *analyzedClass = "PBEmin";
char *analyzedMethodName = "getPBEKeySpec";
char *analyzedMethodSignature = "()Ljavax/crypto/spec/PBEKeySpec;";
char *callsiteClass = "javax/crypto/spec/PBEKeySpec"; 
char *callsiteMethodName = "<init>";
char *callsiteMethodSignature = "([C[BII)V";
uint32_t bci = 80;
//https://github.com/CROSSINGTUD/Crypto-API-Rules/blob/master/JavaCryptographicArchitecture/src/PBEKeySpec.cryptsl#L19
int32_t constraintValue = 10000;
int32_t ithChild = 2; //indexed from 0 atm


bool
TR_CogniTransformationOpt::shouldPerform() {
   return true;
}


/*
 * Places a safety check into a particular 
 * location determined from information 
 * sent to us from CogniCrypt
 * 
 * Currently ONLY works on locations that are callsites.
 *
 * Does this by checking 2 things:
 * 1) is the current compiling method (compilee) the one that was analyzed? OR
 * 2) is any inlined method in the compilee the one that was analyzed? 
 * 
 * If either of the above:
 * find the call node that it is to be guarded.
 *
 */
int32_t
TR_CogniTransformationOpt::perform() {

  
  //first check on analyzed method detection:
  TR_ResolvedMethod *feMethod = comp()->getCurrentMethod();
  checkCallSite(feMethod, 0, -1);
  //then check for any methods inlined that are may be the analyzed method
  for (int32_t i = 0; i < comp()->getNumInlinedCallSites(); ++i)
    {
      int32_t callerIndex = comp()->getInlinedCallSite(i)._byteCodeInfo.getCallerIndex();
      TR_ResolvedMethod  *method = comp()->getInlinedResolvedMethod(i);
      //      checkCallSite(method, i, callerIndex);
    }
  return 1;
}


void
TR_CogniTransformationOpt::checkCallSite(TR_ResolvedMethod  *method, int i, int32_t callerIndex) {

  char *callerClass = method->classNameChars();
  char *callerMethodName = method->nameChars();
  char *callerClassMethodSignature = method->signatureChars();

  
  if((strstr(callerClass, analyzedClass) != NULL) && (strstr(callerMethodName, analyzedMethodName) != NULL) && (strstr(callerClassMethodSignature, analyzedMethodSignature) != NULL)){

    TR_VerboseLog::vlogAcquire();
    TR_VerboseLog::writeLine(TR_Vlog_CT, "Found an occurrence of the analyzed method: %s.%s %s\n", callerClass, callerMethodName, callerClassMethodSignature);
    TR_VerboseLog::vlogRelease();

    //now collect the call node that is to be guarded
    TR::Node* node;
    TR::TreeTop *tt;
    if(i != 0){
      tt = comp()->getInlinedResolvedMethodSymbol(i)->getFirstTreeTop();
    }else{
      tt = comp()->getStartTree();
    }

    tt = getCallNode(tt, bci, callerIndex);

    if(tt != NULL){
      
    node = tt->getNode()->getFirstChild();
    
    TR::Symbol *symbol = node->getSymbolReference()->getSymbol();
    if(symbol->isResolvedMethod()){
      
      TR::ResolvedMethodSymbol *callsiteMethodSymbol = node->getSymbolReference()->getSymbol()->castToResolvedMethodSymbol();
      if(callsiteMethodSymbol){
	
	TR_ResolvedMethod *callsiteMethod = callsiteMethodSymbol->getResolvedMethod();
	char *potentialCallsiteClass = callsiteMethod->classNameChars();
	char *potentialCallsiteMethodName = callsiteMethod->nameChars();
	char *potentialCallsiteMethodSignature = callsiteMethod->signatureChars();

	//double check that the analyzed method indeed contains the expected callsite at expected location
	if((strstr(potentialCallsiteClass, callsiteClass) != NULL) && (strstr(potentialCallsiteMethodName, callsiteMethodName) != NULL) && (strstr(potentialCallsiteMethodSignature, callsiteMethodSignature) != NULL)){

	  //TODO rm
	  TR_VerboseLog::vlogAcquire();
	  TR_VerboseLog::writeLine(TR_Vlog_CT, "Found the callsite to perform the transformation at: %s.%s %s\n", potentialCallsiteClass, potentialCallsiteMethodName, potentialCallsiteMethodSignature);
   	  TR_VerboseLog::vlogRelease();

	  //TODO rm
	  //for some reason this transformation currently does not auto know that the trees changed
	  //and that we would like a post transform tree, this msg is all we get
	  if(comp()->getOption(TR_TraceAll)){
            traceMsg(comp(), "Anchoring the children!");
          }
	  
	  //https://github.com/eclipse/openj9-omr/blob/master/compiler/optimizer/OMROptimization.cpp#L188
	  self()->anchorAllChildren(node, tt);

	  //split
	  TR::CFG *cfg = comp()->getFlowGraph();
	  TR::Block *callBlock = tt->getEnclosingBlock();
	  TR::Block *newBlock = callBlock->split(tt, cfg, true, true);

	  
	  //generate a target block, empty for now...

	  TR::Block *exceptionBlock = TR::Block::createEmptyBlock(comp(), 0);
	  
	  //generate an exception tree
	  TR_OpaqueClassBlock *jitHelpersClass = comp()->getJITHelpersClassPointer();
   
	  if (!jitHelpersClass || !TR::Compiler->cls.isClassInitialized(comp(), jitHelpersClass))
	    {
	      printf("Did not find JITHelpers class...\n");
	      return;
	    }

	  TR::Node *originNode = exceptionBlock->getEntry()->getNode() ;
	  TR::TreeTop *exceptBlockExit = exceptionBlock->getExit();
	  
	  TR::SymbolReference* helperAccessor =
	    comp()->getSymRefTab()->methodSymRefFromName(
	    comp()->getMethodSymbol(),
            "com/ibm/jit/JITHelpers",
            const_cast<char*>("jitHelpers"),
            const_cast<char*>("()Lcom/ibm/jit/JITHelpers;"),
            TR::MethodSymbol::Static);
	  TR::SymbolReference* throwSecurityViolationSymRef =
	    comp()->getSymRefTab()->methodSymRefFromName(
            comp()->getMethodSymbol(),
            "com/ibm/jit/JITHelpers",
            "throwSecurityViolation",
            "()V",
            TR::MethodSymbol::Static);
	  TR::Node *getHelpers = TR::Node::createWithSymRef(originNode, TR::acall, 0, helperAccessor);
	  exceptionBlock->append(TR::TreeTop::create(comp(), TR::Node::create(originNode, TR::treetop, 1, getHelpers)));
	  TR::Node *throwViolation = TR::Node::createWithSymRef(originNode, TR::call, 1, throwSecurityViolationSymRef);
	  throwViolation->setAndIncChild(0, getHelpers);
	  exceptionBlock->append(TR::TreeTop::create(comp(), TR::Node::create(originNode, TR::treetop, 1, throwViolation)));
   
	  TR::Node *gotoNode = TR::Node::create(TR::Goto, 0);
	  //point the goto to the join point
	  gotoNode->setBranchDestination(newBlock->getEntry());
	  TR::TreeTop *gotoTree = TR::TreeTop::create(comp(), gotoNode);
	  exceptionBlock->append(gotoTree);
	  
	  TR::TreeTop *exceptionEntry = exceptionBlock->getEntry();
	  
	  //also since its now the last bb, patch that connection as well
	  TR::TreeTop *lastTree = comp()->findLastTree();
	  lastTree->join(exceptionEntry);
	  
	  //add the exception block to cfg
	  cfg->addNode(exceptionBlock);
	  cfg->addEdge(TR::CFGEdge::createEdge(newBlock->getPrevBlock(), exceptionBlock, trMemory()));
	  cfg->addEdge(TR::CFGEdge::createEdge(exceptionBlock, newBlock, trMemory()));
      
	
	  //create the if tree to insert before the call
	  TR::Node *load = TR::Node::createWithSymRef(TR::iload, 0, node->getChild(ithChild+1)->getSymbolReference());
	  TR::Node *iconst = TR::Node::iconst(0, constraintValue);
	  TR::Node *cmp = TR::Node::createif(TR::ificmple, load, iconst, exceptionEntry);
	  TR::TreeTop *cmpTree = TR::TreeTop::create(comp(), cmp, tt, tt->getPrevTreeTop());
	  
	  newBlock->getPrevBlock()->append(cmpTree);
	}
      }
    }
    }else{
      TR_VerboseLog::vlogAcquire();
      TR_VerboseLog::writeLine(TR_Vlog_CT, "Transformation location not found.\n");
      TR_VerboseLog::writeLine(TR_Vlog_CT, "Bad information provided. In %s.%s $s, no callsite of %s.%s %s @ %d.\n", analyzedClass, analyzedMethodName, analyzedMethodSignature, callsiteClass, callsiteMethodName, callsiteMethodSignature, bci);
      TR_VerboseLog::vlogRelease();
    }
  }
}

//slightly modified to check on callerIndex as well, but mostly similar as function
//from https://github.com/eclipse/openj9/blob/master/runtime/compiler/optimizer/J9EstimateCodeSize.cpp#L472
static TR::TreeTop* getCallNode (TR::TreeTop* tt, uint32_t bci, int32_t callerIndex)
   {
     for (; tt; tt=tt->getNextTreeTop())
      {
	
      if (tt->getNode()->getNumChildren()>0 &&
          tt->getNode()->getFirstChild()->getOpCode().isCall() &&
	  tt->getNode()->getFirstChild()->getByteCodeInfo().getCallerIndex()  == callerIndex &&
          tt->getNode()->getFirstChild()->getByteCodeIndex() == bci)
         {
	   return tt;
         }
      }
     return NULL;
   }


