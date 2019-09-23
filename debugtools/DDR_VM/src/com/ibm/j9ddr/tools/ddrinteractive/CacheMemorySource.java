/*******************************************************************************
 * Copyright (c) 2019, 2019 IBM Corp. and others
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
package com.ibm.j9ddr.tools.ddrinteractive;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import com.ibm.j9ddr.corereaders.memory.MemoryFault;
import com.ibm.j9ddr.corereaders.Platform;
import com.ibm.j9ddr.corereaders.memory.AbstractMemory;
import com.ibm.j9ddr.corereaders.memory.IMemorySource;
import com.ibm.j9ddr.corereaders.memory.ProtectedMemoryRange;
import java.nio.BufferUnderflowException;

/**
 * Serves as a ByteBuffer backed memory source.
 *
 * @see com.ibm.j9ddr.tools.ddrinteractive.CacheMemory
 *
 * @author knewbury01
 *
 */

public class CacheMemorySource extends ProtectedMemoryRange implements IMemorySource
{
	private ByteBuffer source;

	public CacheMemorySource(long address,
							 ByteBuffer source)
	{
		super(address,(long)source.capacity());
		this.source = source;
	}

	//
	// Since the src is byte buffer backed
	// it makes most sense to expose the following accessor
	// methods from the src implementation
	// as opposed to a getBytes (rewrapped) result
	//
	public int getUnsignedShort(long address) throws MemoryFault {
		int index = checkIndex(address, Short.BYTES);
		if (source.order() == ByteOrder.BIG_ENDIAN){
			return ((source.get(index) & 0xFF) << 8) | (source.get(index + 1) & 0xFF);
		} else {
			return ((source.get(index + 1) & 0xFF) << 8) | (source.get(index) & 0xFF);
		}
	}

	public short getShort(long address) throws MemoryFault {
		return source.getShort(checkIndex(address, Short.BYTES));
	}

	public int getInt(long address) throws MemoryFault {
		return source.getInt(checkIndex(address, Integer.BYTES));
	}

	public double getDouble(long address) throws MemoryFault {
		return source.getDouble(checkIndex(address, Double.BYTES));
	}

	public long getLong(long address) throws MemoryFault {
		return source.getLong(checkIndex(address, Long.BYTES));
	}

	public byte getByte(long address) throws MemoryFault {
		return source.get(checkIndex(address, 1));
	}

	@Override
	public int getBytes(long address, byte[] buffer, int offset, int length)
		throws MemoryFault
	{
		int index = checkIndex(address, length);
		for (int i = 0; i < length; i++) {
			buffer[i] = source.get(index + i);
		}
		return length;
	}

	private int checkIndex(long address, int byteCount) throws MemoryFault {
		if (byteCount < 0) {
			throw new IllegalArgumentException();
		}

		long maxAddress = baseAddress + Math.min(getSize(), Integer.MAX_VALUE) - byteCount;

		if ((address < baseAddress) || (address > maxAddress)) {
			throw new MemoryFault(address);
		}
		return (int) (address - baseAddress);
	}

	public ByteOrder getByteOrder() {
		return source.order();
	}

	@Override
	public String getName() {
		return null;
	}

	@Override
	public int getAddressSpaceId() {
		return 0;
	}
}
