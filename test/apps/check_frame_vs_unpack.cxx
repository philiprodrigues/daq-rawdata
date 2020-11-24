// check_frame_vs_unpack.cxx
//
// Check the WIB2 frame overlay code in WIB2Frame.hpp by comparing to
// the output of the (independently-written) code in WIB2Unpack.hpp

#include "daq-rawdata/wib2/WIB2Frame.hpp"
#include "daq-rawdata/wib2/WIB2Unpack.hpp"

#include <array>
#include <cstdio>    // For printf
#include <cstring>   // For std::memset
#include <random>

// Given an array `vals` of 256 ADC values, construct a WIB2 frame
// whose ADCs have those values, using the functions from
// WIB2Unpack.hpp . Interpret that frame using the WIB2Frame overlay
// and check that the values given by WIB2Frame equal the
// inputs. Return true on success
bool check_wib2_frame_unpack(const std::array<uint16_t, 256>& vals)
{
  dunedaq::rawdata::wib2::frame14_unpacked unpacked;
  // Zero it out just to be on the safe side
  std::memset(&unpacked, 0, sizeof(dunedaq::rawdata::wib2::frame14_unpacked));

  dunedaq::rawdata::wib2::frame14 packed;
  // Zero it out just to be on the safe side
  std::memset(&packed, 0, sizeof(dunedaq::rawdata::wib2::frame14));

  // Fill the unpacked frame from the input `vals` array
  int counter=0;
  for(int femb=0; femb<2; ++femb){
    for(int i=0; i<40; ++i){
      unpacked.femb[femb].u[i]=vals[counter++];
    }
    for(int i=0; i<40; ++i){
      unpacked.femb[femb].v[i]=vals[counter++];
    }
    for(int i=0; i<48; ++i){
      unpacked.femb[femb].x[i]=vals[counter++];
    }
  }

  // Create the packed array from the unpacked array
  dunedaq::rawdata::wib2::repack_frame(&unpacked, &packed);

  dunedaq::rawdata::WIB2Frame& wib2frame=*reinterpret_cast<dunedaq::rawdata::WIB2Frame*>(&packed);

  bool success=true;
  for(int femb=0; femb<2; ++femb){
    for(int i=0; i<40; ++i){
      uint16_t gold=unpacked.femb[femb].u[i];
      uint16_t test=wib2frame.get_u(femb, i);
      if(test!=gold){
        success=false;
        printf("Failure: u[%d] was %d. Should be %d\n", i, test, gold);
      }
    }
    for(int i=0; i<40; ++i){
      uint16_t gold=unpacked.femb[femb].v[i];
      uint16_t test=wib2frame.get_v(femb, i);
      if(test!=gold){
        success=false;
        printf("Failure: v[%d] was %d. Should be %d\n", i, test, gold);
      }
    }
    for(int i=0; i<48; ++i){
      uint16_t gold=unpacked.femb[femb].x[i];
      uint16_t test=wib2frame.get_x(femb, i);
      if(test!=gold){
        success=false;
        printf("Failure: x[%d] was %d. Should be %d\n", i, test, gold);
      }
    }
  } // loop over femb

  return success;
}

int main()
{
  // Fuzz test with random values
  std::default_random_engine e1(10);
  std::uniform_int_distribution<int> uniform_dist(0, (1<<14)-1);

  const int n_fuzz=100000;
  int n_fail=0;
  printf("Fuzz testing with %d iterations:\n", n_fuzz);

  for(int i=0; i<n_fuzz; ++i){
    std::array<uint16_t, 256> vals;
    for(size_t j=0; j<vals.size(); ++j){
      vals[j]=uniform_dist(e1);
    }
    bool success=check_wib2_frame_unpack(vals);
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
