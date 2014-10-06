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

#ifndef MANGOS_OBJECT_GUID_H
#define MANGOS_OBJECT_GUID_H

#include "Common.h"
#include "ByteBuffer.h"

#include <functional>

enum TypeID
{
    TYPEID_OBJECT        = 0,
    TYPEID_ITEM          = 1,
    TYPEID_CONTAINER     = 2,
    TYPEID_UNIT          = 3,
    TYPEID_PLAYER        = 4,
    TYPEID_GAMEOBJECT    = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE        = 7
};

#define MAX_TYPE_ID        8

enum TypeMask
{
    TYPEMASK_OBJECT         = 0x0001,
    TYPEMASK_ITEM           = 0x0002,
    TYPEMASK_CONTAINER      = 0x0004,
    TYPEMASK_UNIT           = 0x0008,                       // players also have it
    TYPEMASK_PLAYER         = 0x0010,
    TYPEMASK_GAMEOBJECT     = 0x0020,
    TYPEMASK_DYNAMICOBJECT  = 0x0040,
    TYPEMASK_CORPSE         = 0x0080,

    // used combinations in Player::GetObjectByTypeMask (TYPEMASK_UNIT case ignore players in call)
    TYPEMASK_CREATURE_OR_GAMEOBJECT = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT,
    TYPEMASK_CREATURE_GAMEOBJECT_OR_ITEM = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM,
    TYPEMASK_CREATURE_GAMEOBJECT_PLAYER_OR_ITEM = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM | TYPEMASK_PLAYER,

    TYPEMASK_WORLDOBJECT = TYPEMASK_UNIT | TYPEMASK_PLAYER | TYPEMASK_GAMEOBJECT | TYPEMASK_DYNAMICOBJECT | TYPEMASK_CORPSE,
};

enum GuidType
{
    GUIDTYPE_NULL             = 0,
    GUIDTYPE_UNIQ             = 1,
    GUIDTYPE_PLAYER           = 2,
    GUIDTYPE_ITEM             = 3,
    GUIDTYPE_STATICDOOR       = 4,
    GUIDTYPE_TRANSPORT        = 5,
    GUIDTYPE_CONVERSATION     = 6,
    GUIDTYPE_CREATURE         = 7,
    GUIDTYPE_VEHICLE          = 8,
    GUIDTYPE_PET              = 9,
    GUIDTYPE_GAMEOBJECT       = 10,
    GUIDTYPE_DYNAMICOBJECT    = 11,
    GUIDTYPE_AREATRIGGER      = 12,
    GUIDTYPE_CORPSE           = 13,
    GUIDTYPE_LOOTOBJECT       = 14,
    GUIDTYPE_SCENEOBJECT      = 15,
    GUIDTYPE_SCENARIO         = 16,
    GUIDTYPE_AIGROUP          = 17,
    GUIDTYPE_DYNAMICDOOR      = 18,
    GUIDTYPE_CLIENTACTOR      = 19,
    GUIDTYPE_VIGNETTE         = 20,
    GUIDTYPE_CALLFORHELP      = 21,
    GUIDTYPE_AIRESOURCE       = 22,
    GUIDTYPE_AILOCK           = 23,
    GUIDTYPE_AILOCKTICKET     = 24,
    GUIDTYPE_CHATCHANNEL      = 25,
    GUIDTYPE_PARTY            = 26,
    GUIDTYPE_GUILD            = 27,
    GUIDTYPE_WOWACCOUNT       = 28,
    GUIDTYPE_BNETACCOUNT      = 29,
    GUIDTYPE_GMTASK           = 30,
    GUIDTYPE_MOBILESESSION    = 31,
    GUIDTYPE_RAIDGROUP        = 32,
    GUIDTYPE_SPELL            = 33,
    GUIDTYPE_MAIL             = 34,
    GUIDTYPE_WEBOBJ           = 35,
    GUIDTYPE_LFGOBJECT        = 36,
    GUIDTYPE_LFGLIST          = 37,
    GUIDTYPE_USERROUTER       = 38,
    GUIDTYPE_PVPQUEUEGROUP    = 39,
    GUIDTYPE_USERCLIENT       = 40,
    GUIDTYPE_PETBATTLE        = 41,
    GUIDTYPE_UNIQUEUSERCLIENT = 42,
    GUIDTYPE_BATTLEPET        = 43
};

class ObjectGuid;
class PackedGuid;

struct PackedGuidReader
{
    explicit PackedGuidReader(ObjectGuid& guid) : m_guidPtr(&guid) {}
    ObjectGuid* m_guidPtr;
};

#define NUM_GUID_BYTES sizeof(uint64)

