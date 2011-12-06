/*
 * Copyright (C) 2011, British Broadcasting Corporation
 * All Rights Reserved.
 *
 * Author: Philip de Nier
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the British Broadcasting Corporation nor the names
 *       of its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __STDC_FORMAT_MACROS

#include <cstdio>
#include <cstring>
#include <cerrno>

#include <AS11Helper.h>
#include <im/as11/AS11DMS.h>
#include <im/as11/UKDPPDMS.h>
#include "AppUtils.h"
#include <im/IMException.h>
#include <im/Logging.h>

using namespace std;
using namespace im;
using namespace mxfpp;



static const PropertyInfo AS11_CORE_PROPERTY_INFO[] =
{
    {"SeriesTitle",                 MXF_ITEM_K(AS11CoreFramework, AS11SeriesTitle)},
    {"ProgrammeTitle",              MXF_ITEM_K(AS11CoreFramework, AS11ProgrammeTitle)},
    {"EpisodeTitleNumber",          MXF_ITEM_K(AS11CoreFramework, AS11EpisodeTitleNumber)},
    {"ShimName",                    MXF_ITEM_K(AS11CoreFramework, AS11ShimName)},
    {"AudioTrackLayout",            MXF_ITEM_K(AS11CoreFramework, AS11AudioTrackLayout)},
    {"PrimaryAudioLanguage",        MXF_ITEM_K(AS11CoreFramework, AS11PrimaryAudioLanguage)},
    {"ClosedCaptionsPresent",       MXF_ITEM_K(AS11CoreFramework, AS11ClosedCaptionsPresent)},
    {"ClosedCaptionsType",          MXF_ITEM_K(AS11CoreFramework, AS11ClosedCaptionsType)},
    {"ClosedCaptionsLanguage",      MXF_ITEM_K(AS11CoreFramework, AS11ClosedCaptionsLanguage)},

    {0, g_Null_Key},
};

static const PropertyInfo AS11_SEGMENTATION_PROPERTY_INFO[] =
{
    {"PartNumber",                  MXF_ITEM_K(AS11SegmentationFramework,   AS11PartNumber)},
    {"PartTotal",                   MXF_ITEM_K(AS11SegmentationFramework,   AS11PartTotal)},

    {0, g_Null_Key},
};

static const PropertyInfo UK_DPP_PROPERTY_INFO[] =
{
    {"ProductionNumber",            MXF_ITEM_K(UKDPPFramework, UKDPPProductionNumber)},
    {"Synopsis",                    MXF_ITEM_K(UKDPPFramework, UKDPPSynopsis)},
    {"Originator",                  MXF_ITEM_K(UKDPPFramework, UKDPPOriginator)},
    {"CopyrightYear",               MXF_ITEM_K(UKDPPFramework, UKDPPCopyrightYear)},
    {"OtherIdentifier",             MXF_ITEM_K(UKDPPFramework, UKDPPOtherIdentifier)},
    {"OtherIdentifierType",         MXF_ITEM_K(UKDPPFramework, UKDPPOtherIdentifierType)},
    {"Genre",                       MXF_ITEM_K(UKDPPFramework, UKDPPGenre)},
    {"Distributor",                 MXF_ITEM_K(UKDPPFramework, UKDPPDistributor)},
    {"PictureRatio",                MXF_ITEM_K(UKDPPFramework, UKDPPPictureRatio)},
    {"3D",                          MXF_ITEM_K(UKDPPFramework, UKDPP3D)},
    {"3DType",                      MXF_ITEM_K(UKDPPFramework, UKDPP3DType)},
    {"ProductPlacement",            MXF_ITEM_K(UKDPPFramework, UKDPPProductPlacement)},
    {"FPAPass",                     MXF_ITEM_K(UKDPPFramework, UKDPPFPAPass)},
    {"FPAManufacturer",             MXF_ITEM_K(UKDPPFramework, UKDPPFPAManufacturer)},
    {"FPAVersion",                  MXF_ITEM_K(UKDPPFramework, UKDPPFPAVersion)},
    {"VideoComments",               MXF_ITEM_K(UKDPPFramework, UKDPPVideoComments)},
    {"SecondaryAudioLanguage",      MXF_ITEM_K(UKDPPFramework, UKDPPSecondaryAudioLanguage)},
    {"TertiaryAudioLanguage",       MXF_ITEM_K(UKDPPFramework, UKDPPTertiaryAudioLanguage)},
    {"AudioLoudnessStandard",       MXF_ITEM_K(UKDPPFramework, UKDPPAudioLoudnessStandard)},
    {"AudioComments",               MXF_ITEM_K(UKDPPFramework, UKDPPAudioComments)},
    {"LineUpStart",                 MXF_ITEM_K(UKDPPFramework, UKDPPLineUpStart)},
    {"IdentClockStart",             MXF_ITEM_K(UKDPPFramework, UKDPPIdentClockStart)},
    {"TotalNumberOfParts",          MXF_ITEM_K(UKDPPFramework, UKDPPTotalNumberOfParts)},
    {"TotalProgrammeDuration",      MXF_ITEM_K(UKDPPFramework, UKDPPTotalProgrammeDuration)},
    {"AudioDescriptionPresent",     MXF_ITEM_K(UKDPPFramework, UKDPPAudioDescriptionPresent)},
    {"AudioDescriptionType",        MXF_ITEM_K(UKDPPFramework, UKDPPAudioDescriptionType)},
    {"OpenCaptionsPresent",         MXF_ITEM_K(UKDPPFramework, UKDPPOpenCaptionsPresent)},
    {"OpenCaptionsType",            MXF_ITEM_K(UKDPPFramework, UKDPPOpenCaptionsType)},
    {"OpenCaptionsLanguage",        MXF_ITEM_K(UKDPPFramework, UKDPPOpenCaptionsLanguage)},
    {"SigningPresent",              MXF_ITEM_K(UKDPPFramework, UKDPPSigningPresent)},
    {"SignLanguage",                MXF_ITEM_K(UKDPPFramework, UKDPPSignLanguage)},
    {"CompletionDate",              MXF_ITEM_K(UKDPPFramework, UKDPPCompletionDate)},
    {"TextlessElementsExist",       MXF_ITEM_K(UKDPPFramework, UKDPPTextlessElementsExist)},
    {"ProgrammeHasText",            MXF_ITEM_K(UKDPPFramework, UKDPPProgrammeHasText)},
    {"ProgrammeTextLanguage",       MXF_ITEM_K(UKDPPFramework, UKDPPProgrammeTextLanguage)},
    {"ContactEmail",                MXF_ITEM_K(UKDPPFramework, UKDPPContactEmail)},
    {"ContactTelephoneNo",          MXF_ITEM_K(UKDPPFramework, UKDPPContactTelephoneNo)},

    {0, g_Null_Key},
};

static const FrameworkInfo FRAMEWORK_INFO[] =
{
    {"AS11CoreFramework",           MXF_SET_K(AS11CoreFramework),           AS11_CORE_PROPERTY_INFO},
    {"AS11SegmentationFramework",   MXF_SET_K(AS11SegmentationFramework),   AS11_SEGMENTATION_PROPERTY_INFO},
    {"UKDPPFramework",              MXF_SET_K(UKDPPFramework),              UK_DPP_PROPERTY_INFO},

    {0, g_Null_Key, 0},
};



static string trim_string(string value)
{
    size_t start;
    size_t len;

    // trim spaces from the start
    start = 0;
    while (start < value.size() && isspace(value[start]))
        start++;
    if (start >= value.size())
        return "";

    // trim spaces from the end by reducing the length
    len = value.size() - start;
    while (len > 0 && isspace(value[start + len - 1]))
        len--;

    return value.substr(start, len);
}

static bool parse_fw_bool(string value, bool *bool_value)
{
    if (value == "true" || value == "1") {
        *bool_value = true;
        return true;
    } else if (value == "false" || value == "0") {
        *bool_value = false;
        return true;
    }

    log_warn("Invalid boolean value '%s'\n", value.c_str());
    return false;
}

static bool parse_uint8(string value, uint8_t *uint8_value)
{
    unsigned int tmp;
    if (sscanf(value.c_str(), "%u", &tmp) == 1 && tmp < 256) {
        *uint8_value = (uint8_t)tmp;
        return true;
    }

    log_warn("Invalid uint8 value '%s'\n", value.c_str());
    return false;
}

static bool parse_uint16(string value, uint16_t *uint16_value)
{
    unsigned int tmp;
    if (sscanf(value.c_str(), "%u", &tmp) == 1 && tmp < 65536) {
        *uint16_value = (uint16_t)tmp;
        return true;
    }

    log_warn("Invalid uint16 value '%s'\n", value.c_str());
    return false;
}

static bool parse_int64(string value, int64_t *int64_value)
{
    if (sscanf(value.c_str(), "%"PRId64"", int64_value) == 1)
        return true;

    log_warn("Invalid int64 value '%s'\n", value.c_str());
    return false;
}

static bool parse_timestamp(string value, mxfTimestamp *timestamp_value)
{
    int year;
    unsigned int month, day, hour, min, sec;
    if (sscanf(value.c_str(), "%d-%u-%uT%u:%u:%u ", &year, &month, &day, &hour, &min, &sec) == 6) {
        timestamp_value->year  = year;
        timestamp_value->month = month;
        timestamp_value->day   = day;
        timestamp_value->hour  = hour;
        timestamp_value->min   = min;
        timestamp_value->sec   = sec;
        timestamp_value->qmsec = 0;
        return true;
    } else if (sscanf(value.c_str(), "%d-%u-%u ", &year, &month, &day) == 3) {
        timestamp_value->year  = year;
        timestamp_value->month = month;
        timestamp_value->day   = day;
        timestamp_value->hour  = 0;
        timestamp_value->min   = 0;
        timestamp_value->sec   = 0;
        timestamp_value->qmsec = 0;
        return true;
    }

    log_warn("Invalid timestamp value '%s'\n", value.c_str());
    return false;
}

static bool parse_rational(string value, mxfRational *rational_value)
{
    int num, den;
    if (sscanf(value.c_str(), "%d/%d", &num, &den) == 2) {
        rational_value->numerator   = num;
        rational_value->denominator = den;
        return true;
    }

    log_warn("Invalid rational value '%s'\n", value.c_str());
    return false;
}

static size_t get_utf8_code_len(const char *u8_code)
{
    if ((unsigned char)u8_code[0] < 0x80)
    {
        return 1;
    }
    else if (((unsigned char)u8_code[0] & 0xe0) == 0xc0 &&
             ((unsigned char)u8_code[1] & 0xc0) == 0x80)
    {
        return 2;
    }
    else if (((unsigned char)u8_code[0] & 0xf0) == 0xe0 &&
             ((unsigned char)u8_code[1] & 0xc0) == 0x80 &&
             ((unsigned char)u8_code[2] & 0xc0) == 0x80)
    {
        return 3;
    }
    else if (((unsigned char)u8_code[0] & 0xf8) == 0xf0 &&
             ((unsigned char)u8_code[1] & 0xc0) == 0x80 &&
             ((unsigned char)u8_code[2] & 0xc0) == 0x80 &&
             ((unsigned char)u8_code[3] & 0xc0) == 0x80)
    {
        return 4;
    }

    return -1;
}

static size_t get_utf8_clip_len(const char *u8_str, size_t max_unicode_len)
{
    size_t unicode_len = 0;
    size_t len = 0;
    while (u8_str[len] && unicode_len < max_unicode_len) {
        size_t code_len = get_utf8_code_len(&u8_str[len]);
        if (code_len == (size_t)(-1))
            break;
        len += code_len;
        unicode_len++;
    }

    return len;
}



FrameworkHelper::FrameworkHelper(DataModel *data_model, DMFramework *framework)
{
    mFramework = framework;
    IM_ASSERT(data_model->findSetDef(framework->getKey(), &mSetDef));

    const FrameworkInfo *framework_info = FRAMEWORK_INFO;
    while (framework_info->name) {
        if (framework_info->set_key == *framework->getKey()) {
            mFrameworkInfo = framework_info;
            break;
        }
        framework_info++;
    }
    IM_ASSERT(framework_info->name);
}

FrameworkHelper::~FrameworkHelper()
{
}

bool FrameworkHelper::SetProperty(string name, string value)
{
    const PropertyInfo *property_info = mFrameworkInfo->property_info;
    while (property_info->name) {
        if (name == property_info->name)
            break;
        property_info++;
    }
    if (!property_info->name) {
        log_warn("Unknown framework property %s::%s\n", mFrameworkInfo->name, name.c_str());
        return false;
    }

    ItemDef *item_def;
    IM_ASSERT(mSetDef->findItemDef(&property_info->item_key, &item_def));
    MXFItemDef *c_item_def = item_def->getCItemDef();

    switch (c_item_def->typeId)
    {
        case MXF_UTF16STRING_TYPE:
        {
            if (value.empty()) {
                log_warn("Ignoring empty string property %s::%s\n", mFrameworkInfo->name, name.c_str());
            } else {
                size_t clip_len = get_utf8_clip_len(value.c_str(), 127);
                if (clip_len == 0) {
                    log_warn("Ignoring invalid UTF-8 string property %s::%s\n", mFrameworkInfo->name, name.c_str());
                } else {
                    if (value.size() > clip_len) {
                        log_warn("Clipping string property %s::%s because it's length exceeds 127 unicode chars\n",
                                 mFrameworkInfo->name, name.c_str());
                    }
                    mFramework->setStringItem(&c_item_def->key, value.substr(0, clip_len));
                }
            }
            break;
        }
        case MXF_UINT8_TYPE:
        {
            uint8_t uint8_value = 0;
            if (!parse_uint8(value, &uint8_value)) {
                log_warn("Invalid framework property value %s::%s '%s'\n",  mFrameworkInfo->name, name.c_str(),
                         value.c_str());
                return false;
            }

            mFramework->setUInt8Item(&c_item_def->key, uint8_value);
            break;
        }
        case MXF_UINT16_TYPE:
        {
            uint16_t uint16_value = 0;
            if (!parse_uint16(value, &uint16_value)) {
                log_warn("Invalid framework property value %s::%s '%s'\n",  mFrameworkInfo->name, name.c_str(),
                         value.c_str());
                return false;
            }

            mFramework->setUInt16Item(&c_item_def->key, uint16_value);
            break;
        }
        case MXF_POSITION_TYPE:
        case MXF_LENGTH_TYPE:
        case MXF_INT64_TYPE:
        {
            int64_t int64_value = 0;
            if (!parse_int64(value, &int64_value)) {
                log_warn("Invalid framework property value %s::%s '%s'\n",  mFrameworkInfo->name, name.c_str(),
                         value.c_str());
                return false;
            }

            mFramework->setInt64Item(&c_item_def->key, int64_value);
            break;
        }
        case MXF_BOOLEAN_TYPE:
        {
            bool bool_value = false;
            if (!parse_fw_bool(value, &bool_value)) {
                log_warn("Invalid framework property value %s::%s '%s'\n",  mFrameworkInfo->name, name.c_str(),
                         value.c_str());
                return false;
            }

            mFramework->setBooleanItem(&c_item_def->key, bool_value);
            break;
        }
        case MXF_TIMESTAMP_TYPE:
        {
            mxfTimestamp timestamp_value = {0, 0, 0, 0, 0, 0, 0};
            if (!parse_timestamp(value, &timestamp_value)) {
                log_warn("Invalid framework property value %s::%s '%s'\n",  mFrameworkInfo->name, name.c_str(),
                         value.c_str());
                return false;
            }

            mFramework->setTimestampItem(&c_item_def->key, timestamp_value);
            break;
        }
        case MXF_RATIONAL_TYPE:
        {
            mxfRational rational_value = {0, 0};
            if (!parse_rational(value, &rational_value)) {
                log_warn("Invalid framework property value %s::%s '%s'\n",  mFrameworkInfo->name, name.c_str(),
                         value.c_str());
                return false;
            }

            mFramework->setRationalItem(&c_item_def->key, rational_value);
            break;
        }
        default:
            IM_ASSERT(false);
            return false;
    }

    return true;
}



AS11Helper::AS11Helper()
{
    mFillerCompleteSegments = false;
    mClip = 0;
    mAS11FrameworkHelper = 0;
    mUKDPPFrameworkHelper = 0;
    mHaveUKDPPTotalNumberOfParts = false;
    mHaveUKDPPTotalProgrammeDuration = false;
}

AS11Helper::~AS11Helper()
{
    delete mAS11FrameworkHelper;
    delete mUKDPPFrameworkHelper;
}

bool AS11Helper::ParseFrameworkFile(const char *type_str, const char *filename)
{
    FrameworkType type;
    if (!ParseFrameworkType(type_str, &type))
        return false;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    int line_num = 0;
    int c = '1';
    while (c != EOF) {
        // move file pointer past the newline characters
        while ((c = fgetc(file)) != EOF && (c == '\r' || c == '\n')) {
            if (c == '\n')
                line_num++;
        }

        string name, value;
        bool parse_name = true;
        while (c != EOF && (c != '\r' && c != '\n')) {
            if (c == ':' && parse_name) {
                parse_name = false;
            } else {
                if (parse_name)
                    name += c;
                else
                    value += c;
            }

            c = fgetc(file);
        }
        if (!name.empty()) {
            if (parse_name) {
                fprintf(stderr, "Failed to parse line %d\n", line_num);
                fclose(file);
                return false;
            }

            SetFrameworkProperty(type, trim_string(name), trim_string(value));
        }

        if (c == '\n')
            line_num++;
    }

    fclose(file);

    return true;
}

bool AS11Helper::ParseSegmentationFile(const char *filename, Rational frame_rate)
{
    mSegments.clear();
    mFillerCompleteSegments = false;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    bool last_tc_xx = false;
    int line_num = 0;
    int c = '1';
    while (c != EOF) {
        // move file pointer past the newline characters
        while ((c = fgetc(file)) != EOF && (c == '\r' || c == '\n')) {
            if (c == '\n')
                line_num++;
        }

        AS11TCSegment segment;
        segment.start.Init(frame_rate, false);

        string part_number_string;
        string tc_pair_string;
        int space_count = 0;
        size_t second_tc_start = 0;
        while (c != EOF && (c != '\r' && c != '\n')) {
            if (space_count < 2) {
                tc_pair_string += c;
                if (c == ' ') {
                    if (space_count == 0)
                        second_tc_start = tc_pair_string.size();
                    space_count++;
                }
                if (space_count == 2) {
                    last_tc_xx = false;

                    Timecode end;
                    if (!parse_timecode(tc_pair_string.c_str(), frame_rate, &segment.start)) {
                        fprintf(stderr, "Failed to parse 1st timecode on line %d\n", line_num);
                        fclose(file);
                        mSegments.clear();
                        return false;
                    }
                    if (!parse_timecode(&tc_pair_string[second_tc_start], frame_rate, &end)) {
                        if (trim_string(&tc_pair_string[second_tc_start]) != "xx:xx:xx:xx") {
                            fprintf(stderr, "Failed to parse 2nd timecode on line %d\n", line_num);
                            fclose(file);
                            mSegments.clear();
                            return false;
                        }

                        // segment extends to package duration
                        last_tc_xx = true;
                        segment.duration = 0;
                    } else {
                        segment.duration = end.GetOffset() - segment.start.GetOffset() + 1;
                        if (segment.duration < 0) // assume crossed midnight
                            segment.duration += end.GetMaxOffset();
                    }
                }
            } else {
                part_number_string += c;
            }

            c = fgetc(file);
        }
        if (!tc_pair_string.empty()) {
            if (space_count != 2) {
                fprintf(stderr, "Failed to parse line %d\n", line_num);
                fclose(file);
                mSegments.clear();
                return false;
            }

            int pnum, ptotal;
            if (sscanf(part_number_string.c_str(), "%d/%d", &pnum, &ptotal) != 2 ||
                pnum < 0 || ptotal <= 0)
            {
                fprintf(stderr, "Failed to parse valid part number on line %d\n", line_num);
                fclose(file);
                mSegments.clear();
                return false;
            }
            segment.part_number = pnum;
            segment.part_total = ptotal;

            mSegments.push_back(segment);
        }

        if (c == '\n')
            line_num++;
    }

    mFillerCompleteSegments = !last_tc_xx;


    fclose(file);

    return true;
}

bool AS11Helper::SetFrameworkProperty(const char *type_str, const char *name, const char *value)
{
    FrameworkType type;
    if (!ParseFrameworkType(type_str, &type))
        return false;

    SetFrameworkProperty(type, name, value);
    return true;
}

bool AS11Helper::HaveProgrammeTitle() const
{
    size_t i;
    for (i = 0; i < mFrameworkProperties.size(); i++) {
        if (mFrameworkProperties[i].type == AS11_CORE_FRAMEWORK_TYPE &&
            mFrameworkProperties[i].name == "ProgrammeTitle")
        {
            return true;
        }
    }

    return false;
}

string AS11Helper::GetProgrammeTitle() const
{
    size_t i;
    for (i = 0; i < mFrameworkProperties.size(); i++) {
        if (mFrameworkProperties[i].type == AS11_CORE_FRAMEWORK_TYPE &&
            mFrameworkProperties[i].name == "ProgrammeTitle")
        {
            return mFrameworkProperties[i].value;
        }
    }

    return "";
}

void AS11Helper::InsertFrameworks(AS11Clip *as11_clip)
{
    mClip = as11_clip;
    AS11DMS::RegisterExtensions(mClip->GetDataModel());
    UKDPPDMS::RegisterExtensions(mClip->GetDataModel());

    size_t i;
    for (i = 0; i < mFrameworkProperties.size(); i++) {
        if (mFrameworkProperties[i].type == AS11_CORE_FRAMEWORK_TYPE) {
            if (!mAS11FrameworkHelper) {
                mAS11FrameworkHelper = new FrameworkHelper(mClip->GetDataModel(),
                                                           new AS11CoreFramework(mClip->GetHeaderMetadata()));
            }
            if (!mAS11FrameworkHelper->SetProperty(mFrameworkProperties[i].name, mFrameworkProperties[i].value)) {
                log_warn("Failed to set AS11CoreFramework property '%s' to '%s'\n",
                         mFrameworkProperties[i].name.c_str(), mFrameworkProperties[i].value.c_str());
            }
        } else {
            if (!mUKDPPFrameworkHelper) {
                mUKDPPFrameworkHelper = new FrameworkHelper(mClip->GetDataModel(),
                                                            new UKDPPFramework(mClip->GetHeaderMetadata()));
            }
            if (!mUKDPPFrameworkHelper->SetProperty(mFrameworkProperties[i].name, mFrameworkProperties[i].value)) {
                log_warn("Failed to set UKDPPCoreFramework property '%s' to '%s'\n",
                         mFrameworkProperties[i].name.c_str(), mFrameworkProperties[i].value.c_str());
            }
        }
    }


    if (mAS11FrameworkHelper) {
        mClip->InsertAS11CoreFramework(dynamic_cast<AS11CoreFramework*>(mAS11FrameworkHelper->GetFramework()));
        IM_CHECK_M(mAS11FrameworkHelper->GetFramework()->validate(true), ("AS11 Framework validation failed"));
    }

    if (!mSegments.empty())
        mClip->InsertTCSegmentation(mSegments);

    if (mUKDPPFrameworkHelper) {
        mClip->InsertUKDPPFramework(dynamic_cast<UKDPPFramework*>(mUKDPPFrameworkHelper->GetFramework()));

        // make sure UKDPPTotalNumberOfParts and UKDPPTotalProgrammeDuration are set (default 0) for validation
        UKDPPFramework *dpp_framework = dynamic_cast<UKDPPFramework*>(mUKDPPFrameworkHelper->GetFramework());
        if (dpp_framework->haveItem(&MXF_ITEM_K(UKDPPFramework, UKDPPTotalNumberOfParts)))
            mHaveUKDPPTotalNumberOfParts = true;
        else
            dpp_framework->SetTotalNumberOfParts(0);
        if (dpp_framework->haveItem(&MXF_ITEM_K(UKDPPFramework, UKDPPTotalProgrammeDuration)))
            mHaveUKDPPTotalProgrammeDuration = true;
        else
            dpp_framework->SetTotalProgrammeDuration(0);

        IM_CHECK_M(dpp_framework->validate(true), ("UK DPP Framework validation failed"));
    }
}

void AS11Helper::Complete()
{
    if (!mSegments.empty())
        mClip->CompleteSegmentation(mFillerCompleteSegments);

    if (mUKDPPFrameworkHelper) {
        // calculate or check total number of parts and programme duration
        UKDPPFramework *dpp_framework = dynamic_cast<UKDPPFramework*>(mUKDPPFrameworkHelper->GetFramework());
        if (mHaveUKDPPTotalNumberOfParts) {
            if (mClip->GetTotalSegments() != dpp_framework->GetTotalNumberOfParts()) {
                log_warn("UKDPPTotalNumberOfParts value %u does not equal actual total part count %u\n",
                         dpp_framework->GetTotalNumberOfParts(), mClip->GetTotalSegments());
            }
        } else {
            dpp_framework->SetTotalNumberOfParts(mClip->GetTotalSegments());
        }
        if (mHaveUKDPPTotalProgrammeDuration) {
            if (mClip->GetTotalSegmentDuration() != dpp_framework->GetTotalProgrammeDuration()) {
                log_warn("UKDPPTotalProgrammeDuration value %u does not equal actual total part duration %u\n",
                         dpp_framework->GetTotalProgrammeDuration(), mClip->GetTotalSegmentDuration());
            }
        } else {
            dpp_framework->SetTotalProgrammeDuration(mClip->GetTotalSegmentDuration());
        }
    }
}

bool AS11Helper::ParseFrameworkType(const char *type_str, FrameworkType *type) const
{
    if (strcmp(type_str, "as11") == 0) {
        *type = AS11_CORE_FRAMEWORK_TYPE;
        return true;
    } else if (strcmp(type_str, "dpp") == 0) {
        *type = DPP_FRAMEWORK_TYPE;
        return true;
    }

    log_warn("Unknown framework type '%s'\n", type_str);
    return false;
}

void AS11Helper::SetFrameworkProperty(FrameworkType type, std::string name, std::string value)
{
    FrameworkProperty framework_property;
    framework_property.type  = type;
    framework_property.name  = name;
    framework_property.value = value;

    mFrameworkProperties.push_back(framework_property);
}
