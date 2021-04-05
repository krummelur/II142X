//
// Created by Magnus on 4/5/21.
//
#include "../src/queue_impl.h"
#include "gtest/gtest.h"
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <vector>
#include <random>

template <>
struct rc::Arbitrary<IntegerLISTEntry> {
    static Gen<IntegerLISTEntry> arbitrary() {
        return gen::build<IntegerLISTEntry>(
                gen::set(&IntegerLISTEntry::data, gen::arbitrary<int>()));
    }
};

void createList(myLISTHead &head,std::vector<IntegerLISTEntry> &ents) {
    if(ents.empty())
        return;
    LIST_INIT_impl(&head);
    LIST_INSERT_HEAD_impl(&head, &ents.at(0));
    for (std::size_t i = 1; i < ents.size(); i++)
        LIST_INSERT_AFTER_impl(&ents.at(i-1), &ents.at(i));
}

RC_GTEST_PROP(LIST,
        concatenatingListsEmptiesSecondList,
(std::vector<IntegerLISTEntry> a,std::vector<IntegerLISTEntry> b)){
    myLISTHead headA{};
    createList(headA, a);
    myLISTHead headB{};
    createList(headB, b);
    LIST_CONCAT_impl(&headA, &headB);
    RC_ASSERT(nullptr == headB.lh_first);
}

RC_GTEST_PROP(LIST,
              concatenatingMakesLinkFromListAlastToHeadBfirstAndViceVersa,
              (std::vector<IntegerLISTEntry> a,std::vector<IntegerLISTEntry> b)){

    RC_SUCCEED_IF(a.empty() || b.empty());
    myLISTHead headA{};
    createList(headA, a);
    myLISTHead headB{};
    createList(headB, b);
    LIST_CONCAT_impl(&headA, &headB);
    RC_ASSERT(a.at(a.size()-1).links.le_next == &b.at(0));
    RC_ASSERT((b.at(0).links.le_prev) == &a.at(a.size()-1).links.le_next);
}

RC_GTEST_PROP(LIST,
              concatenatingListsYieldsCorrectSizeOnList1,
              (std::vector<IntegerLISTEntry> a,std::vector<IntegerLISTEntry> b)){

    myLISTHead headA{};
    createList(headA, a);
    myLISTHead headB{};
    createList(headB, b);
    LIST_CONCAT_impl(&headA, &headB);

    IntegerLISTEntry *entry;
    unsigned int actualSize = 0;
    unsigned int expectedSize = a.size() + b.size();

    LIST_FOREACH(entry, &headA, links) {
        actualSize++;
    }

    RC_ASSERT(expectedSize == actualSize);
    RC_ASSERT(headB.lh_first == nullptr);
}

RC_GTEST_PROP(LIST,
              concatenatingWithHead1EmptyPutsHeadAFirstOnHeadB,
              (std::vector<IntegerLISTEntry> b)){
    RC_SUCCEED_IF(b.empty());
    myLISTHead headA{};
    LIST_INIT_impl(&headA);
    myLISTHead headB{};
    createList(headB, b);
    IntegerLISTEntry* headBFirst = &b.at(0) ;
    LIST_CONCAT_impl(&headA, &headB);
    RC_ASSERT(headBFirst == headA.lh_first);
}

RC_GTEST_PROP(LIST,
              concatenatingWithHeadBEmptyPutsHeadAFirstOnHeadA,
              (std::vector<IntegerLISTEntry> a)){
    RC_SUCCEED_IF(a.empty());
    myLISTHead headA{};
    myLISTHead headB{};
    createList(headA, a);
    IntegerLISTEntry* headAFirst = &a.at(0) ;
    LIST_CONCAT_impl(&headA, &headB);
    RC_ASSERT(headAFirst == headA.lh_first);
}

RC_GTEST_PROP(LIST,
              concatenatingListsPreservesOrder,
              (std::vector<IntegerLISTEntry> a,std::vector<IntegerLISTEntry> b)){

    myLISTHead headA{};
    createList(headA, a);
    myLISTHead headB{};
    createList(headB, b);
    LIST_CONCAT_impl(&headA, &headB);

    IntegerLISTEntry *entry;
    int temp = 0;
    LIST_FOREACH(entry, &headA, links) {
        const IntegerLISTEntry &shouldPointTo = temp < a.size() ? a.at(temp) : b.at(temp - a.size());
        RC_ASSERT(&shouldPointTo == entry);
        temp++;
    }
}

RC_GTEST_PROP(LIST,
              nonemptyListIsAlwaysEmptyAfterNonRemovingOperation,
              (std::vector<IntegerLISTEntry> a,std::vector<IntegerLISTEntry> b)){
    ///TODO: MODEL PROPERTY TEST.
}

TEST(LIST, emptyListAlwaysReturnsNullAsFirst) {
    myLISTHead head{};
    LIST_INIT_impl(&head);
    EXPECT_EQ (nullptr, head.lh_first);
    EXPECT_EQ (true, LIST_EMPTY_impl(&head));
}

