
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

      TODO : fill it.

   .. py:attribute:: name

      Read-only attribute that stores the name of the sheet.

   .. py:attribute:: sheet_size

      Read-only dictionary object that stores the column and row sizes of the
      sheet with the **column** and **row** keys, respectively.

   .. py:attribute:: data_size

      Read-only dictionary object that stores the column and row sizes of the
      data region of the sheet with the **column** and **row** keys, respectively.
      The data region is the smallest possible range whose top-left corner is
      at column 0 and row 0 that encompasses all non-empty cells.

.. py:class:: SheetRows

   An instance of this class represents an iterator for rows inside a sheet.
