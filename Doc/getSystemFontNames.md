```c++
    auto fontMgr = SkFontMgr::RefDefault();
    int count = fontMgr->countFamilies();
    for (int i = 0; i < count; ++i) {
        SkString familyName;
        fontMgr->getFamilyName(i, &familyName);
        familyName += "\n";
        SkDebugf(familyName.c_str());
    }
```