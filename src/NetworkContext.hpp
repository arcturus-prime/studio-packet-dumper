#include <cstdint>
#include <string>
#include <vector>

struct NetworkSchemaEnum
{
    std::string name;
    uint8_t bit_size;
    uint16_t network_id;

    NetworkSchemaEnum(std::string& name, uint8_t bit_size, uint16_t network_id)
        : name(name), bit_size(bit_size), network_id(network_id)
    {
    }
};

struct NetworkSchema
{
    std::vector<NetworkSchemaEnum> enums;

    inline void clear() {
        this->enums.clear();
    }
};

struct NetworkContext
{
    NetworkSchema schema;
};
