#pragma once
#include <vector>
#include <string>
#include <cstdint>

std::string decode_name(const std::vector<uint8_t>& buffer, size_t& offset);
bool parse_answers(const std::vector<uint8_t>& response, size_t offset, uint16_t ancount);
