#include <iostream>
#include <iomanip>
#include "dns.h"

int main() {
    std::string domain = "dns.google.com";
    auto query = build_dns_query(domain);

    std::cout << "Sending DNS query to 8.8.8.8...\n";
    std::vector<uint8_t> response;

    if (send_udp_query(query, response, 0x1234)) {
        std::cout << "Received response (" << response.size() << " bytes):\n";
        for (size_t i = 0; i < response.size(); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)response[i];
            if ((i + 1) % 16 == 0) std::cout << "\n";
        }
        std::cout << std::dec << "\n";
    } 
    else {
        std::cerr << "Failed to get a valid response.\n";
    }

    return 0;
}
