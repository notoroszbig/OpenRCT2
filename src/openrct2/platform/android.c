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

#ifdef __ANDROID__

#include "platform.h"
#include "../config/Config.h"
#include "../localisation/language.h"
#include "../util/Util.h"
#include <wchar.h>
#include <jni.h>
#include <SDL.h>

void platform_get_exe_path(utf8 *outPath, size_t outSize)
{
    safe_strcpy(outPath, "/sdcard/openrct2", outSize);
}

#ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size)
{
    STUB();
    return false;
}
#endif

void platform_posix_sub_resolve_openrct_data_path(utf8 *out, size_t size) {
    safe_strcpy(out, "/sdcard/openrct2", size);
}


uint16 platform_get_locale_language() {
    return LANGUAGE_ENGLISH_UK;
}

uint8 platform_get_locale_currency() {
    return platform_get_currency_value(NULL);
}

uint8 platform_get_locale_measurement_format() {
    return MEASUREMENT_FORMAT_METRIC;
}

float platform_get_default_scale() {
    JNIEnv *env = SDL_AndroidGetJNIEnv();

    jobject *activity = (jobject *) SDL_AndroidGetActivity();
    jclass *activityClass = (*env)->GetObjectClass(env, activity);
    jmethodID getDefaultScale = (*env)->GetMethodID(env, activityClass, "getDefaultScale", "()F");

    jfloat displayScale = (*env)->CallFloatMethod(env, activity, getDefaultScale);

    (*env)->DeleteLocalRef(env, activity);
    (*env)->DeleteLocalRef(env, activityClass);

    return displayScale;
}

void platform_get_changelog_path(utf8 *outPath, size_t outSize)
{
    STUB();
}

bool platform_get_steam_path(utf8 * outPath, size_t outSize)
{
    return false;
}

#endif
