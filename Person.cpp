#include "Person.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream> 

const char* getAuthor() {
    return "Andy Liu";
}

Person::Person(Sex s, const std::string& n) : sex(s), name(n), mother(nullptr), father(nullptr) {}

Person::~Person() {
    // Unset child's parent pointers
    for (auto child : children)
    {
        child->father = nullptr;
        child->mother = nullptr;
    }

    // Unset parent's child pointers
    if (father)
    {
        father->removeChild(this);
        father = nullptr;
    }

    if (mother)
    {
        mother->removeChild(this);
        mother = nullptr;
    }
}

bool Person::setFather(Person* newFather) {
    if (newFather->sex != MALE){ // Ensures only MALES are fathers
        return false;
    }

    if (father == newFather) { // If father and newFather are the same, do nothing
        return true;
    } 
    
    if (father) { // If this person already has a father, remove this person from the original father
        this->father->removeChild(this);
    }

    if (newFather){
        father = newFather;
        newFather->addChild(this);
    }

    return true;
}

bool Person::setMother(Person* newMother) { // Same logic as setFather() function
    if (newMother->sex != FEMALE){ 
        return false;
    }

    if (mother == newMother) {
        return true;
    }

    if (mother) {
        this->mother->removeChild(this);
    }

    if (newMother) {
        mother = newMother;
        newMother->addChild(this);
    }

    return true;
}

bool Person::hasChild(const Person* child) const { // Returns true or false depending on if child is found or not by findChild
    return findChild(child) != children.end();
}

bool Person::addChild(Person* child) { 
    if (this->sex == MALE) { 
        if (child->father) { // If the child has an existing father, remove the child from them
            child->father->removeChild(child);
        } 
    }

    if (this->sex == FEMALE) {
        if (child->mother) { // If the child has an existing mother, remove the child from them
            child->mother->removeChild(child);
        }
    }
    
    if (!hasChild(child)) { // Checks that this person does not have the child being added, if not, adds child
        if (this->sex == MALE) {
            child->father = this;
        } 
        
        else {
            child->mother = this;
        }
    
        children.push_back(child);
        return true;
    }

    return false;
}

bool Person::removeChild(Person* child) {
    auto it = findChild(child);

    if (it != children.end()) { // Removes child if they exist in the persons children list
        children.erase(it);

        if (this->sex == MALE) {
            child->father = nullptr;
        } 
        
        else {
            child->mother = nullptr;
        } 
        return true;
    }

    return false;
}

void Person::removeAllChildren() {
    for (auto child : children) { // Iterates through the person's children list and removes all of them
        if (this->sex == MALE) {
            child->father = nullptr;
        } 
        
        else {
            child->mother = nullptr;
        } 
    }
    children.clear();
}

void Person::getAncestors(People& results) const { // Recursively navigates up through a person's bloodline to find ancestors
    if (father) {
        if(std::find(results.begin(), results.end(), father) == results.end()) { // This condition prevents repeat entries
            results.push_back(father);
            father->getAncestors(results);
        }
    }

    if (mother) {
        if(std::find(results.begin(), results.end(), mother) == results.end()) {
            results.push_back(mother);
            mother->getAncestors(results);
        }
    }
}

void Person::getDescendants(People& results) const { // Recursively navigates down through a person's bloodline to find descendants
    for (auto child : children) {
        if (std::find(results.begin(), results.end(), child) == results.end()) {
            results.push_back(child);
            child->getDescendants(results);
        }
    }
}

void Person::getSiblings(People& results) const { // Iterates through each parent's children to get all siblings
    if (father) {
        auto siblings = father->children;
        for (auto sibling : siblings) {
            if (std::find(results.begin(), results.end(), sibling) == results.end() && sibling != this) {
                results.push_back(sibling);
            }   
        }
    }

    if (mother) { 
        auto siblings = mother->children;
        for (auto sibling : siblings) {
            if (std::find(results.begin(), results.end(), sibling) == results.end() && sibling != this) {
                results.push_back(sibling);
            }
        }
    }
}

void Person::getCousins(People& results) const { // Iterates through the children of the siblings of parents to find cousins 
    if (father) {
        People siblings;
        father->getSiblings(siblings);
        for (auto sibling : siblings) {
            auto children = sibling->children;
            for (auto child : children) {
                if (std::find(results.begin(), results.end(), child) == results.end() && child != this) {
                    results.push_back(child);
                }   
            }
        }
    }

    if (mother) {
        People siblings;
        mother->getSiblings(siblings);
        for (auto sibling : siblings) {
            auto children = sibling->children;
            for (auto child : children) {
                if (std::find(results.begin(), results.end(), child) == results.end() && child != this) {
                    results.push_back(child);
                }
            }
        }
    }
}

