#include "infra/time.h"

#include <iostream>

int main() { std::cout << infra::time::strfnow(HISTORY_DAY_FORMAT) << std::endl; }