class MANGOS_DLL_SPEC ObjectGuid
{
    public:                                                 // constructors
        ObjectGuid() : m_guidLow(0), m_guidHigh(0) {}
        explicit ObjectGuid(uint64 guid) : m_guidLow(guid), m_guidHigh(0) {}
        ObjectGuid(GuidType type, uint32 entry, uint32 counter) : m_guidLow(m_guidLow | uint64(counter)), m_guidHigh(m_guidHigh | (uint64(type) << 58) | (uint64(entry) << 6)) {}
        ObjectGuid(GuidType type, uint32 counter) : m_guidLow(m_guidLow | uint64(counter)), m_guidHigh(m_guidHigh | (uint64(type) << 58)) {}

        operator uint64() const { return m_guidLow; }
    private:
        explicit ObjectGuid(uint32 const&);                 // no implementation, used for catch wrong type assign
        ObjectGuid(GuidType, uint32, uint64 counter);       // no implementation, used for catch wrong type assign
        ObjectGuid(GuidType, uint64 counter);               // no implementation, used for catch wrong type assign

    public:                                                 // modifiers
        PackedGuidReader ReadAsPacked() { return PackedGuidReader(*this); }

        void Set(uint64 guid) { m_guidLow = guid; }
        void Clear() { m_guidLow = 0; }

        PackedGuid WriteAsPacked() const;
    public:                                                 // accessors
        uint64   GetRawValue() const { return m_guidLow; }
        GuidType GetType() const
        {
            return  GuidType(m_guidHigh >> 58);//GuidType(IsLargeHigh(high) ? high :
                //(m_guid >> 52) & 0xFFF);
        }
        uint32   GetEntry() const { return HasEntry() ? uint32((m_guidHigh & 0xFFFFFF) >> 6) : 0; }
        uint32   GetCounter()  const
        {
            return uint64(m_guidLow & 0xFFFFFFFFFF);  // TODO: switch to 40 bits, but this needs rewrite code to use uint64 instead uint32
        }

        static uint64 GetMaxCounter(GuidType high)
        {
            return 0xFFFFFFFFFF;    // TODO: switch to 40 bits
        }

        uint64 GetMaxCounter() const { return GetMaxCounter(GetType()); }

        bool IsEmpty()             const { return m_guidLow == 0 || m_guidHigh == 0; }
        bool IsCreature()          const { return GetType() == GUIDTYPE_CREATURE; }
        bool IsPet()               const { return GetType() == GUIDTYPE_PET; }
        bool IsVehicle()           const { return GetType() == GUIDTYPE_VEHICLE; }
        bool IsCreatureOrPet()     const { return IsCreature() || IsPet();                    }
        bool IsCreatureOrVehicle() const { return IsCreature() || IsVehicle();                }
        bool IsAnyTypeCreature()   const { return IsCreature() || IsPet() || IsVehicle();     }
        bool IsPlayer()            const { return !IsEmpty() && GetType() == GUIDTYPE_PLAYER; }
        bool IsUnit()              const { return IsAnyTypeCreature() || IsPlayer();          }
        bool IsItem()              const { return GetType() == GUIDTYPE_ITEM; }
        bool IsGameObject()        const { return GetType() == GUIDTYPE_GAMEOBJECT; }
        bool IsDynamicObject()     const { return GetType() == GUIDTYPE_DYNAMICOBJECT; }
        bool IsCorpse()            const { return GetType() == GUIDTYPE_CORPSE; }
        bool IsTransport()         const { return GetType() == GUIDTYPE_TRANSPORT; }
        bool IsMOTransport()       const { return false; }// GetType() == GUIDTYPE_MO_TRANSPORT; }
        bool IsInstance()          const { return GetType() == GUIDTYPE_RAIDGROUP; }
        bool IsGroup()             const { return GetType() == GUIDTYPE_PARTY; }
        bool IsBattleGround()      const { return GetType() == GUIDTYPE_PVPQUEUEGROUP; }
        bool IsGuild()             const { return GetType() == GUIDTYPE_GUILD; }

        static TypeID GetTypeId(GuidType type)
        {
            switch (type)
            {
                case GUIDTYPE_ITEM:         return TYPEID_ITEM;
                    // case GUIDTYPE_ITEM:    return TYPEID_CONTAINER; GUIDTYPE_ITEM==GUIDTYPE_ITEM currently
                case GUIDTYPE_CREATURE:         return TYPEID_UNIT;
                case GUIDTYPE_PET:          return TYPEID_UNIT;
                case GUIDTYPE_PLAYER:       return TYPEID_PLAYER;
                case GUIDTYPE_GAMEOBJECT:   return TYPEID_GAMEOBJECT;
                case GUIDTYPE_DYNAMICOBJECT: return TYPEID_DYNAMICOBJECT;
                case GUIDTYPE_CORPSE:       return TYPEID_CORPSE;
                //case GUIDTYPE_MO_TRANSPORT: return TYPEID_GAMEOBJECT;
                case GUIDTYPE_VEHICLE:      return TYPEID_UNIT;
                    // unknown
                case GUIDTYPE_RAIDGROUP:
                case GUIDTYPE_PARTY:
                default:                    return TYPEID_OBJECT;
            }
        }

