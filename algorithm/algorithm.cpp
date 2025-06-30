#include "algorithm/algorithm.h"

namespace btra {

double simple_return(double current_price, double previous_price) {
    if (previous_price == 0.0) {
        return 0.0; // Avoid division by zero
    }
    return (current_price - previous_price) / previous_price;
}

double log_return(double current_price, double previous_price) {
    if (previous_price <= 0.0) {
        return 0.0; // Avoid log of non-positive numbers
    }
    return std::log(current_price / previous_price);
}

} // namespace btra
