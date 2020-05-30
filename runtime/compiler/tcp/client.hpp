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
#ifndef CLIENT_H
#define CLIENT_H

#include "socket.hpp"
#include "out/compile.pb.h"

#define PORT 38401

//currently going to be hardcoded to use localhost as server address
class Client {


  public:
  int fd;
  TCP::Stream stream;
  
  Client(){
  fd = setupConnection();
  stream.startStream(fd);
  }

int setupConnection(){
  int socketFd, connectSock;
  struct hostent *host;
  const char *serverName = "localhost";
  struct sockaddr_in server;
  
  socketFd = socketInit();

  host = gethostbyname(serverName);
  if(host == NULL){
    //not actually sure if this what we want, error here is in h_errno not errno so...
     handleError(1, socketFd, "Could not resolve hostname");
  }

   memset(&server, 0, sizeof(server));
   memcpy(&server.sin_addr.s_addr, host->h_addr, host->h_length);
   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);


   struct timeval timeout;      
   timeout.tv_sec = 300; //5min timeout on send and receive
    timeout.tv_usec = 0;

    if (setsockopt (socketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
	  handleError(1, socketFd, "setsockopt failed\n");

	if (setsockopt (socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
					sizeof(timeout)) < 0)
	  handleError(1, socketFd, "setsockopt failed\n");
	  
  connectSock = connect(socketFd, (struct sockaddr *) &server, sizeof(server));
  if(connectSock < 0){
    handleError(1, socketFd, "Could not connect client to server");
	return(-1);
  }
  
  return(socketFd);
}
  
  void writeClient(TCP::ClientMsgType type, char * text, int nameLen){

	std::cout << "write client setting text: " << text << " with custom len: " << nameLen ;
	
	//stream.clientmsg.set_type(type);
	//  stream.clientmsg.set_text(text);
	//stream.streamWrite(stream.clientmsg, fd);
	if(nameLen == 0){
	  send(fd, text, strlen(text), 0);
	} else {
	  send(fd, text, nameLen, 0);
	}
}

  //"custom" serialization interfaces for reading native types
  int readInt(){
	return stream.streamReadInt(fd);
  }
  
  std::string readString(){

	return stream.streamReadString(fd);

}

 void closeClient(){
   close(fd);
 }
 
};
#endif //CLIENT_H 
