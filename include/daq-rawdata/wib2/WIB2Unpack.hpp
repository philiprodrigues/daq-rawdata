/**
 *
 *  @file WIB2Unpack.hpp
 *
 *  Functions for manipulating WIB2 frames in the format described by
 *  EDMS document 2088713:
 *  (https://edms.cern.ch/document/2088713/4). The code here is taken
 *  from unpack.h in the dune-wib-firmware repository, written by Ben
 *  Land, and lightly modified to be closer to DUNE DAQ software
 *  conventions. It is used in this repository as a check of the
 *  overlay code in WIB2Frame.hpp
 *
 */
#ifndef DAQRAWDATA_WIB2UNPACK_HPP
#define DAQRAWDATA_WIB2UNPACK_HPP

#include <cstddef>
#include <stdint.h>
#include <vector>

namespace dunedaq::rawdata::wib2 {
  
// Words in the binary format of the FELIX frame14 from the WIB
typedef struct {
    uint32_t start_frame;
    uint32_t wib_pre[4];
    uint32_t femb_a_seg[56];
    uint32_t femb_b_seg[56];
    uint32_t wib_post[2];
    uint32_t idle_frame;
} __attribute__ ((packed)) frame14;

// Samples from the U, V, X channels in a femb_*_seg of a frame as 16bit arrays
typedef struct {    
    uint16_t u[40],v[40],x[48];
} __attribute__ ((packed)) femb_data;

// Byte-aligned unpacked values in a felix_frame
typedef struct {
    uint8_t link_mask, femb_valid, fiber_num, wib_num, frame_version, crate_num;
    uint32_t wib_data;
    uint64_t timestamp;
    femb_data femb[2];
    uint32_t crc20;
    uint16_t flex12;
    uint32_t flex24;
} frame14_unpacked;

// Deframed data, where channels or u,v,x are time ordered uint16 samples for each channel
typedef struct {
    size_t samples;
    std::vector<uint16_t> channels[2][128];
    std::vector<uint64_t> timestamp;
    uint8_t crate_num, wib_num; 
} channel_data;
  
typedef struct {
    size_t samples;
    std::vector<uint16_t> u[2][40];
    std::vector<uint16_t> v[2][40];
    std::vector<uint16_t> x[2][48];
    std::vector<uint64_t> timestamp;
    uint8_t crate_num, wib_num; 
} uvx_data;


// Converts 14 bit packed channel data (56 uint32 words from the WIB) to byte-aligned 16 bit arrays (128 uint16 values)
void unpack14(const uint32_t *packed, uint16_t *unpacked);

// Converts byte-aligned 16 bit arrays (128 uint16 values) to 14 bit packed channel data (56 uint32 words from the WIB)
void repack14(const uint16_t *unpacked, uint32_t *packed);

// Unpacks the data in a frame14 struct to a frame14_unpacked struct
void unpack_frame(const frame14 *frame, frame14_unpacked *data);

// Repacks the data in a frame14_unpacked struct to a frame14 struct
void repack_frame(const frame14_unpacked *data, frame14 *frame);

// Converts a buffer of felix_frame in memory to time ordered data from each channel
void deframe_data(const frame14 *frame_buf, size_t nframes, channel_data &data);

// Converts a buffer of felix_frame in memory to time ordered data from each U,V,X mapping
void deframe_data(const frame14 *frame_buf, size_t nframes, uvx_data &data);

// Convert time ordered data from each channel back into frame14 data
void reframe_data(frame14 *frame_buf, size_t nframes, const channel_data &data);

// Fill a buffer of frame14 with fake data sin on even (cos on odd) period 10+2*ch samples
void fake_data(frame14 *buffer, size_t nframes);

}

#endif
