Yson - JSON serialization and de-serialization made easy
========================================================

Yson provides classes for reading and writing JSON files, and is particularly well suited for serialization/de-serialization and processing very large JSON files. The library has two main classes: JsonReader and JsonWriter. JsonReader is somewhat inspired by .Net's [XmlReader](https://msdn.microsoft.com/en-us/library/system.xml.xmlreader.aspx) and makes it easy to iterate through the contents of a JSON file. JsonWriter is similarly inspired by .Net's [XmlWriter](https://msdn.microsoft.com/en-us/library/system.xml.xmlwriter.aspx) and writes JSON contents directly to a stream or file. 

JsonReader
==========

Main features
-------------
* Supports all standard text encodings plus more.
* Supports various extensions to the basic JSON syntax, including JavaScript comments and multiple documents in the same file.
* Memory-efficient and fairly fast when processing large files.
* Released under the 2-clause *BSD Licence*.

The high-level interface of JsonReader consists of the following members:
-------------------------------------------------------------------------
* its two constructors, one taking in a std::istream, the other the name of a file.
* *nextKey()* for advancing to the next key in a JSON-object.
* *nextValue()* for advancing to the next value in a JSON-array or JSON-object.
* *enter()* for entering the JSON-array or JSON-object starting at the current position in the file. nextKey() and nextValue() will only advance through the most recently entered structure, structures that haven been entered will be skipped entirely by next time either of the two functions is called.
* *leave()* for leaving the most recently *entered* JSON-array or JSON-object.
* *read(...)* for reading the current key or value.
* *valueType()* returns the value type of the current key or value.

Example
-------
To read and process a file name scores.json

```json
[
    {
        "first-name": "Ben",
        "last-name": "Benson",
        "year-of-birth": 1970,
        "score": 86.765
    },
    {
        "first-name": "Carl",
        "last-name": "Carlson",
        "year-of-birth": 1980,
        "score": 91.201
    }
]
```

Code

```cpp
#include <iostream>
#include <Yson/JsonReader.hpp>

struct Person
{
    std::string firstName;
    std::string lastName;
    int yearOfBirth;
    double score;
};

Person deserializePerson(Yson::JsonReader& reader)
{
    Person person;

    // Enter the JSON object and make nextKey/nextValue apply there.
    reader.enter();

    // For each key in the object
    while (reader.nextKey())
    {
        // Read the key
        auto key = read<std::string>(reader);

        // Advance the reader from the key to the value
        reader.nextValue();

        // Depending on the key, read the corresponding value
        if (key == "first-name")
            reader.read(person.firstName);
        else if (key == "last-name")
            reader.read(person.lastName);
        else if (key == "year-of-birth")
            reader.read(person.yearOfBirth);
        else if (key == "score")
            reader.read(person.score);
    }

    // Leave the JSON object an make nextKey/nextValue apply to the parent structure.
    reader.leave();

    return person;
}

int main()
{
    std::vector<Person> persons;

    try
    {
        // Open the file
        Yson::JsonReader reader("scores.json");

        // Advance to the top-level "value" in the file, i.e. the opening bracket.
        if (!reader.nextValue())
            return 1; // 

        // Enter the top-level array and make nextValue apply there.
        reader.enter();
    	
        // Process each object inside the top-level array
        while (reader.nextValue())
        {
            // Deserialize the Person in a separate function
            persons.push_back(readPerson(reader));
        }

        // Leave the top-level array.
        reader.leave();
    }
    catch (Yson::JsonReaderException& ex)
    {
        std::cerr << ex << "\n";
    }
    	
    // ...
}
```

### The low-level interface supplements the high-level interface with the following members:

* *nextToken()* moves to the next token in the JSON contents, including whitespace and commas.
* *tokenType()* returns the type of the current token.
* *token()* returns the current token as string.

### The following extensions to JSON are supported (most must be explicitly enabled):

* comments, both //... and /*...*/
* multi-line strings """...."""
* special integer formats (0x... for hexadecimals, 0o... for octals and 0b... for binary numbers)
* non-string keys
* single-word keys without quotes
* multiple JSON top-level documents in a single file or stream (iterated over with *nextDocument()*).
* support for the special floating point values NaN and positive and negative inifinity. 

### The following encodings are supported and automatically detected:

* UTF-8 with and without BOM (plain ASCII is also treated as UTF-8)
* UTF-16 little-endian and big-endian, with and without BOM
* UTF-32 little-endian and big-endian, with and without BOM

JsonWriter
==========
