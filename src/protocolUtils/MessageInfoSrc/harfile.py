#!/usr/bin/python

# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# modified by qian

import re

def MakeDefaultHeaders(list_o_dicts, items_to_ignore=[]):
  retval = {}
  for kvdict in list_o_dicts:
    key = kvdict["name"]
    val = kvdict["value"]
    if key == "host":
      key = ":host"
    if key in items_to_ignore:
      continue
    if key in retval:
      retval[key] = retval[key] + '\0' + val
    else:
      retval[key] = val
  return retval

def ReadHarFileForHttp(filename):
  f = open(filename)
  null = None
  true = 1
  false = 0
  s = f.read()
  o = eval(s)
  request_headers = []
  response_headers = []
  timings_dict = []

  for entry in o["log"]["entries"]:
    request = entry["request"]
    header = MakeDefaultHeaders(request["headers"])
    header[":method"] = request["method"].lower()
    header[":path"] = re.sub("^[^:]*://[^/]*/","/", request["url"])
    header[":version"] = re.sub("^[^/]*/","", request["httpVersion"])
    header[":scheme"] = re.sub("^([^:]*):.*$", '\\1', request["url"]).lower()
    if not ":host" in request_headers:
      header[":host"] = re.sub("^[^:]*://([^/]*)/.*$","\\1", request["url"])
    if not header[":scheme"] in ["http", "https"]:
      continue
    request_headers.append(header)

    response = entry["response"]
    header = MakeDefaultHeaders(response["headers"],
        ["status", "status-text", "version"])
    header[":status"] = re.sub("^([0-9]*).*","\\1", str(response["status"]))
    header[":status-text"] = response["statusText"]
    header[":version"] = re.sub("^[^/]*/","", response["httpVersion"])
    response_headers.append(header)
    
    timings = entry["timings"]
    timings_dict.append(timings)
    
  return (request_headers, response_headers, timings_dict)

# For spdy name/value pair header block  
# difference between http and spdy is: (reference to "A Methodology to Derive SPDY's Initial Dictionary for Zlib Compression")
#    1. strip certain headers: Connection, Keep-Alive, Proxy-Connection (note that when ignore them, all the value is changed to lowercase)
#    2. change HTTP Header Field Names to Only use lowercase
def MakeDefaultLowerHeaders(list_o_dicts, items_to_ignore=[]):
  retval = {}
  for kvdict in list_o_dicts:
    key = kvdict["name"].lower()
    val = kvdict["value"]
    if key == "host":
      key = ":host"
    if key in items_to_ignore:
      continue
    if key in retval:
      retval[key] = retval[key] + '\0' + val
    else:
      retval[key] = val
  return retval
  
def ReadHarFileForSpdy(filename):
  f = open(filename)
  null = None
  true = 1
  false = 0
  s = f.read()
  o = eval(s)
  request_headers = []
  response_headers = []
  timings_dict = []

  for entry in o["log"]["entries"]:
    request = entry["request"]
    header = MakeDefaultLowerHeaders(request["headers"], ["connection", "keep-alive", "proxy-connection"])
    header[":method"] = request["method"].lower()
    header[":path"] = re.sub("^[^:]*://[^/]*/","/", request["url"])
    header[":version"] = re.sub("^[^/]*/","", request["httpVersion"])
    header[":scheme"] = re.sub("^([^:]*):.*$", '\\1', request["url"]).lower()
    if not ":host" in request_headers:
      header[":host"] = re.sub("^[^:]*://([^/]*)/.*$","\\1", request["url"])
    if not header[":scheme"] in ["http", "https"]:
      continue
    request_headers.append(header)

    response = entry["response"]
    header = MakeDefaultLowerHeaders(response["headers"],
        ["connection", "keep-alive", "proxy-connection", "status", "status-text", "version"])
    header[":status"] = re.sub("^([0-9]*).*","\\1", str(response["status"]))
    header[":status-text"] = response["statusText"]
    header[":version"] = re.sub("^[^/]*/","", response["httpVersion"])
    response_headers.append(header)
    
    timings = entry["timings"]
    timings_dict.append(timings)
    
  return (request_headers, response_headers, timings_dict)
