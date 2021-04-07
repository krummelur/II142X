//
// Created by fredrik on 2021-04-07.
//

//
// Created by fredrik on 2021-04-04.
//
#include "../src/queue_impl.h"
#include "../src/queue.h"
#include "gtest/gtest.h"
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <algorithm>
#include <cstdlib>
#include <random>

template<>
struct rc::Arbitrary<IntegerTAILQueueNode> {
    static Gen<IntegerTAILQueueNode> arbitrary() {
        return gen::build<IntegerTAILQueueNode>(
                gen::set(&IntegerTAILQueueNode::data, gen::arbitrary<int>()));
    }
};

void createList(myTAILQueueHead &head,std::vector<IntegerTAILQueueNode> &ents) {

    TAILQ_INIT_impl(&head);

    if(ents.empty())
        return;

    TAILQ_INSERT_HEAD(&head, &ents.at(0), entries);

    for (std::size_t i = 1; i < ents.size(); i++)
        TAILQ_INSERT_TAIL_impl(&head, &ents.at(i));

}

RC_GTEST_PROP(TAILQ,
              concatenatingTAILQLists,
              (std::vector<IntegerTAILQueueNode> a, std::vector<IntegerTAILQueueNode> b)){
    myTAILQueueHead headA;
    createList(headA, a);
    myTAILQueueHead headB;
    createList(headB, b);
    IntegerTAILQueueNode* temp;
    IntegerTAILQueueNode* first = TAILQ_FIRST_impl(&headA);
    IntegerTAILQueueNode* second = TAILQ_FIRST_impl(&headB);
    unsigned int actualSize = 0;
    unsigned int expectedSize = a.size() + b.size();

    TAILQ_CONCAT_impl(&headA, &headB);

    TAILQ_FOREACH(temp, &headA, entries) {
        actualSize++;
    }

    EXPECT_EQ(true, TAILQ_EMPTY_impl(&headB));
    EXPECT_EQ(expectedSize, actualSize);
    if(a.size() > 0) EXPECT_EQ(first, TAILQ_FIRST_impl(&headA));
    else EXPECT_EQ(second, TAILQ_FIRST_impl(&headA));
}


TEST(TAILQ, isInitiallyEmpty) {
    myTAILQueueHead head;
    TAILQ_INIT_impl(&head);
    EXPECT_EQ (true, TAILQ_EMPTY_impl(&head));
}

RC_GTEST_PROP(TAILQ, returnsFirstElement, (std::vector<IntegerTAILQueueNode> a)) {

    myTAILQueueHead head;
    createList(head, a);
    IntegerTAILQueueNode* first = TAILQ_FIRST_impl(&head);

    if(!TAILQ_EMPTY_impl(&head)) EXPECT_EQ(first, &a.at(0));

}

TEST(TAILQ, initWorksCorrectly) {
    myTAILQueueHead head;
    TAILQ_INIT_impl(&head);
    EXPECT_EQ (true, TAILQ_EMPTY_impl(&head));
    EXPECT_EQ(head.tqh_first, *head.tqh_last);
}

RC_GTEST_PROP(TAILQ, isInsertedAfter, (std::vector<IntegerTAILQueueNode> a)) {

    myTAILQueueHead head;
    IntegerTAILQueueNode after;
    int index;
    createList(head, a);

    if (a.size() > 0) {
        index = std::rand() % a.size();
        TAILQ_INSERT_AFTER_impl(&head, &a.at(index), &after);
        EXPECT_EQ (a.at(index).entries.tqe_next, &after);
    }
}

RC_GTEST_PROP(TAILQ, isInsertedBefore, (std::vector<IntegerTAILQueueNode> a)) {

    myTAILQueueHead head;
    IntegerTAILQueueNode before;
    int index;
    createList(head, a);

    if (a.size() > 0) {
        index = std::rand() % a.size();
        TAILQ_INSERT_BEFORE_impl(&a.at(index), &before);
        EXPECT_EQ (&a.at(index), before.entries.tqe_next);
    }
}


RC_GTEST_PROP(TAILQ, headIsInserted, (std::vector<IntegerTAILQueueNode> a)) {

    myTAILQueueHead head;
    IntegerTAILQueueNode newHead;
    createList(head, a);

    TAILQ_INSERT_HEAD_impl(&head, &newHead);

    EXPECT_EQ (&newHead, TAILQ_FIRST_impl(&head));
}

RC_GTEST_PROP(TAILQ, tailIsInserted, (std::vector<IntegerTAILQueueNode> a)) {

    myTAILQueueHead head;
    IntegerTAILQueueNode tail;
    createList(head, a);

    TAILQ_INSERT_TAIL_impl(&head, &tail);

    EXPECT_EQ (&tail, TAILQ_LAST_impl(&head));
}

