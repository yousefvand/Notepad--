#pragma once

#include "../search/search.h"

class Replace : public Search {
public:
    using Search::Search;

    bool replaceNext();
    bool replacePrevious();
    void replaceAll();
};
