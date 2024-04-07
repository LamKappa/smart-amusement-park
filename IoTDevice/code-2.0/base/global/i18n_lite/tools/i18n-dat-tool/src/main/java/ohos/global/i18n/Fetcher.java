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

import com.ibm.icu.text.DateFormatSymbols;
import com.ibm.icu.text.DateTimePatternGenerator;
import com.ibm.icu.text.DecimalFormatSymbols;
import com.ibm.icu.text.NumberFormat;
import com.ibm.icu.text.NumberingSystem;
import com.ibm.icu.util.ULocale;
import com.ibm.icu.text.DateFormat;
import com.ibm.icu.text.SimpleDateFormat;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.BufferedReader;
import java.io.FileReader;
import java.net.URISyntaxException;
import java.io.File;
import java.io.IOException;
import java.lang.reflect.Field;

/**
 * Fetcher is used to fetche a locale's specified data
 */
public class Fetcher implements Runnable {
    private static final Logger LOG = Logger.getLogger("Fetcher");
    private static int resourceCount = 0;
    private static HashMap<Integer, String> int2Str = new HashMap<>();
    private static HashMap<String, Integer> str2Int = new HashMap<>();
    private static String[] regularDatePatterns = { "MMMEd", "MMMd", "yMMMEEEd",
        "yMMMMEEEd", "EEEEyMd", "EEEyMd", "yMMMEEEEd", "yMMMMd", "Ed", "MEd" };
    private static String[] regularTimePatterns = { "hm", "Hm" };

    private static boolean sStatusOk = true;

    /** Used to store data related to a locale */
    public ArrayList<String> datas = new ArrayList<>();

    /** All non-repeated strings will be put into idMap */
    public Map<String, Integer> idMap;

    /** Indicate whether this Fetcher is included in the final generation process of i18n.dat file */
    public boolean included = true;

    /** LanguageTag related to the locale */
    public String languageTag;
    private String lan; // language
    private ReentrantLock lock; // Lock used to synchronize dump operation
    private ULocale locale;
    private DateFormatSymbols formatSymbols;
    private DateTimePatternGenerator patternGenerator;
    private int status = 0;

    static {
        resourceCount = loadResourceItems();
    }

    /**
     * Add all data items
     */
    private static int loadResourceItems() {
        int count = 0;
        try (BufferedReader fItems = new BufferedReader(new
            FileReader(new File(DataFetcher.class.getResource("/resource/resource_items.txt").toURI())))) {
            String line = "";
            while ((line = fItems.readLine()) != null) {
                String tag = line.trim();
                String[] items = tag.split(",");
                if (items == null || items.length != 2) {
                    LOG.log(Level.SEVERE, "line format is wrong in line " + (count + 1));
                    sStatusOk = false;
                    return count;
                }
                int index = Integer.valueOf(items[1].strip());
                str2Int.put(items[0], index);
                int2Str.put(index, items[1]);
                ++count;
            }
        } catch (URISyntaxException e) {
            sStatusOk = false;
            LOG.log(Level.SEVERE, "URISyntaxException");
        } catch (IOException e) {
            sStatusOk = false;
            LOG.log(Level.SEVERE, "add Fetcher IOEcxception");
        }
        return count;
    }

    /**
     * show whether resouce_items is loaded successfully
     * @return true if status is right, otherwise false
     */
    public static boolean isFetcherStatusOk() {
        return sStatusOk;
    }

    public static int getResourceCount() {
        return resourceCount;
    }

    /**
     * Methods to get int2Str
     *
     * @return Return int2Str
     */
    public static HashMap<Integer, String> getInt2Str() {
        return int2Str;
    }

    /**
     * Methods to get str2Int
     *
     * @return Return str2Int
     */
    public static HashMap<String, Integer> getStr2Int() {
        return str2Int;
    }

    public Fetcher(String tag, ReentrantLock lock, Map<String, Integer> idMap) {
        if (!Utils.isValidLanguageTag(tag)) {
            LOG.log(Level.SEVERE, "wrong languageTag " + tag);
            status = 1;
        }
        this.languageTag = tag;
        Objects.requireNonNull(lock);
        this.lock = lock;
        Objects.requireNonNull(idMap);
        this.idMap = idMap;
        this.lan = this.languageTag.split("-")[0];
        locale = ULocale.forLanguageTag(this.languageTag);
        formatSymbols = DateFormatSymbols.getInstance(locale);
        patternGenerator = DateTimePatternGenerator.getInstance(locale);
    }

    public boolean checkStatus() {
        return status == 0;
    }

    /**
     * Get all meta data defined in str2Int
     */
    public void getData() {
        getFormatAbbrMonthNames();
        getFormatAbbrDayNames();
        getTimePatterns();
        getDatePatterns();
        getAmPmMarkers();
        getPluralRules();
        getNumberFormat();
        getNumberDigits();
        getTimeSeparator();
        getDefaultHour();
        getStandAloneAbbrMonthNames();
        getStandAloneAbbrWeekDayNames();
        getFormatWideMonthNames();
        getHourMinuteSeconds();
        getFMSPattern();
        getFormatWideWeekDayNames();
        getStandAloneWideWeekDayNames();
        getStandAloneWideMonthNames();
    }

