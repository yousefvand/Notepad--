#include <QCoreApplication>
#include "replace.h"

bool Replace::replaceNext() {
    m_searchOptions->role = Role::ReplaceNext;
    setSearchOptions(*m_searchOptions);
    return search();
}

bool Replace::replacePrevious() {
    m_searchOptions->role = Role::ReplacePrevious;
    setSearchOptions(*m_searchOptions);
    return search();
}

void Replace::replaceAll() {
    m_searchOptions->role = Role::ReplaceNext; // Using ReplaceNext to replace all until fixing replaceAll().
    setSearchOptions(*m_searchOptions);
    // replaceAll(); // FIXME: replaceAll causes segmentation fault.
    while(search()) QCoreApplication::processEvents();
}
