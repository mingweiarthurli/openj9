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
//#include "socket.hpp"
#include "out/compile.pb.h"

class Server {

 public:
  int fd;
  TCP::Stream stream;  
  
  Server(){
  fd = socketConnection();
  stream.startStream(fd);
  }


  void writeServer(TCP::ServerMsgType type, std::string text){
  
  stream.srvmsg.set_type(type);
  stream.srvmsg.set_text(text);
  stream.streamWrite(stream.srvmsg, fd);

}


int socketConnection(){

  struct sockaddr_in sin, clientAddr; 
  int backlog = 2;
  int socketFd, setup, lstn, acceptSock;
  socklen_t clientAddrSize = sizeof(clientAddr);
  
  
  //create socket
  socketFd = socketInit();

  //bind socket
  sin.sin_family = AF_INET;
  sin.sin_port = htons(PORT);
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  setup = bind(socketFd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
  if(setup < 0){
    handleError(1, socketFd, "Cannot bind socket");
    }

  //listen on socket
  lstn = listen(socketFd, backlog);
  if(lstn < 0){
    handleError(1, socketFd, "Cannot bind socket");
  }
  //accept a request on this socket to connect, currently can only connect one client
  acceptSock = accept(socketFd, (struct sockaddr *) &clientAddr, &clientAddrSize);
  if(acceptSock < 0){
    handleError(1, socketFd, "Cannot accept client");
  }

  close(socketFd);
  return(acceptSock);
}

void closeServer(){

  close(fd);

}
};
