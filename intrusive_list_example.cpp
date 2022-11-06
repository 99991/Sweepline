#include "intrusive_list.hpp"
#include <stdio.h>
#include <assert.h>

struct Person {
    IntrusiveNode<Person> node0;
    IntrusiveNode<Person> node1;

    const char *name;
    Person(const char *name): name(name){}
};

int main(){
    IntrusiveList<Person, &Person::node0> l0;
    IntrusiveList<Person, &Person::node1> l1;

    Person karl("Karl");
    Person hugo("Hugo");
    Person donald("Donald");
    Person gustav("Gustav");

    l0.push_front(karl);
    l0.push_back(donald);
    l0.push_front(hugo);
    l0.push_back(gustav);
    
    l1.push_front(hugo);
    l1.push_front(gustav);

    for (const Person &person : l0){
        printf("%s\n", person.name);
    }
    printf("\n");
    for (const Person &person : l1){
        printf("%s\n", person.name);
    }
    printf("\n");

    auto it2 = l0.begin();
    ++it2;
    it2 = l0.erase(it2);

    printf("Value after erased value: %s\n", it2->name);
    
    for (const Person &person : l0){
        printf("%s\n", person.name);
    }
    printf("\n");
    for (const Person &person : l1){
        printf("%s\n", person.name);
    }
    printf("\n");

    const IntrusiveList<Person, &Person::node0> &cl = l0;
    for (const Person &person : cl){
        printf("%s\n", person.name);
    }
    printf("\n");

    while (!l0.empty()){
        l0.erase(l0.begin());
    }
    
    l0.push_front(karl);
    l0.push_back(donald);
    l0.push_front(hugo);
    l0.push_back(gustav);
    
    while (!l0.empty()){
        l0.pop_front();
    }
    
    l0.push_back(donald);

    printf("List with Donald:\n");
    for (const Person &person : l0){
        printf("%s\n", person.name);
    }
    printf("\n");

    IntrusiveList<Person, &Person::node0> stolen;

    stolen = l0;

    assert(l0.empty());

    printf("Stolen list with Donald:\n");
    for (const Person &person : stolen){
        printf("%s\n", person.name);
    }
    printf("\n");
}
