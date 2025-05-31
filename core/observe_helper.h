#pragma once

#include "core/journal/journal.h"
#include "core/journal/reader.h"

namespace btra {

class ObserveHelper {
public:
    ObserveHelper();
    void add_customer(journal::ReaderUPtr &reader);

    bool data_available();

private:
    journal::Reader *reader_{nullptr};
#ifndef HP
    journal::JourObserver jour_observer_;
#endif
};

}