        TypeID GetTypeId() const { return GetTypeId(GetType()); }

        bool operator!() const { return IsEmpty(); }
        bool operator== (ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
        bool operator!= (ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
        bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

        uint8& operator[] (uint8 index)
        {
            MANGOS_ASSERT(index < NUM_GUID_BYTES);

#if MANGOS_ENDIAN == MANGOS_LITTLEENDIAN
            return m_guidBytes[index];
#else
            return m_guidBytes[NUM_GUID_BYTES - 1 - index];
#endif
        }

        uint8 const& operator[] (uint8 index) const
        {
            MANGOS_ASSERT(index < NUM_GUID_BYTES);

#if MANGOS_ENDIAN == MANGOS_LITTLEENDIAN
            return m_guidBytes[index];
#else
            return m_guidBytes[NUM_GUID_BYTES - 1 - index];
#endif
        }

    public:                                                 // accessors - for debug
        static char const* GetTypeName(GuidType high);
        char const* GetTypeName() const { return !IsEmpty() ? GetTypeName(GetType()) : "None"; }
        std::string GetString() const;

    private:                                                // internal functions
        static bool HasEntry(GuidType high)
        {
            switch (high)
            {
                case GUIDTYPE_ITEM:
                case GUIDTYPE_PLAYER:
                case GUIDTYPE_DYNAMICOBJECT:
                case GUIDTYPE_CORPSE:
                //case GUIDTYPE_MO_TRANSPORT:
                case GUIDTYPE_RAIDGROUP:
                case GUIDTYPE_PARTY:
                    return false;
                case GUIDTYPE_GAMEOBJECT:
                case GUIDTYPE_TRANSPORT:
                case GUIDTYPE_CREATURE:
                case GUIDTYPE_PET:
                case GUIDTYPE_VEHICLE:
                case GUIDTYPE_PVPQUEUEGROUP:
                default:
                    return true;
            }
        }

        bool HasEntry() const { return HasEntry(GetType()); }

        static bool IsLargeHigh(GuidType high)
        {
            switch(high)
            {
            case GUIDTYPE_GUILD:
                    return true;
                default:
                    return false;
            }
        }

        bool IsLargeHigh() const { return IsLargeHigh(GetType()); }

    private:                                                // fields
        union
        {
            uint64 m_guidLow;
            uint64 m_guidHigh;
            uint8 m_guidBytes[NUM_GUID_BYTES];
        };
};

// Some Shared defines
typedef std::set<ObjectGuid> GuidSet;
typedef std::list<ObjectGuid> GuidList;
typedef std::vector<ObjectGuid> GuidVector;

// minimum buffer size for packed guid is 9 bytes
#define PACKED_GUID_MIN_BUFFER_SIZE 9

class PackedGuid
{
    friend ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid);

    public:                                                 // constructors
        explicit PackedGuid() : m_packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { m_packedGuid.appendPackGUID(0); }
        explicit PackedGuid(uint64 const& guid) : m_packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { m_packedGuid.appendPackGUID(guid); }
        explicit PackedGuid(ObjectGuid const& guid) : m_packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { m_packedGuid.appendPackGUID(guid.GetRawValue()); }

    public:                                                 // modifiers
        void Set(uint64 const& guid) { m_packedGuid.wpos(0); m_packedGuid.appendPackGUID(guid); }
        void Set(ObjectGuid const& guid) { m_packedGuid.wpos(0); m_packedGuid.appendPackGUID(guid.GetRawValue()); }

    public:                                                 // accessors
        size_t size() const { return m_packedGuid.size(); }

    private:                                                // fields
        ByteBuffer m_packedGuid;
};

template<GuidType high>
class ObjectGuidGenerator
{
    public:                                                 // constructors
        explicit ObjectGuidGenerator(uint32 start = 1) : m_nextGuid(start) {}

    public:                                                 // modifiers
        void Set(uint32 val) { m_nextGuid = val; }
        uint32 Generate();

    public:                                                 // accessors
        uint32 GetNextAfterMaxUsed() const { return m_nextGuid; }

    private:                                                // fields
        uint32 m_nextGuid;
};

ByteBuffer& operator<< (ByteBuffer& buf, ObjectGuid const& guid);
ByteBuffer& operator>> (ByteBuffer& buf, ObjectGuid&       guid);

ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid);
ByteBuffer& operator>> (ByteBuffer& buf, PackedGuidReader const& guid);

