#ifndef _CLOUDBUS_BASICTYPES_H_
#define _CLOUDBUS_BASICTYPES_H_
#include "../../os/os_if.h"
#include <string>
#include <memory.h>

#define BEGIN_DDS_NAMESPACE namespace dds{
#define END_DDS_NAMESPACE   }

namespace dds
{
    typedef bool Boolean;

    typedef Boolean& Boolean_out;
    typedef Boolean* Boolean_ptr;

    typedef Int8 Char;
    typedef Char& Char_out;
    typedef Char* Char_ptr;

    typedef UInt8 Octet;
    typedef Octet& Octet_out;
    typedef Octet* Octet_ptr;

    typedef Int16 Short;
    typedef Short& Short_out;
    typedef Short* Short_ptr;

    typedef UInt16 UShort;
    typedef UShort& UShort_out;
    typedef UShort* UShort_ptr;

    typedef Int32 Long;
    typedef Long& Long_out;
    typedef Long* Long_ptr;

    typedef UInt32 ULong;
    typedef ULong& ULong_out;
    typedef ULong* ULong_ptr;

    typedef float Float;
    typedef Float& Float_out;
    typedef Float* Float_ptr;

    typedef double Double;
    typedef Double& Double_out;
    typedef Double* Double_ptr;

    typedef void* Opaque;

    typedef Int64 LongLong;
    typedef LongLong& LongLong_out;
    typedef LongLong* LongLong_ptr;

    typedef UInt64 ULongLong;
    typedef ULongLong& ULongLong_out;
    typedef ULongLong* ULongLong_ptr;

	struct DataSeq
	{
		ULongLong seq;

		DataSeq();

		DataSeq(ULongLong s);

		DataSeq& operator =(const DataSeq& s);

		DataSeq& operator = (ULongLong s);

		bool operator == (const DataSeq& rhs);

		bool operator == (ULongLong s);

		bool operator < (const DataSeq& rhs);

		bool operator < (ULongLong s);

		bool operator >(const DataSeq& rhs);

		bool operator >(ULongLong s);

		DataSeq operator ++();

		const DataSeq operator ++(int);

		DataSeq operator --();

		const DataSeq operator --(int);

		DataSeq& operator +(const DataSeq& rhs);

		DataSeq& operator +(ULongLong s);

		DataSeq& operator -(const DataSeq& rhs);

		DataSeq& operator -(ULongLong s);

	};

	struct NodeId
	{
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];

		NodeId();

		NodeId& operator = (const NodeId& rhs);
	};

	bool operator ==(const NodeId& lhs, const NodeId& rhs);

	bool operator <(const NodeId& lhs, const NodeId& rhs);

}




#if (HERMES_OS != HERMES_OS_WINDOWS_NT)

#define SOCKET			int
#define SOCKADDR		sockaddr
#define SOCKADDR_IN 	sockaddr_in

#define SOCKET_ERROR	-1
#define INVALID_SOCKET	-1

#include "uuid.h"

struct GUID
{

	GUID()
	{
		uuid_clear(m_uuid);
	}

	GUID(const GUID& rhs)
	{
		uuid_copy(m_uuid, rhs.m_uuid);
	}

	bool operator == (const GUID& rhs)
	{
		return memcmp(&this->m_uuid, &rhs.m_uuid, 16) == 0;
	}

	uuid_t m_uuid;

};

#endif

#endif
