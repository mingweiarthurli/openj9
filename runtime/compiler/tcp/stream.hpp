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
#ifndef STREAM_H
#define STREAM_H

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "out/compile.pb.h"
#include <endian.h>

namespace TCP

{
  
  using namespace google::protobuf::io;


class Stream

{

public:

  //each stream object is inherently bidirectional
  ZeroCopyOutputStream* rawOutput;
  ZeroCopyInputStream* rawInput;
  CodedInputStream* codedInput;
  CodedOutputStream* codedOutput;
  ServerMsg srvmsg;
  ClientMsg clientmsg;


  //TODO this could use a refactor, dead code
  void startStream(int fd){

	//std::cout << "file descripor\n" ;
	//std::cout << fd;
	//std::cout << "\n";
  
  }

  //do we need this?
  void cleanup(){
    delete rawOutput;
    delete rawInput;
  }

  //protobuf CodedInputStream does not expose api for big endian order
  //which is what java.io.DataOutputStream insists upon, ick
  int streamReadInt(int fd){
	rawInput = new FileInputStream(fd);
	codedInput = new CodedInputStream(rawInput);

	uint32_t sentInt;
	if (!codedInput->ReadLittleEndian32(&sentInt)){
	  return 0;
	}else{
	  return htobe32(sentInt);
	}
  }

  std::string streamReadString(int fd){
	rawInput = new FileInputStream(fd);
	codedInput = new CodedInputStream(rawInput);
	std::string receivedString;
	
	//a badish way of dealing with java.io.DataOutputStream.writeUTF's 2byte size hint
	uint16_t msgsize;
	if (codedInput->ReadRaw(&msgsize, 2)) {
	  codedInput->ReadString( &receivedString, htobe16(msgsize));
	}
	return receivedString;
  }
  
template <typename T>
void streamRead(T &val, int fd){

  rawInput = new FileInputStream(fd);
  codedInput = new CodedInputStream(rawInput);
  uint32_t msgsize;
  
  if (!codedInput->ReadLittleEndian32(&msgsize)){
    std::cout << "No message size read" ;
  }else{
    std::cout << "Message size read" ;

  }
  
  std::cout << msgsize ;
  auto limit = codedInput->PushLimit(msgsize);
  //read bytes of message now
  if (!val.ParseFromCodedStream(codedInput)){
    std::cout << "Could not read from stream\n" ;
  }else{
    std::cout << "Read from stream\n" ;
  }
  if (!codedInput->ConsumedEntireMessage()){
    std::cout << "Error reading entire message\n" ;
  }
  
  //this should eventually move out of here
  //  ServerMsgType msgtype = val.type();
  std::cout << "Message type is: " ;
  std::cout << val.type() ;
  std::cout << "\n";
  std::cout << "Message text is: " ;
  std::cout << val.text();
  std::cout << "\n";
  
  codedInput->PopLimit(limit);


  delete codedInput;
  delete rawInput;
}


//takes arg of message  
template <typename T>
void streamWrite(const T &val, int fd){
  rawOutput = new FileOutputStream(fd);
  codedOutput = new CodedOutputStream(rawOutput);
  //get size of message and write that then message
  size_t msgSize = val.ByteSizeLong();

  //std::cout << "message size sent" ;
  //std::cout << msgSize ;
  
  codedOutput->WriteLittleEndian32(msgSize);
  val.SerializeToCodedStream(codedOutput);
  //val.SerializeWithCachedSizes(codedOutput);
  if(codedOutput->HadError()){
    std::cout << "Error writing message" ;
  }else{
    //    std::cout << "Wrote a message!" ;
  }

     delete codedOutput;
  delete rawOutput;
  //std::cout << "Wrote a message!" ; 
  //flush the buffer
// if(!((FileOutputStream*)rawOutput)->Flush()){
//std::cout << "Error flushing buffer" ;
// }

 
}

};
};
 
#endif //STREAM_H
