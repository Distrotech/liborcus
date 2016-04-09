
Python API
==========

Spreadsheet Document
--------------------

.. py:module:: orcus

.. py:class:: Document

   An instance of this class represents a document model.  A document consists
   of multiple sheet objects.

   .. py:attribute:: sheets

      Read-only attribute that stores a tuple of :py:class:`.Sheet` instance
      objects.

.. py:class:: Sheet

   An instance of this class represents a single sheet inside a document.

   .. py:function:: get_rows

      This function returns a row iterator object that allows you to iterate
      through rows in the data region.

      :rtype: :py:class:`.SheetRows`
      :return: row iterator object.

      Example::

         rows = sheet.get_rows()

         for row in rows:
             print(row)  # tuple of cell values

   .. py:attribute:: name

      Read-only attribute that stores the name of the sheet.

   .. py:attribute:: sheet_size

      Read-only dictionary object that stores the column and row sizes of the
      sheet with the **column** and **row** keys, respectively.

   .. py:attribute:: data_size

      Read-only dictionary object that stores the column and row sizes of the
      data region of the sheet with the **column** and **row** keys, respectively.
      The data region is the smallest possible range that includes all non-empty
      cells in the sheet.  The top-left corner of the data region is always at
      the top-left corner of the sheet.

.. py:class:: SheetRows

   An instance of this class represents an iterator for rows inside a sheet.


File Loader
-----------

.. py:module:: orcus.xlsx

.. py:function:: read_file

   Read an Excel file from a specified file path and create a
   :py:class:`orcus.Document` instance object.  The file must be of Excel 2007
   XML format.

   :param filepath: file path.
   :rtype: :py:class:`orcus.Document`
   :return: document instance object that stores the content of the file.

   Example::

      from orcus import xlsx

      doc = xlsx.read_file("/path/to/file.xlsx")
