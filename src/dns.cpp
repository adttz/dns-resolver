#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <random>
#include "dns.h"

// Convert hostname to qname (length octet followed by that number of octets)
static void encode_name(const std::string &domain, std::vector<uint8_t> &out) {
    size_t start = 0;
    size_t end;

    while ((end = domain.find('.', start)) != std::string::npos) {
        uint8_t len = end - start;
        out.push_back(len);
        for(size_t i = start; i < end; i++){
            out.push_back(domain[i]);
        }
        start = end + 1;
    }

    uint8_t len = domain.size() - start;
    out.push_back(len);
    for(size_t i = start; i < domain.size(); i++){
        out.push_back(domain[i]);
    }
    out.push_back(0); // null terminator
}

std::vector<uint8_t> build_dns_query(const std::string& domain) {
    std::vector<uint8_t> packet;
    
    DNSHeader header;
    header.id = htons(0x1234);            // htons() - converts host byte order (little-endian) to network byte order (big-endian)
    header.flags = htons(0x0100);         // recursion desired
    header.qdcount = htons(1);            // 1 question
    header.ancount = htons(0);
    header.nscount = htons(0);
    header.arcount = htons(0);

    // Append header
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(header));
    //reinterpret_cast<T>(ptr) treats the bits at ptr as if they belong to type T, without changing them
    
    // Encode domain name
    encode_name(domain, packet);

    // Append question
    uint16_t qtype = htons(1);   // A record
    uint16_t qclass = htons(1);  // IN class

    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&qtype), reinterpret_cast<uint8_t*>(&qtype) + 2);
    packet.insert(packet.end(), reinterpret_cast<uint8_t*>(&qclass), reinterpret_cast<uint8_t*>(&qclass) + 2);

    return packet;
}

bool send_udp_query(const std::vector<uint8_t>& packet, std::vector<uint8_t>& response, uint16_t expected_id) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);    // s = socket(domain, type, protocol)
    if (sock < 0) {
        perror("socket");
        return false;
    }

    struct sockaddr_in server{};
    server.sin_family = AF_INET;    // IPv4
    server.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &server.sin_addr);

    if (sendto(sock, packet.data(), packet.size(), 0, (struct sockaddr*)&server, sizeof(server)) < 0) {     // ssize_t sendto(int sockfd, const void buf[.len], size_t len, int flags,
        perror("sendto");                                                                                   //                const struct sockaddr *dest_addr, socklen_t addrlen);
        close(sock);
        return false;
    }

    uint8_t buf[512];       // max DNS packet size over UDP
    socklen_t slen = sizeof(server);
    ssize_t recv_len = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&server, &slen);
    if (recv_len < 0) {
        perror("recvfrom");
        close(sock);
        return false;
    }

    response.assign(buf, buf + recv_len);
    close(sock);

    // Check the ID field
    uint16_t response_id = (response[0] << 8) | response[1];
    if (response_id != expected_id) {
        std::cerr << "Mismatched ID: got " << response_id << ", expected " << expected_id << "\n";
        return false;
    }

    return true;
}