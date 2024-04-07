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
#include "data_uri_utils.h"
#include <vector>
#include <memory>
#include <regex>
#include "securec.h"

namespace OHOS {
namespace AppExecFwk {

namespace {
const string EMPTY = "";
const std::regex INTEGER_REGEX("^[0-9]+$");
const int BUFFER_LEN = 32;
const char *SEPARATOR = "/";
};  // namespace

/**
 * @brief Default constructor of DataUriUtils class
 * @return None
 */
DataUriUtils::DataUriUtils()
{}

/**
 * @brief Default deconstructor of DataUriUtils class
 * @return None
 */
DataUriUtils::~DataUriUtils()
{}

/**
 * @brief Attaches the given ID to the end of the path component of the given URI.
 * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
 * @param id
 * @return Uri( scheme://authority/path1/path2/path3/updateIDNumber....)
 */
Uri DataUriUtils::AttachId(const Uri &dataUri, long id)
{
    // 1. get Path
    string path = const_cast<Uri &>(dataUri).GetPath();
    if (path.empty()) {
        return dataUri;
    }

    string uriString = dataUri.ToString();

    std::vector<string> pathVector;
    const_cast<Uri &>(dataUri).GetPathSegments(pathVector);
    if (pathVector.empty()) {
        return dataUri;
    }
    string lastPath = pathVector[pathVector.size() - 1];

    char longBuffer[BUFFER_LEN] = {0};

    int ret = sprintf_s(longBuffer, sizeof(longBuffer), "%ld", id);
    if (ret == -1) {
        return dataUri;
    }
    // new path string (lastPath+SEPARATOR+number)
    string newLastPath("");

    newLastPath = lastPath + string(SEPARATOR) + string(longBuffer);

    // find "/+lastPath"
    string tempLastPath = string(SEPARATOR) + lastPath;
    auto lastPathPos = uriString.rfind(tempLastPath);

    uriString.replace(lastPathPos + 1, tempLastPath.size() - 1, newLastPath.c_str());
    return Uri(uriString);
}

/**
 * @brief Obtains the ID attached to the end of the path component of the given URI.
 * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
 * @return long ID
 */
long DataUriUtils::GetId(const Uri &dataUri)
{
    // 1. get Path
    string path = const_cast<Uri &>(dataUri).GetPath();
    if (path.empty()) {
        return -1;
    }
    std::vector<string> pathVector;
    const_cast<Uri &>(dataUri).GetPathSegments(pathVector);
    if (pathVector.empty()) {
        return -1;
    }
    string lastPath = pathVector[pathVector.size() - 1];
    if (!IsNumber(lastPath)) {
        return -1;
    }

    return atoi(lastPath.c_str());
}

/**
 * @brief Deletes the ID from the end of the path component of the given URI.
 * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
 * @return long ID
 */
Uri DataUriUtils::DeleteId(const Uri &dataUri)
{
    return UriUpateLastPath(dataUri, EMPTY);
}

/**
 * @brief Updates the ID in the specified dataUri
 * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
 * @param id indiates Update attached to the end of the path component of the given URI
 * @return Uri return is the URI after path is updated
 */
Uri DataUriUtils::UpdateId(const Uri &dataUri, long id)
{
    char longBuffer[BUFFER_LEN] = {0};
    int ret = sprintf_s(longBuffer, sizeof(longBuffer), "%ld", id);
    if (ret == -1) {
        return dataUri;
    }

    string newLastPath("");
    newLastPath = string(longBuffer);

    return UriUpateLastPath(dataUri, newLastPath);
}

/**
 * @brief Does the end path of the path component of the given URI have an ID attached to it?
 * @param dataUri based on RFC 2396( Uniform Resource Identifier ).
 * @return bool
 */
bool DataUriUtils::IsAttachedId(const Uri &dataUri)
{
    // 1. get Path
    string path = const_cast<Uri &>(dataUri).GetPath();
    if (path.empty()) {
        return false;
    }

    std::vector<string> pathVector;
    const_cast<Uri &>(dataUri).GetPathSegments(pathVector);
    if (pathVector.empty()) {
        return false;
    }
    string lastPath = pathVector[pathVector.size() - 1];

    return IsNumber(lastPath);
}

/**
 * @brief Determine whether the string content is a numeric string
 * @param str indicates stirng.
 * @return bool
 */
bool DataUriUtils::IsNumber(const string &str)
{
    return std::regex_match(str, INTEGER_REGEX);
}

/**
 * @brief Determine whether the string content is a numeric string
 * @param dataUri indicates Uri object
   scheme://authority/path/aaa?query/#fragment
 * @return Uri return is the URI after path is updated
 */
Uri DataUriUtils::UriUpateLastPath(const Uri &dataUri, const string &updateLastPath)
{
    std::string strUpdateLastPath;

    if (updateLastPath.size() > 0) {
        strUpdateLastPath = SEPARATOR + updateLastPath;
    }

    // 1. get Path
    string path = const_cast<Uri &>(dataUri).GetPath();
    if (path.empty()) {
        return dataUri;
    }

    std::vector<string> pathVector;
    const_cast<Uri &>(dataUri).GetPathSegments(pathVector);
    if (pathVector.empty()) {
        return dataUri;
    }
    string lastPath = pathVector[pathVector.size() - 1];
    if (!IsNumber(lastPath)) {
        return dataUri;
    }

    string uriString = dataUri.ToString();
    // find "/+lastPath"
    int lastPathPos = uriString.rfind(string(SEPARATOR) + lastPath);
    if (lastPathPos == -1) {
        return dataUri;
    }

    // replace "/lastpath"==>""
    uriString.replace(lastPathPos, lastPath.size() + 1, strUpdateLastPath);

    return Uri(uriString);
}

}  // namespace AppExecFwk
}  // namespace OHOS