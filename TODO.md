* Enable/disable escaping characters in JsonWriter
* Enable/disable quotes around keys in JsonWriter

Perhaps:
* Convert null to 0 in read-int.
* Convert true to 1 in read-int.
* Convert false to 0 in read-int.
* Convert null to NaN in read-double.
* Convert null to "" in read-string if the string is a value, not if it's a key.
