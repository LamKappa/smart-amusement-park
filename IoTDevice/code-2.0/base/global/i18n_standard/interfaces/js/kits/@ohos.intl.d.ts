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

declare namespace intl {
/**
 * Provides APIs for obtaining locale information.
 *
 * @since 6
 */
export class Locale {
    /**
     * A constructor used to create a Locale object.
     *
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region.
     * @since 6
     */
   constructor(locale?: string);

    /**
     * Indicates the language of the locale.
     *
     * @since 6
     */
    language: string

    /**
     * Indicates the script of the locale.
     *
     * @since 6
     */
    script: string

    /**
     * Indicates the region of the locale.
     *
     * @since 6
     */
    region: string

    /**
     * Indicates the basic locale information, which is returned as a substring of
     * a complete locale string.
     *
     * @since 6
     */
    baseName: string
}

/**
 * Provides the API for formatting date strings.
 *
 * @since 6
 */
export class DateTimeFormat {
    /**
     * A constructor used to create a DateTimeFormat object.
     *
     * @param locale Indicates a character string containing the locale information, including
     *               the language and optionally the script and region, for the DateTimeFormat object.
     * @since 6
     */
    constructor(locale?: string);

    /**
     * Obtains the formatted date strings.
     *
     * @param date Indicates the Date object to be formatted.
     * @return Returns a date string formatted based on the specified locale.
     * @since 6
     */
    format(date: Date): string;
}
}
export default intl;