RC_GTEST_PROP(TAILQ, returnsLastElement, (std::vector<IntegerTAILQueueNode> a)) {

//    myTAILQueueHead head;
//    IntegerTAILQueueNode last;
//    createList(head, a);
//
//    TAILQ_INSERT_TAIL_impl(&head, &last);
//
//    if(a.size() > 0)
//        EXPECT_EQ (TAILQ_LAST_impl(&head), &a.at(a.size()-1));
}


//RC_GTEST_PROP(STAILQ, nextElementIsCorrect, (std::vector<IntegerSTAILQueueNode> a)) {
//
//    mySTAILQueueHead head;
//    IntegerSTAILQueueNode* next;
//    createList(head, a);
//    int index = 1;
//
//    STAILQ_FOREACH(next, &head, entries){
//        if (next && index < a.size())EXPECT_EQ (STAILQ_NEXT_impl(next), &a.at(index));
//        index++;
//    }
//
//
//}
//
//TEST(STAILQ, becomesEmptyWhenElementRemoved) {
//    IntegerSTAILQueueNode entry;
//    mySTAILQueueHead head;
//    STAILQ_INIT_impl(&head);
//    STAILQ_INSERT_HEAD_impl(&head, &entry);
//    STAILQ_REMOVE_impl(&head, &entry);
//    EXPECT_EQ (true, STAILQ_EMPTY_impl(&head));
//}
//
//RC_GTEST_PROP(STAILQ, elementIsRemoved, (std::vector<IntegerSTAILQueueNode> a)) {
//
//    mySTAILQueueHead head;
//    IntegerSTAILQueueNode* temp;
//    int index;
//    int actualSize = 0;
//    createList(head, a);
//
//    if (a.size() > 0) {
//        index = std::rand() % a.size();
//
//        STAILQ_REMOVE_impl(&head, &a.at(index));
//
//        STAILQ_FOREACH(temp, &head, entries) {
//            actualSize++;
//        }
//        EXPECT_EQ(a.size(), actualSize + 1);
//    }
//}
//
//RC_GTEST_PROP(STAILQ, elementAfterIsRemoved, (std::vector<IntegerSTAILQueueNode> a)) {
//
//    mySTAILQueueHead head;
//    IntegerSTAILQueueNode* temp;
//    int actualSize = 0;
//    createList(head, a);
//
//    if (a.size() > 2) {
//        STAILQ_REMOVE_AFTER_impl(&head, &a.at(0));
//
//        STAILQ_FOREACH(temp, &head, entries) {
//            actualSize++;
//        }
//        EXPECT_EQ(head.stqh_first->entries.stqe_next, &a.at(2));
//        EXPECT_EQ(a.size(), actualSize + 1);
//    }
//}
//
//RC_GTEST_PROP(STAILQ, removesHead, (std::vector<IntegerSTAILQueueNode> a)) {
//
//    mySTAILQueueHead head;
//    createList(head, a);
//
//    if(a.size() > 0)STAILQ_REMOVE_HEAD_impl(&head);
//    if(a.size() > 1)EXPECT_EQ(STAILQ_FIRST_impl(&head), &a.at(1));
//    else if (a.size() == 1) RC_ASSERT(STAILQ_EMPTY_impl(&head));
//}
//
//
//RC_GTEST_PROP(STAILQ, doubleSwappingSTAILQAlwaysGivesInitialLists, (std::vector<IntegerSTAILQueueNode> a, std::vector<IntegerSTAILQueueNode> b)){
//    mySTAILQueueHead headA;
//    createList(headA, a);
//    mySTAILQueueHead headB;
//    createList(headB, b);
//    IntegerSTAILQueueNode* var;
//    unsigned int size = 0;
//    STAILQ_FOREACH(var, &headA, entries) {
//        RC_ASSERT(var->data == a.at(size).data);
//        EXPECT_EQ(var, &a.at(size));
//        size++;
//    }
//    EXPECT_EQ(size, a.size());
//
//    size = 0;
//    STAILQ_FOREACH(var, &headB, entries) {
//        RC_ASSERT(var->data == b.at(size).data);
//        EXPECT_EQ(var, &b.at(size));
//        size++;
//    }
//    EXPECT_EQ(size, b.size());
//}
//
//
//RC_GTEST_PROP(STAILQ, endIsAlwaysNull,
//              (std::vector<IntegerSTAILQueueNode> a)) {
//    mySTAILQueueHead head;
//    createList(head, a);
//    EXPECT_EQ(STAILQ_END_impl(&head), nullptr);
//}