#pragma once
#include "../search/search.h"

class Replace : public Search {
public:
    explicit Replace(CodeEditor* editor = nullptr);

    bool replaceNext();          // Replace and find next occurrence
    bool replacePrevious();      // Replace and find previous occurrence
    void replaceAll();           // Replace all occurrences in the document
    void selectAll();
};
