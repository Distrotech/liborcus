#!/usr/bin/env python3

from orcus import xlsx

filepath = "./test/xlsx/raw-values-1/input.xlsx"
doc = xlsx.read_file(filepath)
print("number of sheets: {}".format(len(doc.sheets)))
for sheet in doc.sheets:
    print("---")
    print("- sheet name: {}".format(sheet.name))
    print("- sheet size: (column:{column}, row:{row})".format(**sheet.sheet_size))
    print("- data size: (column:{column}, row:{row})".format(**sheet.data_size))

