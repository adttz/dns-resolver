#include <iostream>
#include "dns.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <domain>" << '\n';
        return 1;
    }

    std::string domain = "dns.google.com";
    std::vector<uint8_t> query = build_dns_query(domain);
    std::vector<uint8_t> response;

    if (send_udp_query(query, response, 0x1234)) {
        uint16_t ancount = (response[6] << 8) | response[7];        // Number of answers
        size_t offset = 12;                                         // 12 bytes : fixed-size DNS header

        decode_name(response, offset); // skip question name
        offset += 4; // skip QTYPE + QCLASS

        if(!parse_answers(response, offset, ancount)){
            std::cerr << "Enter valid url." << '\n';
            return 1;
        }
    } 
    else {
        std::cerr << "DNS query failed.\n";
    }

    return 0;
}
