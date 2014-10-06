/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ObjectGuid.h"

#include "World.h"
#include "ObjectMgr.h"

#include <sstream>

char const* ObjectGuid::GetTypeName(GuidType high)
{
    switch (high)
    {
        case GUIDTYPE_ITEM:         return "Item";
        case GUIDTYPE_PLAYER:       return "Player";
        case GUIDTYPE_GAMEOBJECT:   return "Gameobject";
        case GUIDTYPE_TRANSPORT:    return "Transport";
        case GUIDTYPE_CREATURE:         return "Creature";
        case GUIDTYPE_PET:          return "Pet";
        case GUIDTYPE_VEHICLE:      return "Vehicle";
        case GUIDTYPE_DYNAMICOBJECT: return "DynObject";
        case GUIDTYPE_CORPSE:       return "Corpse";
        //case GUIDTYPE_MO_TRANSPORT: return "MoTransport";
        case GUIDTYPE_RAIDGROUP:     return "InstanceID";
        case GUIDTYPE_PARTY:        return "Group";
        case GUIDTYPE_PVPQUEUEGROUP: return "Battleground";
        default:
            return "<unknown>";
    }
}

std::string ObjectGuid::GetString() const
{
    std::ostringstream str;
    str << GetTypeName();

    if (IsPlayer())
    {
        std::string name;
        if (sObjectMgr.GetPlayerNameByGUID(*this, name))
            str << " " << name;
    }

    str << " (";
    if (HasEntry())
        str << (IsPet() ? "Petnumber: " : "Entry: ") << GetEntry() << " ";
    str << "Guid: " << GetCounter() << ")";
    return str.str();
}

template<GuidType high>
uint32 ObjectGuidGenerator<high>::Generate()
{
    if (m_nextGuid >= ObjectGuid::GetMaxCounter(high) - 1)
    {
        sLog.outError("%s guid overflow!! Can't continue, shutting down server. ", ObjectGuid::GetTypeName(high));
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_nextGuid++;
}

ByteBuffer& operator<< (ByteBuffer& buf, ObjectGuid const& guid)
{
    buf << uint64(guid.GetRawValue());
    return buf;
}

ByteBuffer& operator>>(ByteBuffer& buf, ObjectGuid& guid)
{
    guid.Set(buf.read<uint64>());
    return buf;
}

ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid)
{
    buf.append(guid.m_packedGuid);
    return buf;
}

ByteBuffer& operator>>(ByteBuffer& buf, PackedGuidReader const& guid)
{
    guid.m_guidPtr->Set(buf.readPackGUID());
    return buf;
}

template uint32 ObjectGuidGenerator<GUIDTYPE_ITEM>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_PLAYER>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_GAMEOBJECT>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_TRANSPORT>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_CREATURE>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_PET>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_VEHICLE>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_DYNAMICOBJECT>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_CORPSE>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_RAIDGROUP>::Generate();
template uint32 ObjectGuidGenerator<GUIDTYPE_PARTY>::Generate();
