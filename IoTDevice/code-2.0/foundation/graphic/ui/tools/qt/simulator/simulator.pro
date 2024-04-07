TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    third_party/freetype \
    third_party/harfbuzz \
    third_party/icu \
    third_party/libjpeg \
    third_party/zlib \
    third_party/libpng \
    third_party/qrcodegen \
    libui \
    test \
    uitest/UITest.pro
