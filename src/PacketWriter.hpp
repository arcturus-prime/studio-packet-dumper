#include "NetworkStream.hpp"
#include "NetworkContext.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

enum class PacketId : uint8_t
{
    ID_SET_GLOBALS = 0x81,
    ID_TEACH_DESCRIPTOR_DICTIONARIES = 0x82,
    ID_DATA = 0x83,
    ID_MARKER = 0x84,
    ID_PHYSICS = 0x85,
    ID_TOUCHES = 0x86,
    ID_CHAT_ALL = 0x87,
    ID_CHAT_TEAM = 0x88,
    ID_REPORT_ABUSE = 0x89,
    ID_SUBMIT_TICKET = 0x8A,
    ID_CHAT_GAME = 0x8B,
    ID_CHAT_PLAYER = 0x8C,
    ID_CLUSTER = 0x8D,
    ID_PROTOCOL_MISMATCH = 0x8E,
    ID_PREFERRED_SPAWN_NAME = 0x8F,
    ID_PROTOCOL_SYNC = 0x90,
    ID_SCHEMA_SYNC = 0x91,
    ID_PLACEID_VERIFICATION = 0x92,
    ID_DICTIONARY_FORMAT = 0x93,
    ID_HASH_MISMATCH = 0x94,
    ID_SECURITYKEY_MISMATCH = 0x95,
    ID_REQUEST_STATS = 0x96,
    ID_NEW_SCHEMA = 0x97,
};

enum class IdDataSubId : uint8_t
{
    ID_DELETE_INSTANCE = 0x01,
    ID_CREATE_INSTANCE = 0x02,
    ID_CHANGE_PROPERTY = 0x03,
    ID_MARKER = 0x04,
    ID_PING = 0x05,
    ID_PING_BACK = 0x06,
    ID_EVENT = 0x07,
    ID_CFRAME_ACK = 0x0A,
    ID_JOINDATA = 0x0B,
    ID_UPDATE_CLIENT_QUOTA = 0x0C,
    ID_REPLIC_STREAM_DATA = 0x0D,
    ID_REPLIC_REGION_REMOVAL = 0x0E,
    ID_REPLIC_INSTANCE_REMOVAL = 0x0F,
    ID_TAG = 0x10,
    ID_REPLIC_STATS = 0x11,
    ID_HASH = 0x12,
    ID_REPLIC_ATOMIC = 0x13,
    ID_REPLIC_STREAM_DATA_INFO = 0x14,
};

static inline std::string get_packet_type(uint8_t type)
{
    switch (static_cast<PacketId>(type))
    {
    case PacketId::ID_SET_GLOBALS:
        return "ID_SET_GLOBALS";
    case PacketId::ID_TEACH_DESCRIPTOR_DICTIONARIES:
        return "ID_TEACH_DESCRIPTOR_DICTIONARIES";
    case PacketId::ID_DATA:
        return "ID_DATA";
    case PacketId::ID_MARKER:
        return "ID_MARKER";
    case PacketId::ID_PHYSICS:
        return "ID_PHYSICS";
    case PacketId::ID_TOUCHES:
        return "ID_TOUCHES";
    case PacketId::ID_CHAT_ALL:
        return "ID_CHAT_ALL";
    case PacketId::ID_CHAT_TEAM:
        return "ID_CHAT_TEAM";
    case PacketId::ID_REPORT_ABUSE:
        return "ID_REPORT_ABUSE";
    case PacketId::ID_SUBMIT_TICKET:
        return "ID_SUBMIT_TICKET";
    case PacketId::ID_CHAT_GAME:
        return "ID_CHAT_GAME";
    case PacketId::ID_CHAT_PLAYER:
        return "ID_CHAT_PLAYER";
    case PacketId::ID_CLUSTER:
        return "ID_CLUSTER";
    case PacketId::ID_PROTOCOL_MISMATCH:
        return "ID_PROTOCOL_MISMATCH";
    case PacketId::ID_PREFERRED_SPAWN_NAME:
        return "ID_PREFERRED_SPAWN_NAME";
    case PacketId::ID_PROTOCOL_SYNC:
        return "ID_PROTOCOL_SYNC";
    case PacketId::ID_SCHEMA_SYNC:
        return "ID_SCHEMA_SYNC";
    case PacketId::ID_PLACEID_VERIFICATION:
        return "ID_PLACEID_VERIFICATION";
    case PacketId::ID_DICTIONARY_FORMAT:
        return "ID_DICTIONARY_FORMAT";
    case PacketId::ID_HASH_MISMATCH:
        return "ID_HASH_MISMATCH";
    case PacketId::ID_SECURITYKEY_MISMATCH:
        return "ID_SECURITYKEY_MISMATCH";
    case PacketId::ID_REQUEST_STATS:
        return "ID_REQUEST_STATS";
    case PacketId::ID_NEW_SCHEMA:
        return "ID_NEW_SCHEMA";
    default:
        return "UNKNOWN(0x" + std::string(1, "0123456789ABCDEF"[(type >> 4) & 0xF]) +
               std::string(1, "0123456789ABCDEF"[type & 0xF]) + ")";
    }
}

