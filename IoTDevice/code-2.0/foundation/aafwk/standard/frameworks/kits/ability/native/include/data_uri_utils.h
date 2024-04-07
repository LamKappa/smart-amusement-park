/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DATA_URI_UTIL_H
#define DATA_URI_UTIL_H

#include <cstring>
#include "uri.h"

using string = std::string;
using Uri = OHOS::Uri;

namespace OHOS {
namespace AppExecFwk {
class DataUriUtils final {
public:
    /**
     * @brief Default constructor of DataUriUtils class
     * @return None
     */
    DataUriUtils();

    /**
     * @brief Default deconstructor of DataUriUtils class
     * @return None
     */
    ~DataUriUtils();

    /**
     * @brief Attaches the given ID to the end of the path component of the given URI.
     * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
     * @param id
     * @return Uri( scheme://authority/path1/path2/path3/updateIDNumber....)
     */
    static Uri AttachId(const Uri &dataUri, long id);

    /**
     * @brief Obtains the ID attached to the end of the path component of the given URI.
     * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
     * @return long ID
     */
    static long GetId(const Uri &dataUri);

    /**
     * @brief Deletes the ID from the end of the path component of the given URI.
     * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
     * @return long ID
     */
    static Uri DeleteId(const Uri &dataUri);

    /**
     * @brief Updates the ID in the specified dataUri
     * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
     * @param id indiates Update attached to the end of the path component of the given URI
     * @return Uri return is the URI after path is updated
     */
    static Uri UpdateId(const Uri &dataUri, long id);

    /**
     * @brief Does the end path of the path component of the given URI have an ID attached to it?
     * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
     * @return bool
     */
    static bool IsAttachedId(const Uri &dataUri);

private:
    /**
     * @brief Determine whether the string content is a numeric string
     * @param str indicates stirng.
     * @return bool
     */
    static bool IsNumber(const string &str);

    /**
     * @brief Determine whether the string content is a numeric string
     * @param dataUri indicates Uri object
     * @return Uri return is the URI after path is updated
     */
    static Uri UriUpateLastPath(const Uri &dataUri, const string &updateLastPath);
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif
