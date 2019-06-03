#include "env/VerboseLog.hpp"
#include "optimizer/CogniWorklistOpt.hpp"

bool
TR_CogniWorklistOpt::shouldPerform() {
   return true;
}

int32_t
TR_CogniWorklistOpt::perform() {
   // Say Hello world in the verbose log.
   //TR_VerboseLog::vlogAcquire();
   // TR_Vlog_HWO is the tag to identify one's optimization pass in the verbose log.
   TR_VerboseLog::writeLine(TR_Vlog_CWO, "Hello world");
   printf("IS WORKLIST OPT RUN\n");
   //   TR_VerboseLog::vlogRelease();
   return 1;
}