static inline std::string get_id_data_type(uint8_t type)
{
    switch (static_cast<IdDataSubId>(type))
    {
    case IdDataSubId::ID_DELETE_INSTANCE:
        return "ID_DELETE_INSTANCE";
    case IdDataSubId::ID_CREATE_INSTANCE:
        return "ID_CREATE_INSTANCE";
    case IdDataSubId::ID_CHANGE_PROPERTY:
        return "ID_CHANGE_PROPERTY";
    case IdDataSubId::ID_MARKER:
        return "ID_MARKER";
    case IdDataSubId::ID_PING:
        return "ID_PING";
    case IdDataSubId::ID_PING_BACK:
        return "ID_PING_BACK";
    case IdDataSubId::ID_EVENT:
        return "ID_EVENT";
    case IdDataSubId::ID_CFRAME_ACK:
        return "ID_CFRAME_ACK";
    case IdDataSubId::ID_JOINDATA:
        return "ID_JOINDATA";
    case IdDataSubId::ID_UPDATE_CLIENT_QUOTA:
        return "ID_UPDATE_CLIENT_QUOTA";
    case IdDataSubId::ID_REPLIC_STREAM_DATA:
        return "ID_REPLIC_STREAM_DATA";
    case IdDataSubId::ID_REPLIC_REGION_REMOVAL:
        return "ID_REPLIC_REGION_REMOVAL";
    case IdDataSubId::ID_REPLIC_INSTANCE_REMOVAL:
        return "ID_REPLIC_INSTANCE_REMOVAL";
    case IdDataSubId::ID_TAG:
        return "ID_TAG";
    case IdDataSubId::ID_REPLIC_STATS:
        return "ID_REPLIC_STATS";
    case IdDataSubId::ID_HASH:
        return "ID_HASH";
    case IdDataSubId::ID_REPLIC_ATOMIC:
        return "ID_REPLIC_ATOMIC";
    case IdDataSubId::ID_REPLIC_STREAM_DATA_INFO:
        return "ID_REPLIC_STREAM_DATA_INFO";
    default:
        return "UNKNOWN(0x" + std::string(1, "0123456789ABCDEF"[(type >> 4) & 0xF]) +
               std::string(1, "0123456789ABCDEF"[type & 0xF]) + ")";
    }
}

static inline void print_id_ping(NetworkContext& context, NetworkStream& stream)
{
    (void) context;

    uint8_t version = *stream.read_u8();

    uint64_t timestamp = 0;
    if (version <= 1)
    {
        timestamp = *stream.read_u64_be();
        std::cout << "Timestamp: " << timestamp << std::endl;
    }
    else if (version == 2)
    {
        std::cout << "Int1 (Unknown): " << *stream.read_u32_be() << std::endl;
        timestamp = *stream.read_u32_be();
        std::cout << "Timestamp: " << timestamp << std::endl;
        std::cout << "FPS1: " << *stream.read_f32_be() << std::endl;
        std::cout << "FPS2: " << *stream.read_f32_be() << std::endl;
        std::cout << "FPS3: " << *stream.read_f32_be() << std::endl;
    }
    else
    {
        std::cout << "Invalid version" << std::endl;
    }

    uint32_t sendStats = *stream.read_u32_be();
    uint32_t extraStats = *stream.read_u32_be();

    if ((timestamp & 0x20) != 0)
    {
        extraStats ^= 0xFFFFFFFF;
    }

    std::cout << "Send Stats: " << std::hex << std::setfill('0') << sendStats << std::endl;
    std::cout << "Extra Stats: " << std::hex << std::setfill('0') << extraStats << std::endl;
}

static inline void print_id_ping_back(NetworkContext& context, NetworkStream& stream)
{
    (void) context;

    std::cout << "IsPingBack: " << *stream.read_bool_byte() << std::endl;

    uint64_t timestamp = *stream.read_u64_be();
    std::cout << "Timestamp: " << timestamp << std::endl;

    uint32_t sendStats = *stream.read_u32_be();
    uint32_t extraStats = *stream.read_u32_be();

    if ((timestamp & 0x20) != 0)
    {
        extraStats ^= 0xFFFFFFFF;
    }

    std::cout << "Send Stats: " << std::hex << std::setfill('0') << sendStats << std::endl;
    std::cout << "Extra Stats: " << std::hex << std::setfill('0') << extraStats << std::endl;
}

static inline void print_id_event(NetworkContext& context, NetworkStream& stream)
{
    (void) context;

    uint64_t peerId = *stream.read_varuint64();

    std::cout << "Peer ID: 0x" << std::hex << (uint32_t) peerId << std::endl;
    if (peerId == 0) {
        std::cout << "IsNull: true" << std::endl;

        return;
    }

    std::cout << "IsNull: false" << std::endl;
    std::cout << "Reference ID: " << std::hex << *stream.read_u32_le() << std::endl;
    std::cout << "Event ID: " << std::hex << *stream.read_u16_be() << std::endl;
}

static inline void print_id_data(NetworkContext& context, NetworkStream& stream)
{
    (void) context;

    uint8_t sub_id = *stream.read_u8();

    std::cout << "Type: " << get_id_data_type(sub_id) << std::endl;

    IdDataSubId data_id = static_cast<IdDataSubId>(sub_id);
    if (data_id == IdDataSubId::ID_PING)
    {
        print_id_ping(context, stream);
        stream.read_u8();
    }
    else if (data_id == IdDataSubId::ID_PING_BACK)
    {
        print_id_ping_back(context, stream);
        stream.read_u8();
    }
    else if (data_id == IdDataSubId::ID_EVENT)
    {
        print_id_event(context, stream);
        stream.read_u8();
    }
    else
    {
        std::cout << "Data: " << std::endl;
        while (auto out_byte = stream.read_u8())
        {
            std::cout << std::setw(2) << std::hex << std::setfill('0') << (uint32_t) *out_byte << " ";
        }
    }
}

static inline void handle_packet(NetworkContext& context, NetworkStream& stream)
{
    uint8_t id = *stream.read_u8();
    std::cout << "Id: " << get_packet_type(id) << std::endl;

    PacketId packet_id = static_cast<PacketId>(id);
    if (packet_id == PacketId::ID_DATA)
    {
        print_id_data(context, stream);
    }
}
