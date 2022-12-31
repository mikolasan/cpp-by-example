#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <vector>
#include <boost/math/distributions/chi_squared.hpp>

constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

bool is_in_interval(double left, double right, double value)
{
    return left < value && value < right;
}

typedef double (* Ifctn)( double t);
/* Numerical integration method */
double Simpson3_8( Ifctn f, double a, double b, int N)
{
    int j;
    double l1;
    double h = (b-a)/N;
    double h1 = h/3.0;
    double sum = f(a) + f(b);

    for (j=3*N-1; j>0; j--) {
        l1 = (j%3)? 3.0 : 2.0;
        sum += l1*f(a+h1*j) ;
    }
    return h*sum/8.0;
}

#define A 12
double Gamma_Spouge( double z )
{
    int k;
    static double cspace[A];
    static double *coefs = NULL;
    double accum;
    double a = A;

    if (!coefs) {
        double k1_factrl = 1.0;
        coefs = cspace;
        coefs[0] = sqrt(2.0*M_PI);
        for(k=1; k<A; k++) {
            coefs[k] = exp(a-k) * pow(a-k,k-0.5) / k1_factrl;
            k1_factrl *= -k;
        }
    }

    accum = coefs[0];
    for (k=1; k<A; k++) {
        accum += coefs[k]/(z+k);
    }
    accum *= exp(-(z+a)) * pow(z+a, z+0.5);
    return accum/z;
}

double aa1;
double f0( double t)
{
    return  pow(t, aa1)*exp(-t); 
}

double GammaIncomplete_Q( double a, double x)
{
    double y, h = 1.5e-2;  /* approximate integration step size */

    /* this cuts off the tail of the integration to speed things up */
    y = aa1 = a-1;
    while((f0(y) * (x-y) > 2.0e-8) && (y < x))   y += .4;
    if (y>x) y=x;

    return 1.0 - Simpson3_8( &f0, 0, y, (int)(y/h))/Gamma_Spouge(a);
}

double chi2Probability( int dof, double distance)
{
    return GammaIncomplete_Q( 0.5*dof, 0.5*distance);
}

double chi2UniformDistance( double *ds, int dslen)
{
    double expected = 0.0;
    double sum = 0.0;
    int k;

    for (k=0; k<dslen; k++) 
        expected += ds[k];
    expected /= k;

    for (k=0; k<dslen; k++) {
        double x = ds[k] - expected;
        sum += x*x;
    }
    return sum/expected;
}

double calculate_observed_difference_for_chi_squared_test(std::map<uint32_t, uint64_t>& observed, double expected)
{
    // unsigned k_bins = observed.size();
    double observed_difference = 0.0;
    double subtraction = 0.0;
    for (auto i = 0; i < observed.size(); ++i)
    {
        // try
        // {
            subtraction = observed.at(i) - expected;
        // }
        // catch(const std::out_of_range& e)
        // {
            // std::cerr << e.what() << '\n';
// 
            // for (const auto& [number, hits] : observed) {
                // std::cout << number << ": " << hits << std::endl;
            // }
        // }
        
        observed_difference += subtraction * subtraction;
    }
    return observed_difference / expected;
}

void chi_squared_test(std::map<uint32_t, uint64_t>& observed, double expected)
{
    // non suspicious interval:
    // 'left' bound:  v = 9, p = 5%,  x^2 = 3.325
    // 'right' bound: v = 9, p = 95%, x^2 = 16.92

    // v = 29
    // static const double left_x2 = 17.708;
    // static const double right_x2 = 42.557;
    double x2 = calculate_observed_difference_for_chi_squared_test(observed, expected);
    // std::cout << "chi_squared " << i 
    //     << " " << std::boolalpha << is_in_interval(left_x2, right_x2, x2)
    //     << std::endl;
    // int dof = observed.size() - 1;
    // double chi2 = chi2Probability(dof, x2);
    // std::cout << "x^2 " << x2 << " chi^2 " << chi2 << std::endl;
    using boost::math::chi_squared;
    using boost::math::quantile;
    using boost::math::complement;
    using boost::math::cdf;
    
    auto N = observed.size();
    chi_squared dist(N - 1);
    // double alpha = 0.05;
    // double ucv = quantile(complement(dist, alpha));
    // double ucv2 = quantile(complement(dist, alpha / 2));
    // double lcv = quantile(dist, alpha);
    // double lcv2 = quantile(dist, alpha / 2);

    double p = cdf(dist, x2);
    // std::cout << "ucv " << ucv << std::endl;
    // std::cout << "ucv2 " << ucv2 << std::endl;
    // std::cout << "lcv " << lcv << std::endl;
    // std::cout << "lcv2 " << lcv2 << std::endl;
    // std::cout << "x2 " << x2 << std::endl;
    std::cout << "p " << p << std::endl;
    // std::cout << x2 << std::endl;
}

