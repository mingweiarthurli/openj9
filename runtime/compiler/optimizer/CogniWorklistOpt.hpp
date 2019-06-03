#ifndef COGNICRYPT_INC
#define COGNICRYPT_INC
#pragma once


#include "optimizer/Optimization.hpp" // for optimization
#include "optimizer/OptimizationManager.hpp" // for optimization manager


  class TR_CogniWorklistOpt : public TR::Optimization {
      public:
         TR_CogniWorklistOpt(TR::OptimizationManager *m) : TR::Optimization(m) {};
         static TR::Optimization *create (TR::OptimizationManager *m) {
            return new (m->allocator()) TR_CogniWorklistOpt(m);
         };
         virtual bool shouldPerform();
         virtual int32_t perform();
         virtual const char * optDetailString() const throw()
         {
	   //for now do a hello world test!
            return "O^O Hello World Opt : ";
         };
   };

#endif
