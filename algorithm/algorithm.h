#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace btra {

inline double simple_return(double current_price, double previous_price) {
    if (previous_price == 0.0) {
        return 0.0; // Avoid division by zero
    }
    return (current_price - previous_price) / previous_price;
}

inline double log_return(double current_price, double previous_price) {
    if (previous_price <= 0.0) {
        return 0.0; // Avoid log of non-positive numbers
    }
    return std::log(current_price / previous_price);
}

inline double cumulative_return(double current_price, double initial_price) {
    if (initial_price == 0.0) {
        return 0.0; // Avoid division by zero
    }
    return (current_price - initial_price) / initial_price;
}

inline double annualized_return(double cumulative_return, int years) {
    if (years <= 0) {
        return 0.0; // Avoid division by zero or negative years
    }
    return std::pow(1 + cumulative_return, 1.0 / years) - 1;
}

inline double potifolio_return(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double total_return = 1.0;
    for (int i = 0; i < n; ++i) {
        total_return *= (1 + returns[i]);
    }

    return total_return - 1; // Return as a percentage
}

inline double volatility(double* returns, int n) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate volatility
    }

    double mean = 0.0;
    for (int i = 0; i < n; ++i) {
        mean += returns[i];
    }
    mean /= n;

    double variance = 0.0;
    for (int i = 0; i < n; ++i) {
        variance += (returns[i] - mean) * (returns[i] - mean);
    }
    variance /= (n - 1); // Sample variance

    return std::sqrt(variance);
}

inline double sharpe_ratio(double* returns, int n, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate Sharpe ratio
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double stddev = volatility(returns, n);

    if (stddev == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / stddev;
}

inline double sortino_ratio(double* returns, int n, double target_return) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate Sortino ratio
    }

    double downside_deviation = 0.0;
    int downside_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < target_return) {
            downside_deviation += (returns[i] - target_return) * (returns[i] - target_return);
            downside_count++;
        }
    }

    if (downside_count == 0) {
        return 0.0; // No downside risk
    }

    downside_deviation = std::sqrt(downside_deviation / downside_count);

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    if (downside_deviation == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - target_return) / downside_deviation;
}

inline double calmar_ratio(double cumulative_return, double max_drawdown) {
    if (max_drawdown == 0.0) {
        return 0.0; // Avoid division by zero
    }
    return cumulative_return / max_drawdown;
}

inline double max_drawdown(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double peak = returns[0];
    double trough = returns[0];
    double max_dd = 0.0;

    for (int i = 1; i < n; ++i) {
        if (returns[i] > peak) {
            peak = returns[i];
            trough = returns[i]; // Reset trough when a new peak is found
        } else if (returns[i] < trough) {
            trough = returns[i];
            double dd = (peak - trough) / peak;
            if (dd > max_dd) {
                max_dd = dd;
            }
        }
    }

    return max_dd; // Return as a percentage
}

inline double omega_ratio(double* returns, int n, double target_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            gain += returns[i] - target_return;
        } else {
            loss += target_return - returns[i];
        }
    }

    if (loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return gain / loss;
}

inline double information_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate Information ratio
    }

    double active_return = 0.0;
    for (int i = 0; i < n; ++i) {
        active_return += returns[i] - benchmark_return;
    }
    active_return /= n;

    double tracking_error = 0.0;
    for (int i = 0; i < n; ++i) {
        tracking_error +=
            (returns[i] - benchmark_return - active_return) * (returns[i] - benchmark_return - active_return);
    }
    tracking_error = std::sqrt(tracking_error / (n - 1)); // Sample standard deviation

    if (tracking_error == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return active_return / tracking_error;
}

inline double capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double up_capture = 0.0;
    double down_capture = 0.0;
    int up_count = 0;
    int down_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > benchmark_return) {
            up_capture += returns[i] / benchmark_return;
            up_count++;
        } else if (returns[i] < benchmark_return) {
            down_capture += returns[i] / benchmark_return;
            down_count++;
        }
    }

    if (up_count == 0) {
        up_capture = 1.0; // No upward movement
    } else {
        up_capture /= up_count;
    }

    if (down_count == 0) {
        down_capture = 1.0; // No downward movement
    } else {
        down_capture /= down_count;
    }

    return up_capture / down_capture; // Capture ratio
}

inline double beta(double* returns, int n, double benchmark_return) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate beta
    }

    double mean_return = 0.0;
    double mean_benchmark = 0.0;

    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
        mean_benchmark += benchmark_return; // Assuming benchmark_return is constant
    }
    mean_return /= n;
    mean_benchmark /= n;

    double covariance = 0.0;
    double variance = 0.0;

    for (int i = 0; i < n; ++i) {
        covariance += (returns[i] - mean_return) * (benchmark_return - mean_benchmark);
        variance += (benchmark_return - mean_benchmark) * (benchmark_return - mean_benchmark);
    }

    if (variance == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return covariance / variance;
}

