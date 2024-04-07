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

#ifndef OHOS_AAFWK_SKILLS_H
#define OHOS_AAFWK_SKILLS_H

#include <vector>
#include <string>
#include "want.h"
#include "want_params.h"
#include "parcel.h"
#include "parcel_macro.h"
#include "match_type.h"
#include "patterns_matcher.h"
#include "uri.h"

namespace OHOS {
namespace AAFwk {
class Skills final : public Parcelable {
public:
    /**
     * @brief Default constructor used to create a Skills instance.
     *
     */
    Skills();

    /**
     * @brief A parameterized constructor used to create a Skills instance.
     *
     * @param skills Indicates skills used to create a Skills instance.
     */
    Skills(const Skills &skills);
    ~Skills();

    /**
     * @brief Obtains the list of entities.
     *
     * @return vector of Entities.
     */
    std::vector<std::string> GetEntities() const;

    /**
     * @brief Obtains the specified entity.
     *
     * @param entity Id of the specified entity.
     */
    std::string GetEntity(int index) const;

    /**
     * @brief Adds an entity to this Skills object.
     *
     * @param entity Indicates the entity to add.
     */
    void AddEntity(const std::string &entity);

    /**
     * @brief Checks whether the specified entity is exist.
     *
     * @param entity Name of the specified entity.
     */
    bool HasEntity(const std::string &entity);

    /**
     * @brief Remove the specified entity.
     *
     * @param entity Name of the specified entity.
     */
    void RemoveEntity(const std::string &entity);

    /**
     * @brief Obtains the count of entities.
     *
     */
    int CountEntities() const;

    /**
     * @brief Obtains the specified action.
     *
     * @param actionId Id of the specified action.
     */
    std::string GetAction(int index) const;

    /**
     * @brief Adds an action to this Skills object.
     *
     * @param action Indicates the action to add.
     */
    void AddAction(const std::string &action);

    /**
     * @brief Checks whether the specified action is exist.
     *
     * @param action Name of the specified action.
     */
    bool HasAction(const std::string &action);

    /**
     * @brief Remove the specified action.
     *
     * @param action Name of the specified action.
     */
    void RemoveAction(const std::string &action);

    /**
     * @brief Obtains the count of actions.
     *
     */
    int CountActions() const;

    /**
     * @brief Obtains the iterator of Actions.
     *
     * @return iterator of Actions.
     */
    std::vector<std::string>::iterator ActionsIterator();

    /**
     * @brief Obtains the iterator of Authorities.
     *
     * @return iterator of Authorities.
     */
    std::vector<std::string>::iterator AuthoritiesIterator();

    /**
     * @brief Obtains the iterator of Entities.
     *
     * @return iterator of Entities.
     */
    std::vector<std::string>::iterator EntitiesIterator();

    /**
     * @brief Obtains the iterator of Paths.
     *
     * @return iterator of Paths.
     */
    std::vector<std::string>::iterator PathsIterator();

    /**
     * @brief Obtains the iterator of Schemes parts.
     *
     * @return iterator of Schemes parts.
     */
    std::vector<std::string>::iterator SchemeSpecificPartsIterator();

    /**
     * @brief Obtains the iterator of Schemes.
     *
     * @return iterator of Schemes.
     */
    std::vector<std::string>::iterator SchemesIterator();

    /**
     * @brief Obtains the iterator of Types.
     *
     * @return iterator of Types.
     */
    std::vector<std::string>::iterator TypesIterator();

    /**
     * @brief Obtains the specified authority.
     *
     * @param authorityId Id of the specified authority.
     */
    std::string GetAuthority(int index) const;

    /**
     * @brief Adds an authority to this Skills object.
     *
     * @param authority Indicates the authority to add.
     */
    void AddAuthority(const std::string &authority);

    /**
     * @brief Checks whether the specified authority is exist.
     *
     * @param authority Name of the specified authority.
     */
    bool HasAuthority(const std::string &authority);

    /**
     * @brief Remove the specified authority.
     *
     * @param authority Name of the specified authority.
     */
    void RemoveAuthority(const std::string &authority);

    /**
     * @brief Obtains the count of authorities.
     *
     */
    int CountAuthorities() const;

    /**
     * @brief Obtains the specified path.
     *
     * @param pathId Id of the specified path.
     */
    std::string GetPath(int index) const;

    /**
     * @brief Adds a path to this Skills object.
     *
     * @param path Indicates the path to add.
     */
    void AddPath(const std::string &path);

    /**
     * @brief Adds a path to this Skills object.
     *
     * @param path Indicates the path to add.
     */
    void AddPath(const PatternsMatcher &patternsMatcher);

    /**
     * @brief Adds a path to this Skills object.
     *
     * @param path Indicates the path to add.
     * @param matchType the specified match type.
     */
    void AddPath(const std::string &path, const MatchType &matchType);

    /**
     * @brief Checks whether the specified path is exist.
     *
     * @param path Name of the specified path.
     */
    bool HasPath(const std::string &path);

    /**
     * @brief Remove the specified path.
     *
     * @param path Name of the specified path.
     */
    void RemovePath(const std::string &path);

    /**
     * @brief Remove the specified path.
     *
     * @param path The path to be added.
     */
    void RemovePath(const PatternsMatcher &patternsMatcher);

    /**
     * @brief Remove the specified path.
     *
     * @param path Name of the specified path.
     * @param matchType the specified match type.
     */
    void RemovePath(const std::string &path, const MatchType &matchType);

    /**
     * @brief Obtains the count of paths.
     *
     */
    int CountPaths() const;

    /**
     * @brief Obtains the specified scheme.
     *
     * @param schemeId Id of the specified scheme.
     */
    std::string GetScheme(int index) const;

