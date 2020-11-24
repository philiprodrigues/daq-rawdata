// in_out_test.cxx
//
// Self-consistency check for WIB2Frame getters and setters: Create a
// WIB2Frame, set its values using set_adc() and check that the same
// values are read back using get_adc()

#include "daq-rawdata/wib2/WIB2Frame.hpp"

#include <array>
#include <cstdio>    // For printf
#include <cstring>   // For std::memset
#include <random>

bool in_out_test(const std::array<uint16_t, 256>& vals)
{
  dunedaq::rawdata::WIB2Frame frame;
  // Zero it out first
  std::memset(&frame, 0, sizeof(dunedaq::rawdata::WIB2Frame));

  for(size_t i=0; i<vals.size(); ++i){
    frame.set_adc(i, vals[i]);
  }

  bool success=true;
  for(size_t i=0; i<vals.size(); ++i){
    uint16_t out_val=frame.get_adc(i);
    uint16_t in_val=vals[i];
    if(out_val!=in_val){
      success=false;
      printf("Failure at index %ld. Input: %03x, output: %03x\n", i, in_val, out_val);
    }
  }
  return success;
}

int main()
{
  {
    printf("Test 1:\n");
    std::array<uint16_t, 256> vals;
    for(size_t i=0; i<vals.size(); ++i) vals[i]=i;
    bool success=in_out_test(vals);
    if(success){
      printf("Success\n");
    }
    else{
      printf("Failed\n");
    }
  }

  {
    printf("Test 2:\n");
    std::array<uint16_t, 256> vals;
    for(size_t i=0; i<vals.size(); ++i) vals[i]=0x3a0+i;
    bool success=in_out_test(vals);
    if(success){
      printf("Success\n");
    }
    else{
      printf("Failed\n");
    }
  }

  // Fuzz test with random values
  std::default_random_engine e1(10);
  std::uniform_int_distribution<int> uniform_dist(0, (1<<14)-1);

  const int n_fuzz=1000000;
  int n_fail=0;
  printf("Fuzz testing with %d iterations:\n", n_fuzz);

  for(int i=0; i<n_fuzz; ++i){
    std::array<uint16_t, 256> vals;
    for(size_t j=0; j<vals.size(); ++j){
      vals[j]=uniform_dist(e1);
    }
    bool success=in_out_test(vals);
    if(!success){
      ++n_fail;
      printf("Test %d of %d failed\n", i, n_fuzz);
      printf("Values were:\n");
      for(size_t j=0; j<vals.size(); ++j){
        printf("% 6d ", vals[j]);
      }
      printf("\n");
    }
  }
  printf("%d failures in %d tests\n", n_fail, n_fuzz);
  
}
