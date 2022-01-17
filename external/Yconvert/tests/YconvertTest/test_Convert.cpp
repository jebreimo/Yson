//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-24.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Convert.hpp"

#include <catch2/catch.hpp>

using namespace Yconvert;

TEST_CASE("Convert ISO 8859-10 -> UTF-8")
{
    std::string s = "AB\xE7\xF1";
    REQUIRE(convertTo<std::string>(s, Encoding::ISO_8859_10, Encoding::UTF_8) == u8"ABįņ");
}

TEST_CASE("Convert ISO 8859-10 -> UTF-16")
{
    std::string s = "AB\xE7\xF1";
    REQUIRE(convertTo<std::u16string>(s, Encoding::ISO_8859_10, Encoding::UTF_16_NATIVE) == u"ABįņ");
}

TEST_CASE("Convert UTF-32 -> UTF-16")
{
    std::u32string s = U"ABįņ";
    REQUIRE(convertTo<std::u16string>(s, Encoding::UTF_32_NATIVE, Encoding::UTF_16_NATIVE) == u"ABįņ");
}

TEST_CASE("Convert long wstring")
{
    std::wstring w = L"Property(Name = D.01_Prosess_1, Value = 74.4191, Utlegging av matjord (A-sjikt))Property(Name ="
                     L" D.02_Prosess_2, Value = 74.4191, Utlegging av matjord (B-sjikt))Property(Name ="
                     L" A.01_Objektnavn, Value = )Property(Name = E.01_Overflatebehandling, Value = Matjorda skal"
                     L" legges ut løst med ujevn overflate på ruglete/løs/ujevn undergrunnsmasse. Matjorda skal ikke"
                     L" komprimeres.)Property(Name = E.02_Sjikt_01, Value = Matjord, A-sjikt, t= avhengig av avtak,"
                     L" min. 50 cm)Property(Name = E.03_Sjikt_02, Value = Matjord, B-sjikt, t= avhengig av avtak, ca"
                     L" 20-40 cm)Property(Name = E.04_Sjikt_03, Value = Usoldet jord fra linja, til traubunn/fjell)"
                     L"Property(Name = A.02_Objektkode_tekst, Value = Matjord)Property(Name = A.03_Objektkode_nummer,"
                     L" Value = 744190)Property(Name = A.04_Beskrivelse, Value = Mellomlagret matjord tilbakeført fra"
                     L" ranker på samme eiendom.)Property(Name = B.01_Parsell, Value = )Property(Name = B.02_Område,"
                     L" Value = )Property(Name = C.01_MMI, Value = )Property(Name = D.03_Prosess_3, Value = 25.223,"
                     L" Usoldet jord fra linja)Property(Name = F.01_Merknad, Value = Omfatter også nødvendig"
                     L" utsortering av stubber, røtter, kvist og stein/klumper med tykkelse 50-100mm. All"
                     L" tilbakeføring skal skje med beltegående maskin.)Property(Name = G.01_Vertikalnivå, Value = 1)";
    std::string s = u8"Property(Name = D.01_Prosess_1, Value = 74.4191, Utlegging av matjord (A-sjikt))Property(Name ="
                    u8" D.02_Prosess_2, Value = 74.4191, Utlegging av matjord (B-sjikt))Property(Name ="
                    u8" A.01_Objektnavn, Value = )Property(Name = E.01_Overflatebehandling, Value = Matjorda skal"
                    u8" legges ut løst med ujevn overflate på ruglete/løs/ujevn undergrunnsmasse. Matjorda skal ikke"
                    u8" komprimeres.)Property(Name = E.02_Sjikt_01, Value = Matjord, A-sjikt, t= avhengig av avtak,"
                    u8" min. 50 cm)Property(Name = E.03_Sjikt_02, Value = Matjord, B-sjikt, t= avhengig av avtak, ca"
                    u8" 20-40 cm)Property(Name = E.04_Sjikt_03, Value = Usoldet jord fra linja, til traubunn/fjell)"
                    u8"Property(Name = A.02_Objektkode_tekst, Value = Matjord)Property(Name = A.03_Objektkode_nummer,"
                    u8" Value = 744190)Property(Name = A.04_Beskrivelse, Value = Mellomlagret matjord tilbakeført fra"
                    u8" ranker på samme eiendom.)Property(Name = B.01_Parsell, Value = )Property(Name = B.02_Område,"
                    u8" Value = )Property(Name = C.01_MMI, Value = )Property(Name = D.03_Prosess_3, Value = 25.223,"
                    u8" Usoldet jord fra linja)Property(Name = F.01_Merknad, Value = Omfatter også nødvendig"
                    u8" utsortering av stubber, røtter, kvist og stein/klumper med tykkelse 50-100mm. All"
                    u8" tilbakeføring skal skje med beltegående maskin.)Property(Name = G.01_Vertikalnivå, Value = 1)";
    REQUIRE(convertTo<std::string>(w, Encoding::WSTRING_NATIVE, Encoding::UTF_8) == s);
}
