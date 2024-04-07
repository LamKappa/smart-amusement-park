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

#include "ohos/aafwk/content/skills.h"
#include "parcel_macro.h"
using namespace OHOS;
using namespace OHOS::AppExecFwk;
namespace OHOS {
namespace AAFwk {

const int LENGTH_FOR_FINDMINETYPE = 3;
/**
 * @brief Default constructor used to create a Skills instance.
 */
Skills::Skills()
{}

/**
 * @brief A parameterized constructor used to create a Skills instance.
 *
 * @param skills Indicates skills used to create a Skills instance.
 */
Skills::Skills(const Skills &skills)
{
    // entities_
    entities_ = skills.entities_;
    // actions_
    actions_ = skills.actions_;
    // authorities_
    authorities_ = skills.authorities_;
    // schemes_
    schemes_ = skills.schemes_;

    // paths_
    paths_ = skills.paths_;
    // schemeSpecificParts_
    schemeSpecificParts_ = skills.schemeSpecificParts_;
    // types_
    types_ = skills.types_;
}

Skills::~Skills()
{
    entities_.clear();
    actions_.clear();
    authorities_.clear();
    schemes_.clear();

    paths_.clear();
    schemeSpecificParts_.clear();
    types_.clear();
}

/**
 * @brief Obtains the list of entities.
 *
 * @return vector of Entities.
 */
std::vector<std::string> Skills::GetEntities() const
{
    return entities_;
}

/**
 * @brief Obtains the specified entity.
 *
 * @param entity Id of the specified entity.
 */
std::string Skills::GetEntity(int index) const
{
    if (index < 0 || entities_.empty() || std::size_t(index) >= entities_.size()) {
        return std::string();
    }
    return entities_.at(index);
}

/**
 * @brief Adds an entity to this Skills object.
 *
 * @param entity Indicates the entity to add.
 */
void Skills::AddEntity(const std::string &entity)
{
    auto it = std::find(entities_.begin(), entities_.end(), entity);
    if (it == entities_.end()) {
        entities_.emplace_back(entity);
    }
}

/**
 * @brief Checks whether the specified entity is exist.
 *
 * @param entity Name of the specified entity.
 */
bool Skills::HasEntity(const std::string &entity)
{
    return std::find(entities_.begin(), entities_.end(), entity) != entities_.end();
}

/**
 * @brief Remove the specified entity.
 *
 * @param entity Name of the specified entity.
 */
void Skills::RemoveEntity(const std::string &entity)
{
    if (!entities_.empty()) {
        auto it = std::find(entities_.begin(), entities_.end(), entity);
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }
}

/**
 * @brief Obtains the count of entities.
 *
 */
int Skills::CountEntities() const
{
    return entities_.empty() ? 0 : entities_.size();
}

/**
 * @brief Obtains the specified action.
 *
 * @param actionId Id of the specified action.
 */
std::string Skills::GetAction(int index) const
{
    if (index < 0 || actions_.empty() || std::size_t(index) >= actions_.size()) {
        return std::string();
    } else {
        return actions_.at(index);
    }
}

/**
 * @brief Adds an action to this Skills object.
 *
 * @param action Indicates the action to add.
 */
void Skills::AddAction(const std::string &action)
{
    auto it = std::find(actions_.begin(), actions_.end(), action);
    if (it == actions_.end()) {
        actions_.emplace_back(action);
    }
}

/**
 * @brief Checks whether the specified action is exist.
 *
 * @param action Name of the specified action.
 */
bool Skills::HasAction(const std::string &action)
{
    return std::find(actions_.begin(), actions_.end(), action) != actions_.end();
}

/**
 * @brief Remove the specified action.
 *
 * @param action Name of the specified action.
 */
void Skills::RemoveAction(const std::string &action)
{
    if (!actions_.empty()) {
        auto it = std::find(actions_.begin(), actions_.end(), action);
        if (it != actions_.end()) {
            actions_.erase(it);
        }
    }
}

/**
 * @brief Obtains the count of actions.
 *
 */
int Skills::CountActions() const
{
    return actions_.empty() ? 0 : actions_.size();
}

/**
 * @brief Obtains the iterator of Actions.
 *
 * @return iterator of Actions.
 */
std::vector<std::string>::iterator Skills::ActionsIterator()
{
    return actions_.begin();
}

/**
 * @brief Obtains the specified authority.
 *
 * @param authorityId Id of the specified authority.
 */
std::string Skills::GetAuthority(int index) const
{
    if (index < 0 || authorities_.empty() || std::size_t(index) >= authorities_.size()) {
        return std::string();
    } else {
        return authorities_.at(index);
    }
}

/**
 * @brief Adds an authority to this Skills object.
 *
 * @param authority Indicates the authority to add.
 */
void Skills::AddAuthority(const std::string &authority)
{
    auto it = std::find(authorities_.begin(), authorities_.end(), authority);
    if (it == authorities_.end()) {
        authorities_.emplace_back(authority);
    }
}

/**
 * @brief Checks whether the specified authority is exist.
 *
 * @param action Name of the specified authority.
 */
bool Skills::HasAuthority(const std::string &authority)
{
    return std::find(authorities_.begin(), authorities_.end(), authority) != authorities_.end();
}

/**
 * @brief Remove the specified authority.
 *
 * @param authority Name of the specified authority.
 */
void Skills::RemoveAuthority(const std::string &authority)
{
    if (!authorities_.empty()) {
        auto it = std::find(authorities_.begin(), authorities_.end(), authority);
        if (it != authorities_.end()) {
            authorities_.erase(it);
        }
    }
}

/**
 * @brief Obtains the count of authorities.
 *
 */
int Skills::CountAuthorities() const
{
    return authorities_.empty() ? 0 : authorities_.size();
}

/**
 * @brief Obtains the specified path.
 *
 * @param pathId Id of the specified path.
 */
std::string Skills::GetPath(int index) const
{
    if (index < 0 || paths_.empty() || std::size_t(index) >= paths_.size()) {
        return std::string();
    }
    return paths_.at(index).GetPattern();
}

/**
 * @brief Adds a path to this Skills object.
 *
 * @param path Indicates the path to add.
 */
void Skills::AddPath(const std::string &path)
{
    PatternsMatcher pm(path, MatchType::PATTERN_LITERAL);
    AddPath(pm);
}

/**
 * @brief Adds a path to this Skills object.
 *
 * @param path Indicates the path to add.
 */
void Skills::AddPath(const PatternsMatcher &patternsMatcher)
{
    auto hasPath = std::find_if(paths_.begin(), paths_.end(), [&patternsMatcher](const PatternsMatcher pm) {
        return (pm.GetPattern() == patternsMatcher.GetPattern()) && (pm.GetType() == patternsMatcher.GetType());
    });

    if (hasPath == paths_.end()) {
        paths_.emplace_back(patternsMatcher);
    }
}

/**
 * @brief Adds a path to this Skills object.
 *
 * @param path Indicates the path to add.
 * @param matchType the specified match type.
 */
void Skills::AddPath(const std::string &path, const MatchType &matchType)
{
    PatternsMatcher pm(path, matchType);
    AddPath(pm);
}

/**
 * @brief Checks whether the specified path is exist.
 *
 * @param path Name of the specified path.
 */
bool Skills::HasPath(const std::string &path)
{
    auto hasPath = std::find_if(
        paths_.begin(), paths_.end(), [&path](const PatternsMatcher pm) { return pm.GetPattern() == path; });
    return hasPath != paths_.end();
}

/**
 * @brief Remove the specified path.
 *
 * @param path Name of the specified path.
 */
void Skills::RemovePath(const std::string &path)
{
    auto hasPath = std::find_if(
        paths_.begin(), paths_.end(), [&path](const PatternsMatcher pm) { return pm.GetPattern() == path; });

    if (hasPath != paths_.end()) {
        paths_.erase(hasPath);
    }
}

/**
 * @brief Remove the specified path.
 *
 * @param path The path to be added.
 */
void Skills::RemovePath(const PatternsMatcher &patternsMatcher)
{
    auto hasPath = std::find_if(paths_.begin(), paths_.end(), [&patternsMatcher](const PatternsMatcher pm) {
        return (pm.GetPattern() == patternsMatcher.GetPattern()) && (pm.GetType() == patternsMatcher.GetType());
    });

    if (hasPath != paths_.end()) {
        paths_.erase(hasPath);
    }
}

/**
 * @brief Remove the specified path.
 *
 * @param path Name of the specified path.
 * @param matchType the specified match type.
 */
void Skills::RemovePath(const std::string &path, const MatchType &matchType)
{
    PatternsMatcher pm(path, matchType);
    RemovePath(pm);
}

/**
 * @brief Obtains the count of paths.
 *
 */
int Skills::CountPaths() const
{
    return paths_.empty() ? 0 : paths_.size();
}

/**
 * @brief Obtains the specified scheme.
 *
 * @param schemeId Id of the specified scheme.
 */
std::string Skills::GetScheme(int index) const
{
    if (index < 0 || schemes_.empty() || std::size_t(index) >= schemes_.size()) {
        return std::string();
    }
    return schemes_.at(index);
}

/**
 * @brief Adds an scheme to this Skills object.
 *
 * @param scheme Indicates the scheme to add.
 */
void Skills::AddScheme(const std::string &scheme)
{
    auto it = std::find(schemes_.begin(), schemes_.end(), scheme);
    if (it == schemes_.end()) {
        schemes_.emplace_back(scheme);
    }
}

/**
 * @brief Checks whether the specified scheme is exist.
 *
 * @param scheme Name of the specified scheme.
 */
bool Skills::HasScheme(const std::string &scheme)
{
    return std::find(schemes_.begin(), schemes_.end(), scheme) != schemes_.end();
}

/**
 * @brief Remove the specified scheme.
 *
 * @param scheme Name of the specified scheme.
 */
void Skills::RemoveScheme(const std::string &scheme)
{
    if (!schemes_.empty()) {
        auto it = std::find(schemes_.begin(), schemes_.end(), scheme);
        if (it != schemes_.end()) {
            schemes_.erase(it);
        }
    }
}

/**
 * @brief Obtains the count of schemes.
 *
 */
int Skills::CountSchemes() const
{
    return schemes_.empty() ? 0 : schemes_.size();
}

/**
 * @brief Obtains the specified scheme part.
 *
 * @param schemeId Id of the specified scheme part.
 */
std::string Skills::GetSchemeSpecificPart(int index) const
{
    if (index < 0 || schemeSpecificParts_.empty() || std::size_t(index) >= schemeSpecificParts_.size()) {
        return std::string();
    }
    return schemeSpecificParts_.at(index).GetPattern();
}

/**
 * @brief Adds an scheme to this Skills object.
 *
 * @param scheme Indicates the scheme to add.
 */
void Skills::AddSchemeSpecificPart(const std::string &schemeSpecificPart)
{
    PatternsMatcher patternsMatcher(schemeSpecificPart, MatchType::PATTERN_LITERAL);
    auto it = std::find_if(
        schemeSpecificParts_.begin(), schemeSpecificParts_.end(), [&patternsMatcher](const PatternsMatcher pm) {
            return (pm.GetPattern() == patternsMatcher.GetPattern()) && (pm.GetType() == patternsMatcher.GetType());
        });

    if (it == schemeSpecificParts_.end()) {
        schemeSpecificParts_.emplace_back(patternsMatcher);
    }
}

/**
 * @brief Checks whether the specified scheme part is exist.
 *
 * @param scheme Name of the specified scheme part.
 */
bool Skills::HasSchemeSpecificPart(const std::string &schemeSpecificPart)
{
    auto it = std::find_if(schemeSpecificParts_.begin(),
        schemeSpecificParts_.end(),
        [&schemeSpecificPart](const PatternsMatcher pm) { return pm.GetPattern() == schemeSpecificPart; });
    return it != schemeSpecificParts_.end();
}

/**
 * @brief Remove the specified scheme part.
 *
 * @param scheme Name of the specified scheme part.
 */
void Skills::RemoveSchemeSpecificPart(const std::string &schemeSpecificPart)
{
    auto it = std::find_if(schemeSpecificParts_.begin(),
        schemeSpecificParts_.end(),
        [&schemeSpecificPart](const PatternsMatcher pm) { return pm.GetPattern() == schemeSpecificPart; });

    if (it != schemeSpecificParts_.end()) {
        schemeSpecificParts_.erase(it);
    }
}

/**
 * @brief Obtains the count of scheme parts.
 *
 */
int Skills::CountSchemeSpecificParts() const
{
    return schemeSpecificParts_.empty() ? 0 : schemeSpecificParts_.size();
}

/**
 * @brief Obtains the specified type.
 *
 * @param typeId Id of the specified type.
 */
std::string Skills::GetType(int index) const
{
    if (index < 0 || types_.empty() || std::size_t(index) >= types_.size()) {
        return std::string();
    }
    return types_.at(index).GetPattern();
}

/**
 * @brief Adds a type to this Skills object.
 *
 * @param type Indicates the type to add.
 */
void Skills::AddType(const std::string &type)
{
    PatternsMatcher pm(type, MatchType::PATTERN_LITERAL);
    AddType(pm);
}

/**
 * @brief Adds a type to this Skills object.
 *
 * @param type Indicates the type to add.
 * @param matchType the specified match type.
 */
void Skills::AddType(const std::string &type, const MatchType &matchType)
{
    PatternsMatcher pm(type, matchType);
    AddType(pm);
}

/**
 * @brief Adds a type to this Skills object.
 *
 * @param type Indicates the type to add.
 */
void Skills::AddType(const PatternsMatcher &patternsMatcher)
{
    const size_t posNext = 1;
    const size_t posOffset = 2;
    std::string type = patternsMatcher.GetPattern();
    size_t slashpos = type.find('/');
    size_t typelen = type.length();
    if (slashpos != std::string::npos && typelen >= slashpos + posOffset) {
        if (typelen == slashpos + posOffset && type.at(slashpos + posNext) == '*') {
            PatternsMatcher pm(type.substr(0, slashpos), patternsMatcher.GetType());
            auto it = std::find_if(types_.begin(),
                types_.end(),
                [type = pm.GetPattern(), matchType = pm.GetType()](
                    const PatternsMatcher pm) { return (pm.GetPattern() == type) && (pm.GetType() == matchType); });
            if (it == types_.end()) {
                types_.emplace_back(pm);
            }
            hasPartialTypes_ = true;
        } else {
            PatternsMatcher pm(patternsMatcher);
            auto it = std::find_if(types_.begin(),
                types_.end(),
                [type = pm.GetPattern(), matchType = pm.GetType()](
                    const PatternsMatcher pm) { return (pm.GetPattern() == type) && (pm.GetType() == matchType); });
            if (it == types_.end()) {
                types_.emplace_back(pm);
            }
        }
    }
}

/**
 * @brief Checks whether the specified type is exist.
 *
 * @param type Name of the specified type.
 */
bool Skills::HasType(const std::string &type)
{
    auto it = std::find_if(
        types_.begin(), types_.end(), [&type](const PatternsMatcher pm) { return pm.GetPattern() == type; });
    return it != types_.end();
}

/**
 * @brief Remove the specified type.
 *
 * @param type Name of the specified type.
 */
void Skills::RemoveType(const std::string &type)
{
    auto it = std::find_if(
        types_.begin(), types_.end(), [&type](const PatternsMatcher pm) { return pm.GetPattern() == type; });

    if (it != types_.end()) {
        types_.erase(it);
    }
}

/**
 * @brief Remove the specified scheme type.
 *
 * @param patternsMatcher The type to be added.
 */
void Skills::RemoveType(const PatternsMatcher &patternsMatcher)
{
    auto it = std::find_if(types_.begin(), types_.end(), [&patternsMatcher](const PatternsMatcher pm) {
        return (pm.GetPattern() == patternsMatcher.GetPattern()) && (pm.GetType() == patternsMatcher.GetType());
    });

    if (it != types_.end()) {
        types_.erase(it);
    }
}

/**
 * @brief Remove the specified scheme type.
 *
 * @param type Name of the specified type.
 * @param matchType the specified match type.
 */
void Skills::RemoveType(const std::string &type, const MatchType &matchType)
{
    PatternsMatcher pm(type, matchType);
    RemoveType(pm);
}

/**
 * @brief Obtains the count of types.
 *
 */
int Skills::CountTypes() const
{
    return types_.empty() ? 0 : types_.size();
}

/**
 * @brief Match this skill against a Want's data.
 *
 * @param want The desired want data to match for.
 */
bool Skills::Match(const Want &want)
{
    if (want.GetAction() != std::string() && !MatchAction(want.GetAction())) {
        return false;
    }

    int dataMatch = MatchData(want.GetType(), want.GetScheme(), want.GetUri());
    if (dataMatch < 0) {
        return false;
    }

    std::string entityMismatch = MatchEntities(want.GetEntities());
    if (entityMismatch == std::string()) {
        return false;
    }

    return true;
}

/**
 * @brief Match this skills against a Want's entities.  Each entity in
 * the Want must be specified by the skills; if any are not in the
 * skills, the match fails.
 *
 * @param entities The entities included in the want, as returned by
 *                   Want.getEntities().
 *
 * @return If all entities match (success), null; else the name of the
 *         first entity that didn't match.
 */
std::string Skills::MatchEntities(const std::vector<std::string> &entities)
{
    if (!entities.empty()) {
        size_t size = entities.size();
        for (size_t i = 0; i < size; i++) {
            auto it = std::find(entities_.begin(), entities_.end(), entities[i]);
            if (it != entities_.end()) {
                return entities[i];
            }
        }
    }

    return std::string();
}

/**
 * @brief Match this skills against a Want's action.  If the skills does not
 * specify any actions, the match will always fail.
 *
 * @param action The desired action to look for.
 *
 * @return True if the action is listed in the skills.
 */
bool Skills::MatchAction(const std::string &action)
{
    return HasAction(action);
}

/**
 * @brief Match this skills against a Want's data (type, scheme and path).
 *
 * @param type The desired data type to look for.
 * @param scheme The desired data scheme to look for.
 * @param data The full data string to match against.
 *
 * @return Returns either a valid match constant.
 */
int Skills::MatchData(const std::string &type, const std::string &scheme, Uri data)
{
    std::vector<std::string> types;
    for (auto it = types_.begin(); it != types_.end(); it++) {
        types.emplace_back(it->GetPattern());
    }
    std::vector<std::string> schemes = schemes_;

    int match = MATCH_CATEGORY_EMPTY;

    if (types.empty() && schemes.empty()) {
        return (type == std::string() ? (MATCH_CATEGORY_EMPTY + MATCH_ADJUSTMENT_NORMAL) : NO_MATCH_DATA);
    }

    if (!schemes.empty()) {
        auto it = std::find(schemes.begin(), schemes.end(), scheme);
        if (it != schemes.end()) {
            match = MATCH_CATEGORY_SCHEME;
        } else {
            return NO_MATCH_DATA;
        }

        std::vector<PatternsMatcher> schemeSpecificParts = schemeSpecificParts_;
        if (schemeSpecificParts.size() >= 0) {
            match = HasSchemeSpecificPart(data.GetSchemeSpecificPart()) ? MATCH_CATEGORY_SCHEME_SPECIFIC_PART
                                                                        : NO_MATCH_DATA;
        }
        if (match != MATCH_CATEGORY_SCHEME_SPECIFIC_PART) {
            std::vector<std::string> authorities = authorities_;
            if (authorities.size() >= 0) {
                bool authMatch = HasAuthority(data.GetAuthority());
                if (authMatch == true) {
                    std::vector<PatternsMatcher> paths = paths_;
                    if (paths.size() <= 0) {
                        match = authMatch;
                    } else if (HasPath(data.GetPath())) {
                        match = MATCH_CATEGORY_PATH;
                    } else {
                        return NO_MATCH_DATA;
                    }
                } else {
                    return NO_MATCH_DATA;
                }
            }
        }
        if (match == NO_MATCH_DATA) {
            return NO_MATCH_DATA;
        }
    } else {
        if (scheme != std::string() && scheme != "content" && scheme != "file") {
            return NO_MATCH_DATA;
        }
    }

    if (!types.empty()) {
        if (FindMimeType(type)) {
            match = MATCH_CATEGORY_TYPE;
        } else {
            return NO_MATCH_TYPE;
        }
    } else {
        if (type != std::string()) {
            return NO_MATCH_TYPE;
        }
    }

    return match + MATCH_ADJUSTMENT_NORMAL;
}

bool Skills::FindMimeType(const std::string &type)
{
    const int posNext = 1;
    const int posOffset = 2;
    std::vector<PatternsMatcher> types = types_;

    if (type == std::string()) {
        return false;
    }
    auto it = types.begin();
    for (; it != types.end(); it++) {
        if (it->GetPattern() == type) {
            break;
        }
    }
    if (it != types.end()) {
        return true;
    }

    int typeLength = type.length();
    if (typeLength == LENGTH_FOR_FINDMINETYPE && type == "*/*") {
        return !types.empty();
    }

    auto hasType =
        std::find_if(types.begin(), types.end(), [](const PatternsMatcher pm) { return pm.GetPattern() == "*"; });
    if (hasPartialTypes_ && hasType != types.end()) {
        return true;
    }

    auto typeIt = type.find(0, 1, '/');
    int slashpos = type.size() - typeIt;
    if (slashpos > 0) {
        std::string typeSubstr = type.substr(0, slashpos);

        auto hasType = std::find_if(types.begin(), types.end(), [&typeSubstr](const PatternsMatcher pm) {
            return pm.GetPattern() == typeSubstr;
        });
        if (hasPartialTypes_ && hasType != types.end()) {
            return true;
        }

        if (typeLength == slashpos + posOffset && type.at(slashpos + posNext) == '*') {
            int numTypes = types.size();
            for (int i = 0; i < numTypes; i++) {
                std::string value = types.at(i).GetPattern();
                if (RegionMatches(type, 0, value, 0, slashpos + posNext)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Skills::RegionMatches(const std::string &type, int toffset, const std::string &other, int ooffset, int len)
{
    int to = toffset;
    int po = ooffset;
    // Note: toffset, ooffset, or len might be near -1>>>1.
    if ((ooffset < 0) || (toffset < 0) || (toffset > (long)type.size() - len) ||
        (ooffset > (long)other.length() - len)) {
        return false;
    }
    while (len-- > 0) {
        if (type.at(to++) != other.at(po++)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Obtains the want params data.
 *
 * @return the WantParams object.
 */
const WantParams &Skills::GetWantParams() const
{
    return wantParams_;
}

/**
 * @brief Sets a WantParams object in this MatchingSkills object.
 *
 * @param wantParams Indicates the WantParams object.
 */
void Skills::SetWantParams(const WantParams &wantParams)
{
    wantParams_ = wantParams;
}

/**
 * @brief Marshals this Sequenceable object into a Parcel.
 *
 * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
 */
bool Skills::Marshalling(Parcel &parcel) const
{
    // entities​_
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, entities_);
    // actions_
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, actions_);
    // authorities_
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, authorities_);
    // schemes_
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, schemes_);
    // paths_
    if (paths_.empty()) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, VALUE_NULL);
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, paths_.size());
        for (auto path : paths_) {
            if (!parcel.WriteParcelable(&path)) {
                return false;
            }
        }
    }
    // schemeSpecificParts_
    if (schemeSpecificParts_.empty()) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, VALUE_NULL);
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, schemeSpecificParts_.size());
        for (auto schemeSpecificPart : schemeSpecificParts_) {
            if (!parcel.WriteParcelable(&schemeSpecificPart)) {
                return false;
            }
        }
    }
    // types_
    if (types_.empty()) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, VALUE_NULL);
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, types_.size());
        for (auto type : types_) {
            if (!parcel.WriteParcelable(&type)) {
                return false;
            }
        }
    }

    // parameters_
    if (wantParams_.GetParams().empty()) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, VALUE_NULL);
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteParcelable(&wantParams_)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Unmarshals this Sequenceable object from a Parcel.
 *
 * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
 */
