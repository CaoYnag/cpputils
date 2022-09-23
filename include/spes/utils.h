#pragma once
#include <string>

long cur_ms();

std::string str_lowercase(const std::string& s);
std::string str_uppercase(const std::string& s);

int my_atoi(const std::string& s);

/*
 * https://sean.cm/a/c-tricks
 * simple random number generator.
 */
uint32_t smush();
void     smush_seed(uint32_t s);