RC_GTEST_PROP(LIST,
              foreachSummingaShouldYieldCorectSum,
              (std::vector<IntegerLISTEntry> a, std::vector<IntegerLISTEntry> b)){
    myLISTHead headA{};
    createList(headA, a);
    myLISTHead headB{};
    createList(headB, b);
    unsigned long expectedSum = 0;
    unsigned long actualSum = 0;
    for (auto &el : a) {
        expectedSum += (unsigned)abs(el.data);
    }
    for (auto &el : b) {
        expectedSum += (unsigned)abs(el.data);
    }
    IntegerLISTEntry* var;
    LIST_FOREACH(var, &headA, links) {
        actualSum+=(unsigned)abs(var->data);
    }
    LIST_FOREACH(var, &headB, links) {
        actualSum+=(unsigned)abs(var->data);
    }
    RC_ASSERT(actualSum == expectedSum);
}

RC_GTEST_PROP(LIST,
              addingElementsInForEachDoublesListSize,
              (std::vector<IntegerLISTEntry> a)){
    myLISTHead headA{};
    createList(headA, a);
    IntegerLISTEntry* var;
    std::vector<IntegerLISTEntry> newEntries;
    newEntries.reserve(a.size());
    LIST_FOREACH(var, &headA, links) {
        if(var != &a.at(0)) {
            IntegerLISTEntry entry = *rc::gen::arbitrary<IntegerLISTEntry>();
            newEntries.push_back(entry);
            IntegerLISTEntry* toInsert = &newEntries.back();
            LIST_INSERT_HEAD_impl(&headA, toInsert);
        }
    }
    unsigned expectedSize = std::max((int)(a.size()*2-1), 0);
    unsigned int actualSize = 0;
    LIST_FOREACH(var, &headA, links) {
        actualSize++;
    }

    RC_ASSERT(actualSize == expectedSize);
}

RC_GTEST_PROP(LIST,
              foreachSafeShouldHandleRemovalDuringTraversal,
              (std::vector<IntegerLISTEntry> a)){
    myLISTHead headA{};
    createList(headA, a);
    IntegerLISTEntry* var;
    IntegerLISTEntry* tvar;
    LIST_FOREACH_SAFE(var, &headA, links, tvar) {
        LIST_REMOVE_impl(var);
    }
    unsigned int actualSize = 0;
    unsigned int expectedSize = 0;
    LIST_FOREACH(var, &headA, links) {
        actualSize++;
    }
    RC_ASSERT(actualSize == expectedSize);
}

RC_GTEST_PROP(LIST,
              insertingAfterIntoNonEmptyListShouldIncreaseSizeBy1,
              (std::vector<IntegerLISTEntry> a)){
    RC_SUCCEED_IF(a.empty());
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, a.size()-1);
    myLISTHead headA{};
    createList(headA, a);

    IntegerLISTEntry* var;
    unsigned int expectedSize = 1;
    LIST_FOREACH(var, &headA, links) {
        expectedSize++;
    }

    auto insertAfterThis = &a.at(distribution(generator));
    auto toInsert = *rc::gen::arbitrary<IntegerLISTEntry>();
    LIST_INSERT_AFTER_impl(insertAfterThis, &toInsert);
    unsigned int actualSize = 0;
    LIST_FOREACH(var, &headA, links) {
        actualSize++;
    }
    RC_ASSERT(actualSize == expectedSize);
}

RC_GTEST_PROP(LIST,
              insertingElementWithVal1ShouldIncreaseSumBy1,
              (std::vector<IntegerLISTEntry> a)){
    RC_SUCCEED_IF(a.empty());
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, a.size()-1);
    myLISTHead headA{nullptr};
    createList(headA, a);

    IntegerLISTEntry* var;
    unsigned int expectedSum = 1;
    LIST_FOREACH(var, &headA, links) {
        expectedSum+= var->data;
    }

    auto insertAfterThis = &a.at(distribution(generator));
    auto toInsert = IntegerLISTEntry{1, nullptr};
    LIST_INSERT_AFTER_impl(insertAfterThis, &toInsert);
    unsigned int actualSum = 0;
    LIST_FOREACH(var, &headA, links) {
        actualSum+=var->data;
    }
    RC_ASSERT(actualSum == expectedSum);
}

RC_GTEST_PROP(LIST,
              insertingAtHeadShouldIncreaseLengthBy1,
              (std::vector<IntegerLISTEntry> a)){
    myLISTHead headA{};
    createList(headA, a);

    IntegerLISTEntry* var;
    unsigned int expectedLength = a.size()+1;

    auto toInsert = IntegerLISTEntry{1, nullptr};
    LIST_INSERT_HEAD_impl(&headA, &toInsert);
    unsigned int actualLength = 0;
    LIST_FOREACH(var, &headA, links) {
        actualLength++;
    }
    RC_ASSERT(LIST_FIRST_impl(&headA) == &toInsert);
    RC_ASSERT(actualLength == expectedLength);
}

