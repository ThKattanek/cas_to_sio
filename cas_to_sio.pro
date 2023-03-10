#//////////////////////////////////////////////////
#//                                              //
#// CasToSio                                     //
#// by  Thorsten Kattanek                        //
#//						 //
#// Atari Cas files send over sio bus with uart  //
#//                                              //
#// #file: cas_to_sio.pro                        //
#//                                              //
#// This source code is Copyright protected!     //
#//                                              //
#// Last changed at 2022-12-29                   //
#// https://github.com/ThKattanek/cas_to_sio     //
#//                                              //
#//////////////////////////////////////////////////

TEMPLATE = subdirs

  SUBDIRS = cas_to_sio

  cas_to_sio.subdir = src

  CONFIG += ordered

# Example for more subdirs
# SUBDIRS = src1\
#            src2
#
# src1.subdir = src1
# src2.subdir = src2
#
# app.depends = src1 src2