People::iterator Person::findChild(const Person* child) {
    return std::find(children.begin(), children.end(), child);
}

People::const_iterator Person::findChild(const Person* child) const {
    return std::find(children.begin(), children.end(), child);
}

/* UNIT TEST SUITE TO VERIFY FUNCTION OUTPUTS AND CORRECTNESS
void testPerson() {
    // Test creating a person
    Person* p = new Person(Person::MALE, "John");
    Person* p1 = new Person(Person::FEMALE, "Johna");
    assert(p->name == "John");
    assert(p->sex == Person::MALE);

    // Test setting parents
    Person* father = new Person(Person::MALE, "Father");
    assert(father->name == "Father");
    Person* mother = new Person(Person::FEMALE, "Mother");
    assert(mother->name == "Mother");

    assert(p->setFather(father));
    assert(p->setMother(mother));
    assert(p->getFather() == father);
    assert(p->getMother() == mother);
    assert(father->hasChild(p));
    assert(mother->hasChild(p));

    // Test adding children
    Person* child1 = new Person(Person::MALE, "Child1");
    Person* child2 = new Person(Person::FEMALE, "Child2");
    Person* child3 = new Person(Person::FEMALE, "Child3");

    assert(p->addChild(child1));
    assert(p->addChild(child2));
    assert(p->addChild(child3));
    assert(p1->addChild(child1));
    assert(child1->getFather() == p);
    assert(child1->getMother() == p1);
    assert(child2->getFather() == p);
    assert(p->getNumChildren() == 3);

    // Test removing children
    assert(p->removeChild(child1));
    assert(p->getNumChildren() == 2);
    assert(child1->getFather() == nullptr);
    assert(child1->getMother() == p1);

    // Removing all children
    p->removeAllChildren();
    assert(p->getNumChildren() == 0);
    assert(child2->getFather() == nullptr);
    assert(child3->getFather() == nullptr);
    assert(child2->getMother() == nullptr);
    assert(child3->getMother() == nullptr);

    // Swapping Parents
    assert(child1->setMother(mother));
    assert(child1->getMother() == mother);

    // Testing Ancestors
    Person* grandparent1 = new Person(Person::MALE, "Grandfather1");
    Person* grandparent2 = new Person(Person::FEMALE, "Grandmother1");
    Person* parent = new Person(Person::MALE, "Father");
    Person* child = new Person(Person::FEMALE, "Child");
    Person* extra_child = new Person(Person::FEMALE, "chile");
    Person* parent2 = new Person(Person::FEMALE, "Mother");
    Person* cousin = new Person(Person::FEMALE, "Cousin");

    parent->setFather(grandparent1);
    parent->setMother(grandparent2);
    parent2->setFather(grandparent1);
    parent2->setMother(grandparent2);

    child->setFather(parent);
    cousin->setMother(parent2);
    extra_child->setFather(parent);

    People ancestors;

    child->getAncestors(ancestors);

    assert(ancestors.size() == 3);
    assert(ancestors[0]->name == "Father");
    assert(ancestors[1]->name == "Grandfather1");
    assert(ancestors[2]->name == "Grandmother1");

    child->getAncestors(ancestors);
    assert(ancestors.size() == 3);

    // Testing Descendants
    People descendants;

    grandparent1->getDescendants(descendants);
    assert(descendants.size() == 5);
    assert(descendants[0]->name == "Father");
    assert(descendants[1]->name == "Child");
    assert(descendants[2]->name == "chile");

    grandparent1->getDescendants(descendants);
    assert(descendants.size() == 5);

    // Testing Siblings
    People siblings;
    child->getSiblings(siblings);
    assert(siblings.size() == 1);
    assert(siblings[0]->name == "chile");

    child->setMother(mother);
    child->getSiblings(siblings);
    assert(siblings.size() == 3);
    assert(siblings[0]->name == "chile");
    assert(siblings[1]->name == "John");
    assert(siblings[2]->name == "Child1");

    // Testing Cousins
    People cousins;

    extra_child->getCousins(cousins);
    assert(cousins.size() == 1);
    assert(cousins[0]->name == "Cousin");

    delete p;
    delete p1;
    delete father;
    delete mother;
    delete child1;
    delete child2;
    delete child3;
    delete grandparent1;
    delete grandparent2;
    delete parent;
    delete child;
    delete extra_child;
    delete parent2;
    delete cousin;
} */ 