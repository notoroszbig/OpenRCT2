#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "core/Math.hpp"
#include "core/Util.hpp"
#include "Game.h"
#include "object.h"
#include "ObjectList.h"
#include "object/ObjectRepository.h"
#include "util/SawyerCoding.h"
#include "util/Util.h"

// 98DA00
sint32 object_entry_group_counts[] = {
    MAX_RIDE_OBJECTS,          // rides
    MAX_SMALL_SCENERY_OBJECTS, // small scenery
    MAX_LARGE_SCENERY_OBJECTS, // large scenery
    MAX_WALL_SCENERY_OBJECTS,  // walls
    MAX_BANNER_OBJECTS,        // banners
    MAX_PATH_OBJECTS,          // paths
    MAX_PATH_ADDITION_OBJECTS, // path bits
    MAX_SCENERY_GROUP_OBJECTS, // scenery sets
    MAX_PARK_ENTRANCE_OBJECTS, // park entrance
    MAX_WATER_OBJECTS,         // water
    MAX_SCENARIO_TEXT_OBJECTS  // scenario text
};

// 98DA2C
sint32 object_entry_group_encoding[] = {
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_ROTATE
};

    rct_ride_entry              *gRideEntries[MAX_RIDE_OBJECTS];
    rct_small_scenery_entry     *gSmallSceneryEntries[MAX_SMALL_SCENERY_OBJECTS];
    rct_large_scenery_entry     *gLargeSceneryEntries[MAX_LARGE_SCENERY_OBJECTS];
    rct_wall_scenery_entry      *gWallSceneryEntries[MAX_WALL_SCENERY_OBJECTS];
    rct_banner                  *gBannerSceneryEntries[MAX_BANNER_OBJECTS];
    rct_footpath_entry          *gFootpathEntries[MAX_PATH_OBJECTS];
    rct_path_bit_scenery_entry  *gFootpathAdditionEntries[MAX_PATH_ADDITION_OBJECTS];
    rct_scenery_group_entry     *gSceneryGroupEntries[MAX_SCENERY_GROUP_OBJECTS];
    rct_entrance_type           *gParkEntranceEntries[MAX_PARK_ENTRANCE_OBJECTS];
    rct_water_type              *gWaterEntries[MAX_WATER_OBJECTS];
    rct_stex_entry              *gStexEntries[MAX_SCENARIO_TEXT_OBJECTS];

    static rct_object_entry_extended _objectEntriesRides[MAX_RIDE_OBJECTS];
    static rct_object_entry_extended _objectEntriesSmallScenery[MAX_SMALL_SCENERY_OBJECTS];
    static rct_object_entry_extended _objectEntriesLargeScenery[MAX_LARGE_SCENERY_OBJECTS];
    static rct_object_entry_extended _objectEntriesWalls[MAX_WALL_SCENERY_OBJECTS];
    static rct_object_entry_extended _objectEntriesBanners[MAX_BANNER_OBJECTS];
    static rct_object_entry_extended _objectEntriesFootpaths[MAX_PATH_OBJECTS];
    static rct_object_entry_extended _objectEntriesFootpathAdditions[MAX_PATH_ADDITION_OBJECTS];
    static rct_object_entry_extended _objectEntriesSceneryGroups[MAX_SCENERY_GROUP_OBJECTS];
    static rct_object_entry_extended _objectEntriesParkEntrances[MAX_PARK_ENTRANCE_OBJECTS];
    static rct_object_entry_extended _objectEntriesWaters[MAX_WATER_OBJECTS];
    static rct_object_entry_extended _objectEntriesStexs[MAX_SCENARIO_TEXT_OBJECTS];