int main(int argc, char const *argv[])
{
    // std::cout << chi2Probability(dof, x2); << std::endl;
    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()}; 
    std::mt19937 rng(seed);

    std::map<uint32_t, uint64_t> m;
    // std::vector<double> observed;

    uint64_t n_processed = 0;
    uint64_t total_sum = 0;
    uint64_t range_upper_limit = std::atoll(argv[1]);
    uint64_t sample_size = std::atoll(argv[2]);
    // observed.reserve(sample_size);
    int64_t input_number = 0;

    do {
        std::cin >> input_number;
        if (std::cin.eof() || std::cin.bad()) {
            break;
        } else if (std::cin.fail()) {
            std::cin.clear(); // unset failbit
            std::cin.ignore(max_size, '\n'); // skip bad input
        } else {
            // if (input_number >= range_upper_limit) {
            //     std::cout << "input number is out of range" << std::endl;
            //     break;
            // }

            // observed.push_back(input_number);
            if (input_number < 0)
                input_number += 0x100000000LL;

            
            
            // Classic Modulo (Biased)
            // uint32_t random_number = static_cast<uint32_t>(input_number % range_upper_limit);
            
            // Division with Rejection (Unbiased)
            // uint32_t random_number = 0;
            // uint32_t range = static_cast<uint32_t>(range_upper_limit);
            // // calculates divisor = 2**32 / range
            // uint32_t divisor = ((-range) / range) + 1;
            // if (divisor != 0) {
            //     random_number = static_cast<uint32_t>(input_number) / divisor;
            //     if (random_number >= range) {
            //         continue; // bad random number, use another one
            //     }
            // }

            // Debiased Modulo (Twice) — OpenBSD's Method
            uint32_t random_number = static_cast<uint32_t>(input_number);
            uint32_t range = static_cast<uint32_t>(range_upper_limit);
            uint32_t t = (-range) % range;
            if (random_number >= t)
                random_number = random_number % range;
            else
                continue; // bad random number, use another one


            ++n_processed;
            m[random_number]++;

            // total_sum += input_number;
        }
        
        
        // // std::uniform_int_distribution<uint32_t> dist(0, range_upper_limit - 1);
        // // auto random_number = dist(rng);

        // // uint32_t random_number = rand() % range_upper_limit;

        // m[random_number]++;
        // ++n_processed;

        if (n_processed == sample_size) {
            double expected = sample_size / range_upper_limit;
            // std::cout << "expected " << expected << std::endl;
            // for (const auto& [number, hits] : m) {
            //     std::cout << number << ": " << hits << std::endl;
            // }
            chi_squared_test(m, expected);
            n_processed = 0;
            m.clear();
            // return 5;
            // std::cout << "----------------------------------------------" << std::endl;
        }

    } while (true);

    
    for (const auto& [number, hits] : m) {
        std::cout << number << ": " << hits / static_cast<double>(n_processed) << " [" << hits << "]" << std::endl;
    }
    
    // double expected = sample_size / range_upper_limit;
    // chi_squared_test(m, expected);

    // std::cout << "AVG: " << total_sum / static_cast<double>(range_upper_limit) << std::endl;

    return 0;
}