#pragma once
#include<vector>
#include<string>
#include<cstdint>

struct DNSHeader {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
};

struct DNSQuestion {
    std::string qname;
    uint16_t qtype;
    uint16_t qclass;
};

// Build DNS query packet
std::vector<uint8_t> build_dns_query(const std::string& domain);

bool send_udp_query(const std::vector<uint8_t> &packet, std::vector<uint8_t> &response, uint16_t expected_id);
