// fp_overflow_checker.cpp
// CS3339 Spring 2026 - Homework 2
// Author: Basil Idris Mohad
// Date: March 27, 2026
//
// Detects floating-point precision overflow: finds the smallest value at
// which incrementing by the loop counter has no effect (increment is lost
// due to insufficient mantissa bits).

#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

// Represents an IEEE 754 single-precision float using std::bitset<32>.
// Layout: bit 31 = sign, bits 30-23 = biased exponent, bits 22-0 = mantissa.
class FloatBits {
 public:
  explicit FloatBits(float value) {
    uint32_t raw;
    std::memcpy(&raw, &value, sizeof(float));
    bits_ = std::bitset<32>(raw);
  }

  explicit FloatBits(uint32_t raw) : bits_(raw) {}

  float ToFloat() const {
    uint32_t raw = static_cast<uint32_t>(bits_.to_ulong());
    float value;
    std::memcpy(&value, &raw, sizeof(float));
    return value;
  }

  // Returns the 8-bit biased exponent field.
  uint32_t GetBiasedExponent() const {
    return (static_cast<uint32_t>(bits_.to_ulong()) >> 23) & 0xFFU;
  }

  // Returns the 23-bit mantissa (fraction) field.
  uint32_t GetMantissa() const {
    return static_cast<uint32_t>(bits_.to_ulong()) & 0x7FFFFFU;
  }

  // Prints the IEEE 754 bit representation as: s eeeeeeee mmmmmmmmmmmmmmmmmmmmmmm
  void Print() const {
    // Sign bit (MSB)
    std::cout << bits_[31] << " ";
    // Exponent bits (30 down to 23)
    for (int i = 30; i >= 23; --i) std::cout << bits_[i];
    std::cout << " ";
    // Mantissa bits (22 down to 0)
    for (int i = 22; i >= 0; --i) std::cout << bits_[i];
  }

 private:
  std::bitset<32> bits_;
};

void PrintUsage(const char* prog_name) {
  std::cout << "usage:\n"
            << "       " << prog_name << " loop_bound loop_counter\n\n"
            << "       loop_bound is a positive floating-point value\n"
            << "       loop_counter is a positive floating-point value\n";
}

// Computes the overflow threshold using only bit manipulation.
//
// For a float v with biased exponent E, ULP(v) = 2^(E-150).
// The increment `inc` is lost (v + inc == v) when inc <= ULP(v)/2 = 2^(E-151).
//
// Solving for the minimum biased exponent E of the threshold:
//   2^(E-151) >= inc = 2^(E_inc-127) * (1 + M_inc/2^23)
//
//   If M_inc == 0 (inc is an exact power of 2): E_threshold = E_inc + 24
//   If M_inc != 0 (inc has fractional mantissa bits): E_threshold = E_inc + 25
//
// The threshold value is 2^(E_threshold - 127), i.e., mantissa = 0.
FloatBits ComputeThreshold(const FloatBits& increment) {
  uint32_t e_inc = increment.GetBiasedExponent();
  uint32_t m_inc = increment.GetMantissa();

  uint32_t e_threshold = (m_inc == 0) ? (e_inc + 24) : (e_inc + 25);

  // Build threshold raw bits: sign=0, exponent=e_threshold, mantissa=0
  uint32_t threshold_raw = e_threshold << 23;
  return FloatBits(threshold_raw);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    PrintUsage(argv[0]);
    return 1;
  }

  float loop_bound   = static_cast<float>(std::atof(argv[1]));
  float loop_counter = static_cast<float>(std::atof(argv[2]));

  FloatBits bound_bits(loop_bound);
  FloatBits counter_bits(loop_counter);

  // Print IEEE 754 bit representations
  std::cout << "Loop bound:   ";
  bound_bits.Print();
  std::cout << "\n";
  std::cout << "Loop counter: ";
  counter_bits.Print();
  std::cout << "\n\n";

  // Detect overflow threshold via bit manipulation (no brute-force, no log2/pow)
  FloatBits threshold_bits = ComputeThreshold(counter_bits);
  float threshold = threshold_bits.ToFloat();

  if (threshold > loop_bound) {
    std::cout << "There is no overflow!\n";
  } else {
    std::cout << "Warning: Possible overflow!\n";
    std::cout << "Overflow threshold:\n";
    std::cout << "      " << threshold << "\n";
    std::cout << "      ";
    threshold_bits.Print();
    std::cout << "\n";
  }

  return 0;
}
