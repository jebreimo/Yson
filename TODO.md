* Convert string-values to int in read-int.
* Convert null to 0 in read-int.
* Convert true to 1 in read-int.
* Convert false to 0 in read-int.
* Implement read-double.
* Convert null to NaN in read-double.
* Convert null to "" in read-string if the string is a value, not if it's a key.

* Add a raw-token function.
* Add a read any value as string function. 
* Reconsider current enter/leave restrictions.

* Special read-functions for reading incorrect types?

* enter/leave/nextKey/nextValue must handle null. AFTER_NULL_ENTRY?
* Test performance with a big json-file.
    * Test with UTF-8.
    * Test with UTF-16.