    /**
     * @brief Adds an scheme to this Skills object.
     *
     * @param scheme Indicates the scheme to add.
     */
    void AddScheme(const std::string &scheme);

    /**
     * @brief Checks whether the specified scheme is exist.
     *
     * @param scheme Name of the specified scheme.
     */
    bool HasScheme(const std::string &scheme);

    /**
     * @brief Remove the specified scheme.
     *
     * @param scheme Name of the specified scheme.
     */
    void RemoveScheme(const std::string &scheme);

    /**
     * @brief Obtains the count of schemes.
     *
     */
    int CountSchemes() const;

    /**
     * @brief Obtains the specified scheme part.
     *
     * @param schemeId Id of the specified scheme part.
     */
    std::string GetSchemeSpecificPart(int index) const;

    /**
     * @brief Adds an scheme to this Skills object.
     *
     * @param scheme Indicates the scheme to add.
     */
    void AddSchemeSpecificPart(const std::string &schemeSpecificPart);

    /**
     * @brief Checks whether the specified scheme part is exist.
     *
     * @param scheme Name of the specified scheme part.
     */
    bool HasSchemeSpecificPart(const std::string &schemeSpecificPart);

    /**
     * @brief Remove the specified scheme part.
     *
     * @param scheme Name of the specified scheme part.
     */
    void RemoveSchemeSpecificPart(const std::string &schemeSpecificPart);

    /**
     * @brief Obtains the count of scheme parts.
     *
     */
    int CountSchemeSpecificParts() const;

    /**
     * @brief Obtains the specified type.
     *
     * @param typeId Id of the specified type.
     */
    std::string GetType(int index) const;

    /**
     * @brief Adds a type to this Skills object.
     *
     * @param type Indicates the type to add.
     */
    void AddType(const std::string &type);

    /**
     * @brief Adds a type to this Skills object.
     *
     * @param type Indicates the type to add.
     */
    void AddType(const PatternsMatcher &patternsMatcher);

    /**
     * @brief Adds a type to this Skills object.
     *
     * @param type Indicates the type to add.
     * @param matchType the specified match type.
     */
    void AddType(const std::string &type, const MatchType &matchType);

    /**
     * @brief Checks whether the specified type is exist.
     *
     * @param type Name of the specified type.
     */
    bool HasType(const std::string &type);

    /**
     * @brief Remove the specified type.
     *
     * @param type Name of the specified type.
     */
    void RemoveType(const std::string &type);

    /**
     * @brief Remove the specified scheme type.
     *
     * @param type The type to be added.
     */
    void RemoveType(const PatternsMatcher &patternsMatcher);

    /**
     * @brief Remove the specified scheme type.
     *
     * @param type Name of the specified type.
     * @param matchType the specified match type.
     */
    void RemoveType(const std::string &type, const MatchType &matchType);

    /**
     * @brief Obtains the count of types.
     *
     */
    int CountTypes() const;

    /**
     * @brief Match this skill against a Want's data.
     *
     * @param want The desired want data to match for.
     */
    bool Match(const Want &want);

    /**
     * @brief Obtains the want params data.
     *
     * @return the WantParams object.
     */
    const WantParams &GetWantParams() const;

    /**
     * @brief Sets a WantParams object in this MatchingSkills object.
     *
     * @param wantParams Indicates the WantParams object.
     */
    void SetWantParams(const WantParams &wantParams);

    /**
     * @brief Marshals this Sequenceable object into a Parcel.
     *
     * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
     */
    bool Marshalling(Parcel &parcel) const;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     */
    static Skills *Unmarshalling(Parcel &parcel);

private:
    static const int NO_MATCH_TYPE = -1;
    static const int NO_MATCH_DATA = -2;
    static const int NO_MATCH_ACTION = -3;
    static const int NO_MATCH_CATEGORY = -4;
    static const int MATCH_CATEGORY_EMPTY = 0x0100000;
    static const int MATCH_CATEGORY_SCHEME = 0x0200000;
    static const int MATCH_CATEGORY_PATH = 0x0500000;
    static const int MATCH_CATEGORY_SCHEME_SPECIFIC_PART = 0x0580000;
    static const int MATCH_CATEGORY_TYPE = 0x0600000;
    static const int MATCH_ADJUSTMENT_NORMAL = 0x8000;

    std::vector<std::string> entities_;
    std::vector<std::string> actions_;
    std::vector<std::string> authorities_;
    std::vector<std::string> schemes_;

    std::vector<PatternsMatcher> paths_;
    std::vector<PatternsMatcher> schemeSpecificParts_;
    std::vector<PatternsMatcher> types_;

    WantParams wantParams_;
    bool hasPartialTypes_ = false;

    // no object in parcel
    static constexpr int VALUE_NULL = -1;
    // object exist in parcel
    static constexpr int VALUE_OBJECT = 1;

private:
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Match this skills against a Want's action.  If the skills does not
     * specify any actions, the match will always fail.
     *
     * @param action The desired action to look for.
     *
     * @return True if the action is listed in the skills.
     */
    bool MatchAction(const std::string &action);

    /**
     * @brief Match this skills against a Want's data (type, scheme and path).
     *
     * @param type The desired data type to look for.
     * @param scheme The desired data scheme to look for.
     * @param data The full data string to match against.
     *
     * @return Returns either a valid match constant.
     */
    int MatchData(const std::string &type, const std::string &scheme, Uri data);

    bool FindMimeType(const std::string &type);

    bool RegionMatches(const std::string &type, int toffset, const std::string &other, int ooffset, int len);

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
    std::string MatchEntities(const std::vector<std::string> &entities);
};

}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_AAFWK_SKILLS_H