// 0x98D97C chunk address', 0x98D980 object_entries
const rct_object_entry_group object_entry_groups[] = {
    (void**)(gRideEntries               ), _objectEntriesRides, // rides
    (void**)(gSmallSceneryEntries       ), _objectEntriesSmallScenery,  // small scenery    0x009AD1A4, 0xF2FA3C
    (void**)(gLargeSceneryEntries       ), _objectEntriesLargeScenery,  // large scenery    0x009AD594, 0xF40DEC
    (void**)(gWallSceneryEntries        ), _objectEntriesWalls, // walls            0x009AD794, 0xF417EC
    (void**)(gBannerSceneryEntries      ), _objectEntriesBanners,   // banners          0x009AD994, 0xF421EC
    (void**)(gFootpathEntries           ), _objectEntriesFootpaths, // paths            0x009ADA14, 0xF4246C
    (void**)(gFootpathAdditionEntries   ), _objectEntriesFootpathAdditions, // path bits        0x009ADA54, 0xF425AC
    (void**)(gSceneryGroupEntries       ), _objectEntriesSceneryGroups, // scenery sets     0x009ADA90, 0xF426D8
    (void**)(gParkEntranceEntries       ), _objectEntriesParkEntrances, // park entrance    0x009ADADC, 0xF42854
    (void**)(gWaterEntries              ), _objectEntriesWaters,    // water            0x009ADAE0, 0xF42868
    (void**)(gStexEntries               ), _objectEntriesStexs, // scenario text    0x009ADAE4, 0xF4287C
};

bool object_entry_is_empty(const rct_object_entry *entry)
{
    uint64 a, b;
    memcpy(&a, (uint8 *)entry, 8);
    memcpy(&b, (uint8 *)entry + 8, 8);

    if (a == 0xFFFFFFFFFFFFFFFF && b == 0xFFFFFFFFFFFFFFFF) return true;
    if (a == 0 && b == 0) return true;
    return false;
}

uint8 object_entry_get_source_game(const rct_object_entry * objectEntry)
{
    return (objectEntry->flags & 0xF0) >> 4;
}

/**
 *
 *  rct2: 0x006AB344
 */
void object_create_identifier_name(char* string_buffer, size_t size, const rct_object_entry* object)
{
    snprintf(string_buffer, size, "%.8s/%4X%4X", object->name, object->flags, object->checksum);
}

/**
 *
 *  rct2: 0x006A9DA2
 * bl = entry_index
 * ecx = entry_type
 */
sint32 find_object_in_entry_group(const rct_object_entry* entry, uint8* entry_type, uint8* entry_index){
    if ((entry->flags & 0xF) >= Util::CountOf(object_entry_groups)) {
        return 0;
    }
    *entry_type = entry->flags & 0xF;
    rct_object_entry_group entry_group = object_entry_groups[*entry_type];
    for (*entry_index = 0;
        *entry_index < object_entry_group_counts[*entry_type];
        ++(*entry_index),
        entry_group.chunks++,
        entry_group.entries++){

        if (*entry_group.chunks == NULL) continue;

        if (object_entry_compare((rct_object_entry*)entry_group.entries, entry))break;
    }

    if (*entry_index == object_entry_group_counts[*entry_type])return 0;
    return 1;
}

void get_type_entry_index(size_t index, uint8 * outObjectType, uint8 * outEntryIndex)
{
    uint8 objectType = OBJECT_TYPE_RIDE;
    for (size_t groupCount : object_entry_group_counts)
    {
        if (index >= groupCount) {
            index -= groupCount;
            objectType++;
        } else {
            break;
        }
    }

    if (outObjectType != NULL) *outObjectType = objectType;
    if (outEntryIndex != NULL) *outEntryIndex = (uint8)index;
}

const rct_object_entry * get_loaded_object_entry(size_t index)
{
    uint8 objectType, entryIndex;
    get_type_entry_index(index, &objectType, &entryIndex);

    rct_object_entry * entry = (rct_object_entry *)&(object_entry_groups[objectType].entries[entryIndex]);
    return entry;
}

void * get_loaded_object_chunk(size_t index)
{
    uint8 objectType, entryIndex;
    get_type_entry_index(index, &objectType, &entryIndex);

    void *entry = object_entry_groups[objectType].chunks[entryIndex];
    return entry;
}

void object_entry_get_name_fixed(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry)
{
    bufferSize = Math::Min((size_t)9, bufferSize);
    memcpy(buffer, entry->name, bufferSize - 1);
    buffer[bufferSize - 1] = 0;
}
