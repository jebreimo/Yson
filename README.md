A library for reading and writing JSON files.

The library has two main classes: JsonReader and JsonWriter. JsonReader is somewhat inspired by Microsoft's [XmlReader](https://msdn.microsoft.com/en-us/library/system.xml.xmlreader.aspx) and makes it easy to iterate through the contents of a JSON file. JsonWriter is similarly inspired by Microsoft's [XmlWriter](https://msdn.microsoft.com/en-us/library/system.xml.xmlwriter.aspx) and writes JSON contents directly to a stream or file.

JsonReader
==========

Example
-------

To read a file name scores.json

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
#include <Yson/JsonReader.hpp>

struct Person
{
    std::string firstName;
    std::string lastName;
    int yearOfBirth;
    double score;
};

Person readPerson(Yson::JsonReader& reader)
{
    Person person;
    while (reader.nextKey())
    {
        std::string key;
        reader.read(key);
        reader.nextValue();
        if (key == "first-name")
            reader.read(person.firstName);
        else if (key == "last-name")
            reader.read(person.lastName);
        else if (key == "year-of-birth")
            reader.read(person.yearOfBirth);
        else if (key == "score")
            reader.read(person.score);
    }
    return person;
}

int main()
{
    Yson::JsonReader reader("scores.json");
    std::vector<Person> persons;

    if (!reader.nextValue())
        return 1; 
    reader.enter();
    while (reader.nextValue())
    {
        reader.enter();
        persons.push_back(readPerson(reader));
        reader.leave();
    }
    reader.leave();
}
```

The high-level interface of JsonReader consists of the following members:

* its two constructors, one taking in a std::istream, the other the name of a file.
* nextKey() for advancing to the next key in a JSON-object.
* nextValue() for advancing to the next value in a JSON-array or JSON-object.
* enter() for entering the JSON-array or JSON-object starting at the current position in the file.
* leave() for leaving the most recently *entered* JSON-array or JSON-object.
* read(...) for reading the current key or value.
* valueType() returns the value type of the current key or value.
* isNull() checks if the current value is *null*.

The low-level interface supplements the high-level interface with the following members:

* nextToken() moves to the next token in the JSON contents, including whitespace and commas.
* tokenType() returns the type of the current token.
* token() returns the current token as string.

The following extensions to JSON are supported:

* comments, both //... and /*...*/
* multi-line strings """...."""
* special integer formats (0x... for hexadecimals, 0o... for octals and 0b... for binary numbers)
* non-string keys
* single-word keys without quotes
* and more...

The following encodings are supported and automatically detected:

* UTF-8 with and without BOM (plain ASCII is also treated as UTF-8)
* UTF-16 little-endian and big-endian, with and without BOM
* UTF-32 little-endian and big-endian, with and without BOM

In addition these encodings are supported, but must be specified manually:

* ISO8859-1
* ISO8859-10
* ISO8859-25
* IBM437
* IBM850
* Windows1252

JsonWriter
==========
