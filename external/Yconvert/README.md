# Yconvert – text encoding conversion library 

## Supported encodings

- UTF-8
- UTF-16 (big and little endian)
- UTF-32 (big and little endian)
- All ISO 8859 code pages 
- All Windows single byte code pages (CP1250 etc.)
- All DOS code pages (CP437 etc.)
- Subset of Mac code pages (roman etc.)

## Examples

### Convert Windows CP1252 to UTF-8, UTF-16 and UTF-32

```cxx
#include <Yconvert/Yconvert.hpp>

int main()
{
    std::string text_cp1252 = /* ... */;

    auto textU8 = Yconvert::convert_to<std::string>(
        text_cp1252,
        Yconvert::Encoding::WIN_CP1252,
        Yconvert::Encoding::UTF_8); 

    auto textU16 = Yconvert::convert_to<std::u16string>(
        text_cp1252,
        Yconvert::Encoding::WIN_CP1252,
        Yconvert::Encoding::UTF_16_NATIVE); 

    auto textU32 = Yconvert::convert_to<std::u32string>(
        text_cp1252,
        Yconvert::Encoding::WIN_CP1252,
        Yconvert::Encoding::UTF_32_NATIVE);

    /* ... */ 
}

```
