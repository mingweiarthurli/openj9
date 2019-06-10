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