inline double alpha(double* returns, int n, double benchmark_return, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate alpha
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double beta_value = beta(returns, n, benchmark_return);

    return mean_return - (risk_free_rate + beta_value * (benchmark_return - risk_free_rate));
}

inline double treynor_ratio(double* returns, int n, double benchmark_return, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate Treynor ratio
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double beta_value = beta(returns, n, benchmark_return);

    if (beta_value == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / beta_value;
}

inline double jensens_alpha(double* returns, int n, double benchmark_return, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate Jensen's alpha
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double beta_value = beta(returns, n, benchmark_return);

    return mean_return - (risk_free_rate + beta_value * (benchmark_return - risk_free_rate));
}

inline double downside_risk(double* returns, int n, double target_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double downside_deviation = 0.0;
    int downside_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < target_return) {
            downside_deviation += (returns[i] - target_return) * (returns[i] - target_return);
            downside_count++;
        }
    }

    if (downside_count == 0) {
        return 0.0; // No downside risk
    }

    return std::sqrt(downside_deviation / downside_count); // Return as standard deviation
}

inline double up_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double up_capture = 0.0;
    int up_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > benchmark_return) {
            up_capture += returns[i] / benchmark_return;
            up_count++;
        }
    }

    if (up_count == 0) {
        return 1.0; // No upward movement
    }

    return up_capture / up_count; // Return as average capture ratio
}

inline double down_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double down_capture = 0.0;
    int down_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < benchmark_return) {
            down_capture += returns[i] / benchmark_return;
            down_count++;
        }
    }

    if (down_count == 0) {
        return 1.0; // No downward movement
    }

    return down_capture / down_count; // Return as average capture ratio
}

inline double gain_loss_ratio(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > 0) {
            gain += returns[i];
        } else {
            loss += -returns[i]; // Convert loss to positive value
        }
    }

    if (loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return gain / loss; // Gain to loss ratio
}

inline double profit_factor(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gross_profit = 0.0;
    double gross_loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > 0) {
            gross_profit += returns[i];
        } else {
            gross_loss += -returns[i]; // Convert loss to positive value
        }
    }

    if (gross_loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return gross_profit / gross_loss; // Profit factor
}

inline double risk_adjusted_return(double* returns, int n, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-adjusted return
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double stddev = volatility(returns, n);

    if (stddev == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / stddev; // Risk-adjusted return
}

inline double value_at_risk(double* returns, int n, double confidence_level) {
    if (n <= 0 || confidence_level <= 0.0 || confidence_level >= 1.0) {
        return 0.0; // Invalid input
    }

    std::vector<double> sorted_returns(returns, returns + n);
    std::sort(sorted_returns.begin(), sorted_returns.end());

    int index = static_cast<int>((1 - confidence_level) * n);
    if (index < 0) {
        index = 0; // Ensure index is within bounds
    }

    return sorted_returns[index]; // Return the VaR at the specified confidence level
}

inline double expected_shortfall(double* returns, int n, double confidence_level) {
    if (n <= 0 || confidence_level <= 0.0 || confidence_level >= 1.0) {
        return 0.0; // Invalid input
    }

    std::vector<double> sorted_returns(returns, returns + n);
    std::sort(sorted_returns.begin(), sorted_returns.end());

    int index = static_cast<int>((1 - confidence_level) * n);
    if (index < 0) {
        index = 0; // Ensure index is within bounds
    }

    double sum = 0.0;
    for (int i = 0; i < index; ++i) {
        sum += sorted_returns[i];
    }

    return sum / index; // Return the average of the worst returns
}

inline double skewness(double* returns, int n) {
    if (n <= 2) {
        return 0.0; // Not enough data to calculate skewness
    }

    double mean = 0.0;
    for (int i = 0; i < n; ++i) {
        mean += returns[i];
    }
    mean /= n;

    double m3 = 0.0;
    double m2 = 0.0;

    for (int i = 0; i < n; ++i) {
        double diff = returns[i] - mean;
        m3 += diff * diff * diff;
        m2 += diff * diff;
    }

    m3 /= n;
    m2 /= n;

    if (m2 == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return m3 / std::pow(m2, 1.5); // Return skewness
}

inline double kurtosis(double* returns, int n) {
    if (n <= 3) {
        return 0.0; // Not enough data to calculate kurtosis
    }

    double mean = 0.0;
    for (int i = 0; i < n; ++i) {
        mean += returns[i];
    }
    mean /= n;

    double m4 = 0.0;
    double m2 = 0.0;

    for (int i = 0; i < n; ++i) {
        double diff = returns[i] - mean;
        m4 += diff * diff * diff * diff;
        m2 += diff * diff;
    }

    m4 /= n;
    m2 /= n;

    if (m2 == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return m4 / (m2 * m2) - 3; // Return excess kurtosis
}

inline double tail_ratio(double* returns, int n, double threshold) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double tail_gain = 0.0;
    double tail_loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > threshold) {
            tail_gain += returns[i];
        } else if (returns[i] < threshold) {
            tail_loss += -returns[i]; // Convert loss to positive value
        }
    }

    if (tail_loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return tail_gain / tail_loss; // Tail ratio
}

inline double omega_ratio(double* returns, int n, double target_return, double threshold) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            gain += returns[i] - target_return;
        } else if (returns[i] < threshold) {
            loss += target_return - returns[i];
        }
    }

    if (loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return gain / loss; // Omega ratio
}

