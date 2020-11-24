cmake_minimum_required(VERSION 3.12)
project(daq-rawdata VERSION 0.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

##############################################################################
# Main library

daq_add_library(*.cpp)

##############################################################################
# Plugins

#daq_add_plugin(... ... LINK_LIBRARIES daq-rawdata)

##############################################################################
# Integration tests

daq_add_application(in_out_test in_out_test.cxx TEST LINK_LIBRARIES daq-rawdata)
daq_add_application(check_frame_vs_unpack check_frame_vs_unpack.cxx TEST LINK_LIBRARIES daq-rawdata)

##############################################################################
# No unit tests written

##############################################################################

daq_install()