Skills *Skills::Unmarshalling(Parcel &parcel)
{
    Skills *skills = new (std::nothrow) Skills();
    if (skills != nullptr) {
        if (!skills->ReadFromParcel(parcel)) {
            delete skills;
            skills = nullptr;
        }
    }
    return skills;
}

bool Skills::ReadFromParcel(Parcel &parcel)
{
    int32_t empty;
    int32_t size = 0;
    PatternsMatcher *pm = nullptr;

    // entities​_
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &entities_);
    // actions_
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &actions_);
    // authorities_
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &authorities_);
    // schemes_
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(StringVector, parcel, &schemes_);
    // paths_
    empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, size);
        for (int i = 0; i < size; i++) {
            pm = parcel.ReadParcelable<PatternsMatcher>();
            if (pm == nullptr) {
                return false;
            } else {
                paths_.emplace_back(*pm);
                delete pm;
                pm = nullptr;
            }
        }
    }

    // schemeSpecificParts_
    empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, size);
        for (int i = 0; i < size; i++) {
            pm = parcel.ReadParcelable<PatternsMatcher>();
            if (pm == nullptr) {
                return false;
            } else {
                schemeSpecificParts_.emplace_back(*pm);
                delete pm;
                pm = nullptr;
            }
        }
    }

    // types_
    empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, size);
        for (int i = 0; i < size; i++) {
            pm = parcel.ReadParcelable<PatternsMatcher>();
            if (pm == nullptr) {
                return false;
            } else {
                types_.emplace_back(*pm);
                delete pm;
                pm = nullptr;
            }
        }
    }

    // parameters_
    empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        auto params = parcel.ReadParcelable<WantParams>();
        if (params != nullptr) {
            wantParams_ = *params;
            delete params;
            params = nullptr;
        } else {
            return false;
        }
    }

    return true;
}

}  // namespace AAFwk
}  // namespace OHOS