inline double gain_to_pain_ratio(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double pain = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > 0) {
            gain += returns[i];
        } else {
            pain += -returns[i]; // Convert loss to positive value
        }
    }

    if (pain == 0.0) {
        return std::numeric_limits<double>::infinity(); // No pain, infinite ratio
    }

    return gain / pain; // Gain to pain ratio
}

inline double upside_potential_ratio(double* returns, int n, double target_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double upside = 0.0;
    double downside = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            upside += returns[i] - target_return;
        } else if (returns[i] < target_return) {
            downside += target_return - returns[i];
        }
    }

    if (downside == 0.0) {
        return std::numeric_limits<double>::infinity(); // No downside, infinite ratio
    }

    return upside / downside; // Upside potential ratio
}

inline double downside_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double downside_capture = 0.0;
    int down_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < benchmark_return) {
            downside_capture += returns[i] / benchmark_return;
            down_count++;
        }
    }

    if (down_count == 0) {
        return 1.0; // No downward movement
    }

    return downside_capture / down_count; // Return as average capture ratio
}

inline double upside_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double upside_capture = 0.0;
    int up_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > benchmark_return) {
            upside_capture += returns[i] / benchmark_return;
            up_count++;
        }
    }

    if (up_count == 0) {
        return 1.0; // No upward movement
    }

    return upside_capture / up_count; // Return as average capture ratio
}

inline double risk_return_ratio(double* returns, int n, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-return ratio
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double stddev = volatility(returns, n);

    if (stddev == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / stddev; // Risk-return ratio
}

inline double risk_adjusted_sharpe_ratio(double* returns, int n, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-adjusted Sharpe ratio
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double stddev = volatility(returns, n);

    if (stddev == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / stddev; // Risk-adjusted Sharpe ratio
}

inline double risk_adjusted_sortino_ratio(double* returns, int n, double target_return) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-adjusted Sortino ratio
    }

    double downside_deviation = 0.0;
    int downside_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < target_return) {
            downside_deviation += (returns[i] - target_return) * (returns[i] - target_return);
            downside_count++;
        }
    }

    if (downside_count == 0) {
        return 0.0; // No downside risk
    }

    downside_deviation = std::sqrt(downside_deviation / downside_count);

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    if (downside_deviation == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - target_return) / downside_deviation; // Risk-adjusted Sortino ratio
}

inline double risk_adjusted_calmar_ratio(double cumulative_return, double max_drawdown) {
    if (max_drawdown == 0.0) {
        return 0.0; // Avoid division by zero
    }
    return cumulative_return / max_drawdown; // Risk-adjusted Calmar ratio
}

inline double risk_adjusted_omega_ratio(double* returns, int n, double target_return, double threshold) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            gain += returns[i] - target_return;
        } else if (returns[i] < threshold) {
            loss += target_return - returns[i];
        }
    }

    if (loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return gain / loss; // Risk-adjusted Omega ratio
}

inline double risk_adjusted_gain_loss_ratio(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double pain = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > 0) {
            gain += returns[i];
        } else {
            pain += -returns[i]; // Convert loss to positive value
        }
    }

    if (pain == 0.0) {
        return std::numeric_limits<double>::infinity(); // No pain, infinite ratio
    }

    return gain / pain; // Risk-adjusted gain to pain ratio
}

inline double risk_adjusted_upside_potential_ratio(double* returns, int n, double target_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double upside = 0.0;
    double downside = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            upside += returns[i] - target_return;
        } else if (returns[i] < target_return) {
            downside += target_return - returns[i];
        }
    }

    if (downside == 0.0) {
        return std::numeric_limits<double>::infinity(); // No downside, infinite ratio
    }

    return upside / downside; // Risk-adjusted upside potential ratio
}

