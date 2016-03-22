#!/usr/bin/env python3

from orcus import xlsx

filepath = "./test/xlsx/raw-values-1/input.xlsx"
doc = xlsx.read_file(filepath)
print(doc)
