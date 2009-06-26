/*
Copyright (C) 2009  Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _NETWORKDATA_HPP_
#define _NETWORKDATA_HPP_

namespace backlot
{
	enum PacketType
	{
		EPT_Ready = 1,
		EPT_InitialData,
		EPT_MapChange,
		EPT_EntityCreated,
		EPT_EntityDeleted,
		EPT_ActivateEntity,
		EPT_DeactivateEntity,
		EPT_Rotation,
		EPT_Keys,
		EPT_Update,
		EPT_UpdateReceived
	};
	enum KeyMask
	{
		EKM_Up = 0x0001,
		EKM_Down = 0x0002,
		EKM_Left = 0x0004,
		EKM_Right = 0x0008,
		EKM_Move = 0x000F,
		EKM_Shoot = 0x0010
	};
}

#endif
