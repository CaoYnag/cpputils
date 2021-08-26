#pragma once
#include <string>
using namespace std;

long cur_ms();

string str_lowercase(const string& s);
string str_uppercase(const string& s);

int my_atoi(const string& s);


/* 
 * https://sean.cm/a/c-tricks
 * simple random number generator.
 */
uint32_t smush();
void smush_seed(uint32_t s);
