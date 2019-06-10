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
#include "optimizer/CogniWorklistOpt.hpp"
#include "il/SymbolReference.hpp"

bool
TR_CogniWorklistOpt::shouldPerform() {
   return true;
}

int32_t
TR_CogniWorklistOpt::perform() {
   // Say Hello world in the verbose log.
   //TR_VerboseLog::vlogAcquire();
   // TR_Vlog_HWO is the tag to identify one's optimization pass in the verbose log.
   //TR_VerboseLog::writeLine(TR_Vlog_CWO, "Hello world");
   //TR_VerboseLog::vlogRelease();

   //sample to observe the IL
   TR::TreeTop *tt = comp()->getStartTree();
   for (; tt; tt = tt->getNextTreeTop()){

     TR::Node *node = tt->getNode();

     if (node->getNumChildren() > 0){
       if (node->getOpCodeValue() == TR::treetop) // jump over TreeTop
         node = node->getFirstChild();


       if (node->getNumChildren() > 0 &&
	   node->getFirstChild()->getOpCode().isFunctionCall()){

	 TR::Node *classNode = node->getFirstChild();
	 TR::Symbol *symbol = classNode->getSymbolReference()->getSymbol();

	 if(symbol->isResolvedMethod()){
	 TR::ResolvedMethodSymbol *method = symbol->castToResolvedMethodSymbol();
	 if(method){
	   TR_ResolvedMethod *m = method->getResolvedMethod();
       
	   int32_t len;

	   char *sig = m->signatureChars();
		   	   printf("Found a signature: %s\n", sig);
		   
		   if(((strstr(sig, "java/security") != NULL)) || ((strstr(sig, "javax/crypto") != NULL))){
		     printf("Found this name belonging to sec or crypt: %s\n", sig);
		   }
	 }
	 }
       }
     }
   }

   
   return 1;
}
