/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_ENV_HPP__
#define __ORCUS_ENV_HPP__

// orcus

#if defined _WIN32 || defined __CYGWIN__
  #if defined __MINGW32__
    #define ORCUS_DLLPUBLIC
    #define ORCUS_DLLLOCAL
  #elif defined __ORCUS_BUILDING_DLL
    #ifdef __GNUC__
      #define ORCUS_DLLPUBLIC __attribute__ ((dllexport))
    #else
      #define ORCUS_DLLPUBLIC __declspec(dllexport)
    #endif
  #elif defined __ORCUS_STATIC_LIB
      #define ORCUS_DLLPUBLIC
  #else
    #ifdef __GNUC__
      #define ORCUS_DLLPUBLIC __attribute__ ((dllimport))
    #else
      #define ORCUS_DLLPUBLIC __declspec(dllimport)
    #endif
  #endif
  #define ORCUS_DLLLOCAL
#else
  #if __GNUC__ >= 4
    #define ORCUS_DLLPUBLIC __attribute__ ((visibility ("default")))
    #define ORCUS_DLLLOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define ORCUS_DLLPUBLIC
    #define ORCUS_DLLLOCAL
  #endif
#endif

// orcus-parser

#if defined _WIN32 || defined __CYGWIN__
  #if defined __MINGW32__
    #define ORCUS_PSR_DLLPUBLIC
    #define ORCUS_PSR_DLLLOCAL
  #elif defined __ORCUS_PSR_BUILDING_DLL
    #ifdef __GNUC__
      #define ORCUS_PSR_DLLPUBLIC __attribute__ ((dllexport))
    #else
      #define ORCUS_PSR_DLLPUBLIC __declspec(dllexport)
    #endif
  #elif defined __ORCUS_STATIC_LIB
      #define ORCUS_PSR_DLLPUBLIC
  #else
    #ifdef __GNUC__
      #define ORCUS_PSR_DLLPUBLIC __attribute__ ((dllimport))
    #else
      #define ORCUS_PSR_DLLPUBLIC __declspec(dllimport)
    #endif
  #endif
  #define ORCUS_PSR_DLLLOCAL
#else
  #if __GNUC__ >= 4
    #define ORCUS_PSR_DLLPUBLIC __attribute__ ((visibility ("default")))
    #define ORCUS_PSR_DLLLOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define ORCUS_PSR_DLLPUBLIC
    #define ORCUS_PSR_DLLLOCAL
  #endif
#endif

// orcus-spreadsheet-model

#if defined _WIN32 || defined __CYGWIN__
  #if defined __MINGW32__
    #define ORCUS_SPM_DLLPUBLIC
    #define ORCUS_SPM_DLLLOCAL
  #elif defined __ORCUS_SPM_BUILDING_DLL
    #ifdef __GNUC__
      #define ORCUS_SPM_DLLPUBLIC __attribute__ ((dllexport))
    #else
      #define ORCUS_SPM_DLLPUBLIC __declspec(dllexport)
    #endif
  #elif defined __ORCUS_STATIC_LIB
      #define ORCUS_SPM_DLLPUBLIC
  #else
    #ifdef __GNUC__
      #define ORCUS_SPM_DLLPUBLIC __attribute__ ((dllimport))
    #else
      #define ORCUS_SPM_DLLPUBLIC __declspec(dllimport)
    #endif
  #endif
  #define ORCUS_SPM_DLLLOCAL
#else
  #if __GNUC__ >= 4
    #define ORCUS_SPM_DLLPUBLIC __attribute__ ((visibility ("default")))
    #define ORCUS_SPM_DLLLOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define ORCUS_SPM_DLLPUBLIC
    #define ORCUS_SPM_DLLLOCAL
  #endif
#endif

// orcus-mso

#if defined _WIN32 || defined __CYGWIN__
  #if defined __MINGW32__
    #define ORCUS_MSO_DLLPUBLIC
    #define ORCUS_MSO_DLLLOCAL
  #elif defined __ORCUS_MSO_BUILDING_DLL
    #ifdef __GNUC__
      #define ORCUS_MSO_DLLPUBLIC __attribute__ ((dllexport))
    #else
      #define ORCUS_MSO_DLLPUBLIC __declspec(dllexport)
    #endif
  #elif defined __ORCUS_STATIC_LIB
      #define ORCUS_MSO_DLLPUBLIC
  #else
    #ifdef __GNUC__
      #define ORCUS_MSO_DLLPUBLIC __attribute__ ((dllimport))
    #else
      #define ORCUS_MSO_DLLPUBLIC __declspec(dllimport)
    #endif
  #endif
  #define ORCUS_MSO_DLLLOCAL
#else
  #if __GNUC__ >= 4
    #define ORCUS_MSO_DLLPUBLIC __attribute__ ((visibility ("default")))
    #define ORCUS_MSO_DLLLOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define ORCUS_MSO_DLLPUBLIC
    #define ORCUS_MSO_DLLLOCAL
  #endif
#endif

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