RC_GTEST_PROP(LIST,
              insertingHeadFromList2IntoList1ShouldMakeThemEqual,
              (std::vector<IntegerLISTEntry> a, std::vector<IntegerLISTEntry> b)){
    RC_SUCCEED_IF(b.empty());
    myLISTHead headA{nullptr};
    createList(headA, a);
    myLISTHead headB{nullptr};
    createList(headB, b);
    LIST_INSERT_HEAD_impl(&headA, headB.lh_first);

    RC_ASSERT(LIST_FIRST_impl(&headA) == LIST_FIRST_impl(&headB));
}

RC_GTEST_PROP(LIST,
              removingFromListWith1ElementShouldMakeEmpty,
              ()){
    myLISTHead headA{nullptr};

    auto a = std::vector<IntegerLISTEntry>{*rc::gen::arbitrary<IntegerLISTEntry>()};
    createList(headA, a);
    LIST_REMOVE_impl(&a.at(0));
    RC_ASSERT(LIST_FIRST_impl(&headA) == nullptr);
}

RC_GTEST_PROP(LIST,
              removingFromListShouldEventuallyMakeEmpty,
              (std::vector<IntegerLISTEntry> a)){
    std::default_random_engine generator;

    myLISTHead headA{};
    LIST_INIT_impl(&headA);
    createList(headA, a);
    IntegerLISTEntry* var;
    std::vector<IntegerLISTEntry*> pointers;

    for(int i = 0; i < a.size(); i++) {
        pointers.push_back(&a.at(i));
    }

    unsigned prev_size = a.size();
    while(prev_size) {
        std::uniform_int_distribution<int> distribution(0, prev_size-1);
        unsigned int removeIndex = distribution(generator);
        auto elementToRemove = pointers.at(removeIndex);
        LIST_REMOVE_impl(elementToRemove);
        pointers.erase(pointers.begin() + removeIndex);
        unsigned new_size = 0;
        LIST_FOREACH(var, &headA, links)  {
            new_size++;
        }
        RC_ASSERT(new_size == prev_size-1);
        prev_size = new_size;
    }
}

RC_GTEST_PROP(LIST,
              removingHeadFromListShouldEventuallyMakeEmpty,
              (std::vector<IntegerLISTEntry> a)){
    myLISTHead headA{};
    LIST_INIT_impl(&headA);
    createList(headA, a);
    IntegerLISTEntry* var;
    unsigned prev_size = a.size();
    while(headA.lh_first) {
        LIST_REMOVE_impl(headA.lh_first);
        unsigned new_size = 0;
        LIST_FOREACH(var, &headA, links)  {
            new_size++;
        }
        RC_ASSERT(new_size == prev_size-1);
        prev_size = new_size;
    }
}

RC_GTEST_PROP(LIST,
              removingHeadShouldYieldCorrectFirstElement,
              (std::vector<IntegerLISTEntry> a)){
    myLISTHead headA{nullptr};
    RC_SUCCEED_IF(a.empty());
    createList(headA, a);

    auto expectedNextElement = headA.lh_first->links.le_next;
    LIST_REMOVE_impl(headA.lh_first);
    RC_ASSERT(headA.lh_first == expectedNextElement);
}

RC_GTEST_PROP(LIST,
              removingElementNotPartOfListShouldNotMutateList,
              (std::vector<IntegerLISTEntry> a, std::vector<IntegerLISTEntry> b)){
    RC_SUCCEED_IF(b.size() < 2);
    myLISTHead headA{nullptr};
    createList(headA, a);

    myLISTHead headB{nullptr};
    createList(headB, b);

    LIST_REMOVE_impl(headB.lh_first);
    IntegerLISTEntry* var;
    unsigned int count  = 0;
    LIST_FOREACH(var, &headA, links) {
        RC_ASSERT(&(a.at(count)) == var);
        count++;
    }
}

RC_GTEST_PROP(LIST,
              afterRemovingElementPrevNextNextShouldBePrevNextNext,
              (std::vector<IntegerLISTEntry> a)){
    RC_SUCCEED_IF(a.size() <= 1);
    std::default_random_engine generator;
    myLISTHead headA{};
    LIST_INIT_impl(&headA);
    createList(headA, a);

    std::uniform_int_distribution<int> distribution(1, a.size()-1);
    unsigned int removeIndex = distribution(generator);
    IntegerLISTEntry* next;
    auto prev = &a.at(removeIndex-1);
    if(removeIndex >= a.size()-1)
        next = nullptr;
    else
        next = a.at(removeIndex).links.le_next;
    LIST_REMOVE_impl(&a.at(removeIndex));
    RC_ASSERT(prev->links.le_next == next);

}

