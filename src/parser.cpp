#include "parser.h"
#include <iostream>

std::string decode_name(const std::vector<uint8_t>& buffer, size_t& offset) {
    std::string name;
    size_t original_offset = offset;
    bool jumped = false;

    while (true) {
        uint8_t len = buffer[offset];

        if ((len & 0xC0) == 0xC0) {         // Check if this is a compressesd label (starts with bits '11') RFC 1035 section 4.1.4
            // Read 14-bit pointer (last 6 bits of len + next byte)
            uint16_t pointer = ((len & 0x3F) << 8) | buffer[offset + 1];    
            if (!jumped) original_offset = offset + 2;
            offset = pointer;
            jumped = true;
            continue;
        }


        if (len == 0) {
            offset++;
            break;
        }

        offset += 1;
        name.append(reinterpret_cast<const char*>(&buffer[offset]), len);
        name.append(".");
        offset += len;
    }
    if(jumped){
        offset = original_offset;
    }
    return name;
}

bool parse_answers(const std::vector<uint8_t>& response, size_t offset, uint16_t ancount) {
    bool isValid = false;
    for (int i = 0; i < ancount; ++i) {
        std::string name = decode_name(response, offset);

        // Type (2 bytes): A = 1, CNAME = 5, etc.
        uint16_t type = (response[offset] << 8) | response[offset + 1];
        uint16_t class_ = (response[offset + 2] << 8) | response[offset + 3];

        uint32_t ttl = (response[offset + 4] << 24) | (response[offset + 5] << 16) | (response[offset + 6] << 8) | response[offset + 7];
        uint16_t rdlength = (response[offset + 8] << 8) | response[offset + 9];

        offset += 10;

        if (type == 1 && class_ == 1 && rdlength == 4) {
            isValid = true;
            uint8_t ip1 = response[offset];
            uint8_t ip2 = response[offset + 1];
            uint8_t ip3 = response[offset + 2];
            uint8_t ip4 = response[offset + 3];
            std::cout << name << " has IPv4 address: " << (int)ip1 << "." << (int)ip2 << "." << (int)ip3 << "." << (int)ip4 << " (TTL: " << ttl << ")\n";
        }

        offset += rdlength;
    }
    return isValid;
}