inline PackedGuid ObjectGuid::WriteAsPacked() const { return PackedGuid(*this); }

HASH_NAMESPACE_START

template<>
class hash<ObjectGuid>
{
    public:

        size_t operator()(ObjectGuid const& key) const
        {
            return hash<uint64>()(key.GetRawValue());
        }
};

HASH_NAMESPACE_END

// deprecated
#define DEFINE_READGUIDMASK(T1, T2) template <T1> \
    void ByteBuffer::ReadGuidMask(ObjectGuid& guid) \
    { \
        uint8 maskArr[] = { T2 }; \
        for (uint8 i = 0; i < countof(maskArr); ++i) \
            guid[maskArr[i]] = ReadBit(); \
    }

#define DEFINE_WRITEGUIDMASK(T1, T2) template <T1> \
    void ByteBuffer::WriteGuidMask(ObjectGuid guid) \
    { \
        uint8 maskArr[] = { T2 }; \
        for (uint8 i = 0; i < countof(maskArr); ++i) \
            WriteBit(guid[maskArr[i]]); \
    }

#define DEFINE_READGUIDBYTES(T1, T2) template <T1> \
    void ByteBuffer::ReadGuidBytes(ObjectGuid& guid) \
    { \
        uint8 maskArr[] = { T2 }; \
        for (uint8 i = 0; i < countof(maskArr); ++i) \
            if (guid[maskArr[i]] != 0) \
                guid[maskArr[i]] ^= read<uint8>(); \
    }

#define DEFINE_WRITEGUIDBYTES(T1, T2) template <T1> \
    void ByteBuffer::WriteGuidBytes(ObjectGuid guid) \
    { \
        uint8 maskArr[] = { T2 }; \
        for (uint8 i = 0; i < countof(maskArr); ++i) \
            if (guid[maskArr[i]] != 0) \
                (*this) << uint8(guid[maskArr[i]] ^ 1); \
    }

DEFINE_READGUIDMASK(BITS_1, BIT_VALS_1)
DEFINE_READGUIDMASK(BITS_2, BIT_VALS_2)
DEFINE_READGUIDMASK(BITS_3, BIT_VALS_3)
DEFINE_READGUIDMASK(BITS_4, BIT_VALS_4)
DEFINE_READGUIDMASK(BITS_5, BIT_VALS_5)
DEFINE_READGUIDMASK(BITS_6, BIT_VALS_6)
DEFINE_READGUIDMASK(BITS_7, BIT_VALS_7)
DEFINE_READGUIDMASK(BITS_8, BIT_VALS_8)

DEFINE_WRITEGUIDMASK(BITS_1, BIT_VALS_1)
DEFINE_WRITEGUIDMASK(BITS_2, BIT_VALS_2)
DEFINE_WRITEGUIDMASK(BITS_3, BIT_VALS_3)
DEFINE_WRITEGUIDMASK(BITS_4, BIT_VALS_4)
DEFINE_WRITEGUIDMASK(BITS_5, BIT_VALS_5)
DEFINE_WRITEGUIDMASK(BITS_6, BIT_VALS_6)
DEFINE_WRITEGUIDMASK(BITS_7, BIT_VALS_7)
DEFINE_WRITEGUIDMASK(BITS_8, BIT_VALS_8)

DEFINE_READGUIDBYTES(BITS_1, BIT_VALS_1)
DEFINE_READGUIDBYTES(BITS_2, BIT_VALS_2)
DEFINE_READGUIDBYTES(BITS_3, BIT_VALS_3)
DEFINE_READGUIDBYTES(BITS_4, BIT_VALS_4)
DEFINE_READGUIDBYTES(BITS_5, BIT_VALS_5)
DEFINE_READGUIDBYTES(BITS_6, BIT_VALS_6)
DEFINE_READGUIDBYTES(BITS_7, BIT_VALS_7)
DEFINE_READGUIDBYTES(BITS_8, BIT_VALS_8)

DEFINE_WRITEGUIDBYTES(BITS_1, BIT_VALS_1)
DEFINE_WRITEGUIDBYTES(BITS_2, BIT_VALS_2)
DEFINE_WRITEGUIDBYTES(BITS_3, BIT_VALS_3)
DEFINE_WRITEGUIDBYTES(BITS_4, BIT_VALS_4)
DEFINE_WRITEGUIDBYTES(BITS_5, BIT_VALS_5)
DEFINE_WRITEGUIDBYTES(BITS_6, BIT_VALS_6)
DEFINE_WRITEGUIDBYTES(BITS_7, BIT_VALS_7)
DEFINE_WRITEGUIDBYTES(BITS_8, BIT_VALS_8)

#endif