inline double risk_adjusted_downside_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double downside_capture = 0.0;
    int down_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < benchmark_return) {
            downside_capture += returns[i] / benchmark_return;
            down_count++;
        }
    }

    if (down_count == 0) {
        return 1.0; // No downward movement
    }

    return downside_capture / down_count; // Return as average capture ratio
}

inline double risk_adjusted_upside_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double upside_capture = 0.0;
    int up_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > benchmark_return) {
            upside_capture += returns[i] / benchmark_return;
            up_count++;
        }
    }

    if (up_count == 0) {
        return 1.0; // No upward movement
    }

    return upside_capture / up_count; // Return as average capture ratio
}

inline double risk_adjusted_risk_return_ratio(double* returns, int n, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-return ratio
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double stddev = volatility(returns, n);

    if (stddev == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / stddev; // Risk-adjusted risk-return ratio
}

inline double risk_adjusted_risk_adjusted_sharpe_ratio(double* returns, int n, double risk_free_rate) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-adjusted Sharpe ratio
    }

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    double stddev = volatility(returns, n);

    if (stddev == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - risk_free_rate) / stddev; // Risk-adjusted Sharpe ratio
}

inline double risk_adjusted_risk_adjusted_sortino_ratio(double* returns, int n, double target_return) {
    if (n <= 1) {
        return 0.0; // Not enough data to calculate risk-adjusted Sortino ratio
    }

    double downside_deviation = 0.0;
    int downside_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < target_return) {
            downside_deviation += (returns[i] - target_return) * (returns[i] - target_return);
            downside_count++;
        }
    }

    if (downside_count == 0) {
        return 0.0; // No downside risk
    }

    downside_deviation = std::sqrt(downside_deviation / downside_count);

    double mean_return = 0.0;
    for (int i = 0; i < n; ++i) {
        mean_return += returns[i];
    }
    mean_return /= n;

    if (downside_deviation == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (mean_return - target_return) / downside_deviation; // Risk-adjusted Sortino ratio
}

inline double risk_adjusted_risk_adjusted_calmar_ratio(double cumulative_return, double max_drawdown) {
    if (max_drawdown == 0.0) {
        return 0.0; // Avoid division by zero
    }
    return cumulative_return / max_drawdown; // Risk-adjusted Calmar ratio
}

inline double risk_adjusted_risk_adjusted_omega_ratio(double* returns, int n, double target_return, double threshold) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double loss = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            gain += returns[i] - target_return;
        } else if (returns[i] < threshold) {
            loss += target_return - returns[i];
        }
    }

    if (loss == 0.0) {
        return std::numeric_limits<double>::infinity(); // No losses, infinite ratio
    }

    return gain / loss; // Risk-adjusted Omega ratio
}

inline double risk_adjusted_risk_adjusted_gain_loss_ratio(double* returns, int n) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double gain = 0.0;
    double pain = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > 0) {
            gain += returns[i];
        } else {
            pain += -returns[i]; // Convert loss to positive value
        }
    }

    if (pain == 0.0) {
        return std::numeric_limits<double>::infinity(); // No pain, infinite ratio
    }

    return gain / pain; // Risk-adjusted gain to pain ratio
}

inline double risk_adjusted_risk_adjusted_upside_potential_ratio(double* returns, int n, double target_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double upside = 0.0;
    double downside = 0.0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > target_return) {
            upside += returns[i] - target_return;
        } else if (returns[i] < target_return) {
            downside += target_return - returns[i];
        }
    }

    if (downside == 0.0) {
        return std::numeric_limits<double>::infinity(); // No downside, infinite ratio
    }

    return upside / downside; // Risk-adjusted upside potential ratio
}

inline double risk_adjusted_risk_adjusted_downside_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double downside_capture = 0.0;
    int down_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] < benchmark_return) {
            downside_capture += returns[i] / benchmark_return;
            down_count++;
        }
    }

    if (down_count == 0) {
        return 1.0; // No downward movement
    }

    return downside_capture / down_count; // Return as average capture ratio
}

inline double risk_adjusted_risk_adjusted_upside_capture_ratio(double* returns, int n, double benchmark_return) {
    if (n <= 0) {
        return 0.0; // No returns to calculate
    }

    double upside_capture = 0.0;
    int up_count = 0;

    for (int i = 0; i < n; ++i) {
        if (returns[i] > benchmark_return) {
            upside_capture += returns[i] / benchmark_return;
            up_count++;
        }
    }

    if (up_count == 0) {
        return 1.0; // No upward movement
    }

    return upside_capture / up_count; // Return as average capture ratio
}

} // namespace btra