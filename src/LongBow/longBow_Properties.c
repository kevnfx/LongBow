/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center (PARC)
 * All rights reserved.
 *  
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution. 
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2015, Xerox Corporation (Xerox) and Palo Alto Research Center (PARC).  All rights reserved.
 */
//#include <config.h>

#include <string.h>

#include <LongBow/private/longBow_String.h>
#include <LongBow/longBow_Properties.h>
#include <LongBow/private/longBow_ArrayList.h>
#include <LongBow/private/longBow_Memory.h>

struct LongBowProperties {
    LongBowArrayList *list;
};

typedef struct Property {
    const char *name;
    const char *value;
} _Property;

static void
_property_Destroy(_Property **pointer)
{
    _Property *property = *pointer;
    longBowMemory_Deallocate((void **) &property->name);
    longBowMemory_Deallocate((void **) &property->value);
    longBowMemory_Deallocate((void **) property);
}

LongBowProperties *
longBowProperties_Create(void)
{
    LongBowProperties *result = longBowMemory_Allocate(sizeof(LongBowProperties));

    if (result != NULL) {
        result->list = longBowArrayList_Create((void (*)(void **)) _property_Destroy);
    }

    return result;
}

static _Property *
_longBowProperties_Get(const LongBowProperties *properties, const char *name)
{
    _Property *result = NULL;

    for (size_t index = 0; index < longBowArrayList_Length(properties->list); index++) {
        _Property *property = longBowArrayList_Get(properties->list, index);
        if (strcmp(property->name, name) == 0) {
            result = property;
            break;
        }
    }
    return result;
}

const char *
longBowProperties_Get(const LongBowProperties *properties, const char *name)
{
    _Property *property = _longBowProperties_Get(properties, name);

    if (property != NULL) {
        return property->value;
    }

    return NULL;
}

bool
longBowProperties_Set(LongBowProperties *properties, const char *name, const char *value)
{
    bool result = false;

    _Property *property = _longBowProperties_Get(properties, name);
    if (property == NULL) {
        property = longBowMemory_Allocate(sizeof(_Property));
        property->name = longBowMemory_StringCopy(name);
        property->value = longBowMemory_StringCopy(value);
        longBowArrayList_Add(properties->list, property);
        result = true;
    } else {
        longBowMemory_Deallocate((void **) &property->value);
        property->value = longBowMemory_StringCopy(value);
    }
    return result;
}

bool
longBowProperties_Exists(const LongBowProperties *properties, const char *name)
{
    return (_longBowProperties_Get(properties, name) == NULL) ? false : true;
}

bool
longBowProperties_Delete(LongBowProperties *properties, const char *name)
{
    bool result = false;

    for (size_t index = 0; index < longBowArrayList_Length(properties->list); index++) {
        _Property *property = longBowArrayList_Get(properties->list, index);
        if (strcmp(property->name, name) == 0) {
            longBowArrayList_RemoveAtIndex(properties->list, index);
            result = true;
        }
    }
    
    return result;
}

size_t
longBowProperties_Length(const LongBowProperties *properties)
{
    return longBowArrayList_Length(properties->list);
}

char *
longBowProperties_ToString(const LongBowProperties *properties)
{
    LongBowString *string = longBowString_Create(128);

    for (size_t index = 0; index < longBowArrayList_Length(properties->list); index++) {
        _Property *property = longBowArrayList_Get(properties->list, index);
        longBowString_Format(string, "%s=%s\n", property->name, property->value);
    }

    char *result = longBowString_ToString(string);
    longBowString_Destroy(&string);

    return result;
}

void
longBowProperties_Destroy(LongBowProperties **propertiesPtr)
{
    LongBowProperties *properties = *propertiesPtr;
    longBowArrayList_Destroy(&properties->list);
    longBowMemory_Deallocate((void **) propertiesPtr);
}