    /**
     * Dump all datas in this locale to idMap
     */
    public void dump() {
        try {
            lock.lock();
            int size = this.idMap.size();
            for (int i = 0; i < datas.size(); i++) {
                String data = datas.get(i);
                if (!idMap.containsKey(data)) {
                    idMap.put(data, size);
                    size++;
                }
            }
        } finally {
            lock.unlock();
        }
    }

    /**
     * Equals function to determin whether two objs are equal
     *
     * @param obj Object to be compared
     * @return Return true if obj is equals to this Fetcher object, otherwise false
     */
    public boolean equals(Object obj) {
        if (!(obj instanceof Fetcher)) {
            return false;
        }
        Fetcher fetcher = (Fetcher) obj;
        if (datas.size() != fetcher.datas.size()) {
            return false;
        }
        for (int i = 0; i < datas.size(); i++) {
            if (!datas.get(i).equals(fetcher.datas.get(i))) {
                return false;
            }
        }
        return true;
    }

    /**
     * Returns hashcode
     *
     * @return HashCode of Fetcher object
     */
    @Override
    public int hashCode() {
        return datas.hashCode() + languageTag.hashCode();
    }

    /**
     * Override methods in Runnable
     */
    public void run() {
        getData();
        dump();
    }

    private String convertNoAscii(String str) {
        return str;
    }

