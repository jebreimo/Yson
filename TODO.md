* Escape characters in JsonWriter
* Unescape character in JsonReader
* Enable/disable escaping characters in JsonWriter
* Enable/disable quotes around keys in JsonWriter
* Make nextDocument more usable (requires peek-functionality in JsonTokenizer)

Perhaps:
* Convert null to 0 in read-int.
* Convert true to 1 in read-int.
* Convert false to 0 in read-int.
* Convert null to NaN in read-double.
* Convert null to "" in read-string if the string is a value, not if it's a key.
