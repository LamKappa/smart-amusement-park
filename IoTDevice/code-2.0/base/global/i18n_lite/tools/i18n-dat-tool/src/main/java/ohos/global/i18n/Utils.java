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

package ohos.global.i18n;

import com.ibm.icu.util.ULocale;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

public class Utils {
    private Utils() {}

    /**
     * Get a locale's fallback, locale is specified with languageTag
     *
     * @param languageTag Use this languageTag to compute the fallback
     * @return Fallback languageTag
     */
    public static String getFallback(String languageTag) {
        if ("".equals(languageTag)) {
            return "en-US";
        }
        String[] split = languageTag.split("-");
        if ("en-US".equals(languageTag) || split.length == 1) {
            return "en-US";
        }
        if (split.length != 2) {
            return split[0] + "-" + split[1];
        }
        if ((split[1].length() != 4) && (!"en".equals(split[0]))) {
            return split[0];
        }
        return "en-US";
    }

    // check whether a languageTag is valid.
    public static boolean isValidLanguageTag(String languageTag) {
        if (null == languageTag) {
            return false;
        }
        String[] items = languageTag.split("-");
        switch (items.length) {
            case 1: {
                return checkLanguage(items[0]);
            }
            case 2: {
                if (!checkLanguage(items[0])) {
                    return false;
                }
                // script
                if (items[1].length() == 4) {
                    if (checkScript(items[1])) {
                        return true;
                    }
                } else if(items[1].length() == 2) {
                    if (checkRegion(items[1])) {
                        return true;
                    }
                }
                return false;
            }
            case 3: {
                return checkLanguage(items[0]) && checkScript(items[1]) && checkRegion(items[2]);
            }
            default: {
                return false;
            }
        }
    }

    private static boolean checkLanguage(String lan) {
        if (null == lan) {
            return false;
        }
        int length = lan.length();
        if (length > 3 || length < 2) {
            return false;
        }
        for (int i = 0; i < length; ++i) {
            if ((int) lan.charAt(i) > 255) {
                return false;
            }
        }
        return true;
    }

    // script is a 4 character string, started with a uppercase letter
    private static boolean checkScript(String script) {
        int length = script.length();
        if (length != 4) {
            return false;
        }
        for (int i = 0; i < length; ++i) {
            if (i == 0 ) {
                if (!Character.isUpperCase(script.charAt(0))) {
                    return false;
                }
            } else {
                char cur = script.charAt(i);
                if ('a' > cur || 'z' < cur) {
                    return false;
                }
            }
        }
        return true;
    }

    private static boolean checkRegion(String region) {
        int length = region.length();
        if (length != 2) {
            return false;
        }
        for (int i = 0; i < length; ++i) {
            char cur = region.charAt(i);
            if ('A' > cur || 'Z' < cur) {
                return false;
            }
        }
        return true;
    }

    /**
     * Write i18n.dat's Header to DataOutputStream
     * 
     * @param out
     * @param hashCode reserved for future use
     * @param localesCount valid locales in total
     * @param metaCount all metaData in total
     * @throws IOException
     */
    public static void writeHeader(DataOutputStream out, int hashCode, int localesCount,
        int metaCount) throws IOException {
        out.writeInt(hashCode); // reserved hashcode
        out.writeByte(FileConfig.FILE_VERSION);
        out.writeByte(0); // reserved
        out.writeChar(0);
        out.writeChar(0); // reserved
        out.writeChar(FileConfig.HEADER_SIZE + 8 * localesCount);
        out.writeChar(localesCount);
        out.writeChar(FileConfig.HEADER_SIZE + 8 * localesCount  + metaCount * 6);
        out.flush();
    }

    /**
     * Get mask of a locale
     *
     * @param locale Indicates the specified locale related to the output mask
     * @param maskOut The value of mask will be stored in the first element of maskOut
     * @return The text representation of mask in hex format
     * @throws UnsupportedEncodingException if getBytes function failed
     */
    public static String getMask(ULocale locale, long[] maskOut) throws UnsupportedEncodingException {
        long mask = 0;
        byte[] langs;
        // Deal with "fil" and "mai" these 3-leters language
        if ("fil".equals(locale.getLanguage())) {
            langs = "tl".getBytes("utf-8");
        } else if ("mai".equals(locale.getLanguage())) {
            langs = "md".getBytes("utf-8");
        } else {
            langs = locale.getLanguage().getBytes("utf-8");
        }
        mask = mask | ((long)(langs[0] - 48)) << 25 | ((long)(langs[1] - 48)) << 18;
        int temp = 0;
        if ("Latn".equals(locale.getScript())) {
            temp = 1;
        } else if ("Hans".equals(locale.getScript())) {
            temp = 2;
        } else if ("Hant".equals(locale.getScript())) {
            temp = 3;
        } else if ("Qaag".equals(locale.getScript())) {
            temp = 4;
        } else if ("Cyrl".equals(locale.getScript())) {
            temp = 5;
        } else if ("Deva".equals(locale.getScript())) {
            temp = 6;
        } else {
            temp = "Guru".equals(locale.getScript()) ? 7 : 0;
        }
        mask = mask | ((long)temp << 14);
        if (locale.getCountry() != null && locale.getCountry().length() == 2) {
            byte[] ret = locale.getCountry().getBytes("utf-8");
            mask = mask | ((long) (ret[0] - 48) << 7) | ((long)(ret[1] - 48));
        }
        maskOut[0] = mask;
        String ret = "0x" + Long.toHexString(mask);
        return ret;
    }
}