    // Get month names
    private void getMonthNames(int formatType, int lengthType) {
        StringBuilder sb = new StringBuilder();
        String[] months = formatSymbols.getMonths(formatType, lengthType);
        for (int i = 0; i < months.length; i++) {
            sb.append(months[i]);
            if (i != months.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        datas.add(sb.toString());
    }

    // Get weekday names
    private void getWeekDayNames(int formatType, int lengthType) {
        StringBuilder sb = new StringBuilder();
        String[] weekdays = formatSymbols.getWeekdays(formatType, lengthType);
        String[] adjustWeekdays = new String[(weekdays.length - 1)];
        for (int i = 0; i < adjustWeekdays.length; i++) {
            adjustWeekdays[i] = weekdays[i + 1];
        }
        for (int i = 0; i < adjustWeekdays.length; i++) {
            sb.append(adjustWeekdays[i]);
            if (i != adjustWeekdays.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        this.datas.add(sb.toString());
    }

    private void getHourMinuteSeconds() {
        datas.add(patternGenerator.getBestPattern("hms") + "_" + patternGenerator.getBestPattern("Hms"));
    }


    // use to get patterns from skeletons
    private void getPatterns(String[] skeletons) {
        StringBuilder sb = new StringBuilder();
        String[] outPatterns = new String[skeletons.length];
        for (int i = 0; i < skeletons.length; ++i) {
            switch (skeletons[i]) {
                case "FULL":
                case "MEDIUM":
                case "SHORT": {
                    outPatterns[i] = getFMSPattern(skeletons[i]);
                    break;
                }
                default: {
                    // special process for en-US's pattern Ed
                    if ("en-US".equals(languageTag) && ("Ed".equals(skeletons[i]))) {
                        outPatterns[i] = "EEE d";
                        continue;
                    }
                    outPatterns[i] = patternGenerator.getBestPattern(skeletons[i]);
                }
            }
        }
        for (int i = 0; i < skeletons.length; i++) {
            sb.append(outPatterns[i]);
            if (i != outPatterns.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        datas.add(sb.toString());
    }

    // Get FULL-MEDIUM_SHORT pattern
    private String getFMSPattern(String skeleton) { 
        DateFormat formatter = null;
        try {
            Field patternField = DateFormat.class.getField(skeleton);
            int patternIndex = patternField.getInt(null);
            formatter = DateFormat.getDateInstance(patternIndex, locale);
        } catch (NoSuchFieldException | IllegalArgumentException | IllegalAccessException e ) {
            LOG.log(Level.SEVERE, "cannot get field " + skeleton);
        }
        if (formatter instanceof SimpleDateFormat) {
            return ((SimpleDateFormat)formatter).toPattern();
        } else {
            LOG.log(Level.SEVERE, "wrong type in getFMSPattern");
            return "";
        }
    }

    private void getFMSPattern() {
        StringBuilder sb = new StringBuilder();
        DateFormat dfull = DateFormat.getDateInstance(DateFormat.FULL, locale);
        if (dfull instanceof com.ibm.icu.text.SimpleDateFormat) {
            sb.append(((com.ibm.icu.text.SimpleDateFormat)dfull).toPattern());
            sb.append(FileConfig.SEP);
        }
        DateFormat dmedium = DateFormat.getDateInstance(DateFormat.MEDIUM, locale);
        if (dmedium instanceof com.ibm.icu.text.SimpleDateFormat) {
            sb.append(((com.ibm.icu.text.SimpleDateFormat)dmedium).toPattern());
            sb.append(FileConfig.SEP);
        }
        DateFormat dshort = DateFormat.getDateInstance(DateFormat.SHORT, locale);
        if (dshort instanceof com.ibm.icu.text.SimpleDateFormat) {
            sb.append(((com.ibm.icu.text.SimpleDateFormat)dshort).toPattern());
        }
        datas.add(sb.toString());
    }

    // 0. get format abbreviated month names
    private void getFormatAbbrMonthNames() {
        getMonthNames(DateFormatSymbols.FORMAT, DateFormatSymbols.ABBREVIATED);
    }

    // 1. get format abbreviated day names
    private void getFormatAbbrDayNames() {
        getWeekDayNames(DateFormatSymbols.FORMAT, DateFormatSymbols.ABBREVIATED);
    }

    // 2. get time pattern
    private void getTimePatterns() {
        getPatterns(regularTimePatterns);
    }

    // 3. get date patterns() 
    private void getDatePatterns() {
        getPatterns(regularDatePatterns);
    }

    // 4. get am pm markser
    private void getAmPmMarkers() {
        StringBuilder sb = new StringBuilder();
        String[] amPmStrings = formatSymbols.getAmPmStrings();
        for (int i = 0; i < amPmStrings.length; ++i) {
            sb.append(amPmStrings[i]);
            if (i != amPmStrings.length - 1) {
                sb.append(FileConfig.SEP);
            }
        }
        this.datas.add(sb.toString());
    }

    // 5. get plural data
    private void getPluralRules() {
        String str = PluralFetcher.getInstance().get(this.lan);
        if (str == null) {
            str = "";
        }
        this.datas.add(str);
    }

    // 6. get number format data
    @SuppressWarnings("Deprecation")
    private void getNumberFormat() {
        String pattern = NumberFormat.getPatternForStyle(locale, NumberFormat.NUMBERSTYLE);
        String percentPattern = NumberFormat.getPatternForStyle(locale, NumberFormat.PERCENTSTYLE);
        // NumberingSystem numberSystem = NumberingSystem.getInstance(locale);
        DecimalFormatSymbols decimalFormatSymbols = new DecimalFormatSymbols(locale);
        String percent = decimalFormatSymbols.getPercentString();
        String  groupingSeparator = decimalFormatSymbols.getGroupingSeparatorString();
        String decimalSeparator = decimalFormatSymbols.getDecimalSeparatorString();
        StringBuilder sb = new StringBuilder();
        sb.append(pattern);
        sb.append(FileConfig.SEP);
        sb.append(percentPattern);
        sb.append(FileConfig.SEP);
        sb.append(convertNoAscii(decimalSeparator));
        sb.append(FileConfig.SEP);
        sb.append(convertNoAscii(groupingSeparator));
        sb.append(FileConfig.SEP);
        sb.append(convertNoAscii(percent));
        datas.add(sb.toString());
    }

    // 7. get number digits
    private void getNumberDigits() {
        NumberingSystem numberSystem = NumberingSystem.getInstance(locale);
        String description = numberSystem.getDescription();
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < description.length(); i++) {
            sb.append(String.valueOf(description.charAt(i)));
            if (i != description.length() - 1) {
                sb.append(FileConfig.NUMBER_SEP);
            }
        }
        datas.add(sb.toString());
    }

    // 8. get time separtor
    @SuppressWarnings("Deprecation")
    private void getTimeSeparator() {
        datas.add(formatSymbols.getTimeSeparatorString());
    }

    // 9. get default hour
    private void getDefaultHour() {
        DateFormat tempFormat = DateFormat
            .getTimeInstance(DateFormat.SHORT, ULocale.forLanguageTag(languageTag));
        SimpleDateFormat timeInstance = null;
        if (tempFormat instanceof SimpleDateFormat) {
            timeInstance = (SimpleDateFormat) tempFormat;
        }
        String shortDateTimePattern = (timeInstance == null) ? "" : timeInstance.toPattern();
        if (shortDateTimePattern.contains("H")) {
            datas.add("H");
        } else {
            datas.add("h");
        }
    }

    // 10.get standalone abbreviated month
    private void getStandAloneAbbrMonthNames() {
        getMonthNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.ABBREVIATED);
    }

    // 11. get standalone abbreviated weekday
    private void getStandAloneAbbrWeekDayNames() {
        getWeekDayNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.ABBREVIATED);
    }

    // 12. get format wide month
    private void getFormatWideMonthNames() {
        getMonthNames(DateFormatSymbols.FORMAT, DateFormatSymbols.WIDE);
    }

    // 13. get format wide days
    private void getFormatWideWeekDayNames() {
        getWeekDayNames(DateFormatSymbols.FORMAT, DateFormatSymbols.WIDE);
    }

    // 14. get standalone wide days
    private void getStandAloneWideWeekDayNames() {
        getWeekDayNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.WIDE);
    }

    // 15. get standalone wide month
    private void getStandAloneWideMonthNames() {
        getMonthNames(DateFormatSymbols.STANDALONE, DateFormatSymbols.WIDE);
    }
}
