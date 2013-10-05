/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_ENV_HPP__
#define __ORCUS_ENV_HPP__

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

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
