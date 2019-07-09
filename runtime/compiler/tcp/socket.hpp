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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include "stream.hpp"

#define PORT 38400

int socketInit();
void handleError(int close, int fd, const char *errorMsg);
TCP::Stream initStream();


//intializes a stream object for the server/client
TCP::Stream initStream(){

  TCP::Stream stream;
  return(stream);

  }

//ret socket file descriptor
int socketInit(){

  int socketFd;

  socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketFd < 0){
    handleError(0, 0, "Cannot open socket");
  }
  return(socketFd);
}

//generic handle error, maybe close file descriptor, throws an error
void handleError(int closeFlag, int fd, const char *errorMsg){

  if(closeFlag == 1){
    close(fd);
  }
  perror(errorMsg);
  //since perror handles the message the exception will not have a message
  throw "";
  //  exit(-1);

}
